#include "video.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
namespace fs = std::filesystem;

libtrainsim::Video::simulatorWindowGLArea::simulatorWindowGLArea(std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings): Gtk::GLArea{},decode{simSettings}{
    set_required_version(3,3);
    set_auto_render(false);
}

void libtrainsim::Video::simulatorWindowGLArea::on_realize() {
    Gtk::GLArea::on_realize();

    auto ctx = get_context();

    tickID = add_tick_callback(sigc::mem_fun(*this, &simulatorWindowGLArea::on_tick));

    make_current();
    throw_if_error();

    textureProperties bgProps{};
    bgProps.name = "background";
    auto bgTexture = std::make_shared<texture>(bgProps);
    bgTexture->finish(ctx);
    addTexture(bgTexture);

    try{
        texUnits = SimpleGFX::SimpleGL::GLHelper::getMaxTextureUnits();
        loadBuffers();
    }catch(...){
        std::throw_with_nested(std::runtime_error("error creating data buffers"));
    }

    //load the display shader and set the default values
    try{
        generateDisplayShader();
    }catch(const std::exception& e){
        std::throw_with_nested(std::runtime_error("cannot init display shader"));
    }

    //for(auto tex: displacements){
    //    tex->finish(ctx);
    //}
    //bg->finish(ctx);

    realized = true;
}

void libtrainsim::Video::simulatorWindowGLArea::on_unrealize() {
    if(!realized){
        return;
    }

    remove_tick_callback(tickID);

    make_current();
    if(has_error()){
        return;
    }

    //unrealize all opengl stuff while the context still exists

    displayTextures[0]->freeGL();
    displayTextures.clear();
    displayShader.reset();

    realized = false;
    Gtk::GLArea::on_unrealize();
}

void libtrainsim::Video::simulatorWindowGLArea::loadBuffers() {
    if(VAO != 0 || VBO != 0 || EBO != 0){
        return;
    }
    std::scoped_lock lock{dataMutex};

    //---------------init vertex buffers---------------
    float vertices[] = {
        // position           // texture coords
        1.0f,  1.0f,   1.0f, 0.0f, // top right
        1.0f, -1.0f,   1.0f, 1.0f, // bottom right
        -1.0f, -1.0f,   0.0f, 1.0f, // bottom left
        -1.0f,  1.0f,   0.0f, 0.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    //create all of the blit buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void libtrainsim::Video::simulatorWindowGLArea::generateDisplayShader(){

    auto maxTextureUnits = SimpleGFX::SimpleGL::GLHelper::getMaxTextureUnits();
    std::stringstream fragmentSource;
    fragmentSource << R""""(
        #version 330 core
        layout(location = 0) out vec4 FragColor;
        in vec2 TexCoord;
        in vec2 Coord;
    )"""";

    fragmentSource << "    uniform sampler2D tex[" << maxTextureUnits << "];\n";
    fragmentSource << R""""(
        uniform uint enabledUnits;
        void main(){
            vec4 outColor;
            FragColor = vec4(0.0,0.0,0.0,0.0);
    )"""";

    std::vector<int> units{};
    units.reserve(maxTextureUnits);
    for(unsigned int i = 0; i < maxTextureUnits; i++){
        units.emplace_back(i);
        fragmentSource << "if(enabledUnits > " << i << "u){";
        fragmentSource << "    outColor = texture(tex[" << i << "], TexCoord);";
        fragmentSource << "    FragColor = mix(FragColor, outColor, outColor.a);";
        fragmentSource << "}else{return;}";
    }

    fragmentSource << "}" << std::endl;

    shaderConfiguration disp_conf{
        defaultShaderSources::getBasicVertexSource(),
        fragmentSource.str()
    };

    displayShader = std::make_shared<shader>(disp_conf);

    displayShader->use();
    displayShader->setUniform("tex", units);

}

bool libtrainsim::Video::simulatorWindowGLArea::on_tick ( const Glib::RefPtr<Gdk::FrameClock>& frame_clock ) {
    queue_render();
    return true;
}


bool libtrainsim::Video::simulatorWindowGLArea::on_render ( const Glib::RefPtr<Gdk::GLContext>& context ) {
    Gtk::GLArea::on_render(context);

    std::scoped_lock lock{dataMutex};

    displayTextures[0]->updateImage(decode.getUsableFramebufferBuffer(), decode.getDimensions());

    glClearColor (0, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT);

    displayShader->use();
    auto orth = glm::ortho(
        -1.0f,
        1.0f,
        -1.0f,
        1.0f,
        -10.0f,
        10.0f
    );
    displayShader->setUniform("transform", orth);
    displayShader->setUniform("enabledUnits", displayTextures.size());

    for(unsigned int i = 0; i < displayTextures.size(); i++){
        displayTextures[i]->bind(i);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();

    return TRUE;
}

void libtrainsim::Video::simulatorWindowGLArea::addTexture ( std::shared_ptr<texture> newTexture ) {
    if(displayTextures.size() == texUnits){
        std::stringstream ss;
        ss << "For now only ";
        ss << texUnits;
        ss << " display textures are supported, remove one to add this one!";
        throw std::runtime_error(ss.str());
    }

    auto texName = newTexture->getName();

    for(auto x:displayTextures){
        if(x->getName() == texName){
            throw std::invalid_argument("a texture with this name already exists");
        }
    }

    displayTextures.emplace_back(newTexture);
}

void libtrainsim::Video::simulatorWindowGLArea::removeTexture ( const std::string& textureName ) {
    if(textureName == "background"){
        throw std::invalid_argument("the background texture cannot be removed");
    }

    for(auto i = displayTextures.begin(); i < displayTextures.end(); i++){
        if((*i)->getName() == textureName){
            displayTextures.erase(i);
            return;
        }
    }

    throw std::invalid_argument("a texture with the name '" + textureName + "' does not exist");
}

std::optional<std::vector<sakurajin::unit_system::time_si>> libtrainsim::Video::simulatorWindowGLArea::getNewRendertimes() {
    return  decode.getNewRendertimes();
}

void libtrainsim::Video::simulatorWindowGLArea::gotoFrame ( uint64_t frame_num ) {
    decode.requestFrame(frame_num);
}



libtrainsim::Video::videoManager::videoManager(
    const std::string& windowName,
    std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings
):Gtk::Window{}{

    set_title(windowName);
    set_default_size(1280, 720);

    mainGLArea = Gtk::make_managed<simulatorWindowGLArea>(simSettings);

    areaFrame = Gtk::make_managed<Gtk::AspectFrame>();
    set_child(*areaFrame);
    auto [w,h] = mainGLArea->getDecoder().getDimensions();
    areaFrame->set_ratio(w/h);


    areaFrame->set_child(*mainGLArea);

    keyboardController = Gtk::EventControllerKey::create();
    keyboardController->signal_key_pressed().connect([this](guint keyval, guint keycode, Gdk::ModifierType state){
        if(keyval == GDK_KEY_Escape){
            close();
            return true;
        }
        if(keyval == GDK_KEY_F10){
            if(is_fullscreen()){
                unfullscreen();
            }else{
                fullscreen();
            }
            return true;
        }

        return false;
    },false);
    add_controller(keyboardController);

}

libtrainsim::Video::videoManager::~videoManager(){
    std::cout << "locking video manager to prevent draw calls while destroying" << std::endl;
    std::scoped_lock<std::shared_mutex> lock{videoMutex};

}

void libtrainsim::Video::videoManager::gotoFrame ( uint64_t frame_num ) {
    mainGLArea->gotoFrame(frame_num);
}

void libtrainsim::Video::videoManager::addTexture ( std::shared_ptr<texture> newTexture ) {
        try{
        mainGLArea->addTexture(newTexture);
    }catch(...){
        std::throw_with_nested(std::runtime_error("error adding texture"));
    }
}

void libtrainsim::Video::videoManager::removeTexture ( const std::string& textureName ) {
    try{
        mainGLArea->removeTexture(textureName);
    }catch(...){
        std::throw_with_nested(std::runtime_error("error removing texture"));
    }
}

std::optional<std::vector<sakurajin::unit_system::time_si>> libtrainsim::Video::videoManager::getNewRendertimes() {
    return mainGLArea->getNewRendertimes();
}


