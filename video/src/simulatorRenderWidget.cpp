#include "simulatorRenderWidget.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
namespace fs = std::filesystem;

libtrainsim::Video::simulatorRenderWidget::simulatorRenderWidget(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings)
    : Gtk::GLArea{},
      simSettings{std::move(_simSettings)},
      decode{simSettings} {
    set_use_es(true);
    set_required_version(3, 2);
    set_auto_render(false);
}

void libtrainsim::Video::simulatorRenderWidget::on_realize() {
    Gtk::GLArea::on_realize();

    auto ctx = get_context();

    make_current();
    throw_if_error();

    texUnits = SimpleGFX::SimpleGL::GLHelper::getMaxTextureUnits();

    textureProperties bgProps{};
    bgProps.name   = "background";
    auto bgTexture = std::make_shared<texture>(bgProps);
    bgTexture->finish(ctx);
    addTexture(bgTexture);

    try {
        loadBuffers();
    } catch (...) {
        simSettings->getLogger()->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error creating data buffers"));
    }

    // load the display shader and set the default values
    try {
        generateDisplayShader();
    } catch (...) {
        simSettings->getLogger()->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("cannot init display shader"));
    }

    realized = true;
}

void libtrainsim::Video::simulatorRenderWidget::on_unrealize() {
    if (!realized) {
        return;
    }

    std::scoped_lock lock{GLDataMutex};

    make_current();
    if (has_error()) {
        return;
    }

    // unrealize all opengl stuff while the context still exists

    displayTextures[0]->freeGL();
    displayTextures.clear();
    displayShader.reset();

    realized = false;
    Gtk::GLArea::on_unrealize();
}

void libtrainsim::Video::simulatorRenderWidget::loadBuffers() {
    if (VAO != 0 || VBO != 0 || EBO != 0) {
        return;
    }
    std::scoped_lock lock{GLDataMutex};

    //---------------init vertex buffers---------------
    float        vertices[] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f};
    unsigned int indices[]  = {0, 1, 3, 1, 2, 3};

    // create all the blit buffers
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

void libtrainsim::Video::simulatorRenderWidget::generateDisplayShader() {

    auto              maxTextureUnits = SimpleGFX::SimpleGL::GLHelper::getMaxTextureUnits();
    std::stringstream fragmentSource;
    fragmentSource << R""""(
        #version 320 es
        precision mediump float;
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
    for (unsigned int i = 0; i < maxTextureUnits; i++) {
        units.emplace_back(i);
        fragmentSource << "if(enabledUnits > " << i << "u){";
        fragmentSource << "    outColor = texture(tex[" << i << "], TexCoord);";
        fragmentSource << "    FragColor = mix(FragColor, outColor, outColor.a);";
        fragmentSource << "}else{return;}";
    }

    fragmentSource << "}" << std::endl;

    shaderConfiguration disp_conf{defaultShaderSources::getBasicVertexSource(GL_320ES), fragmentSource.str()};

    displayShader = std::make_shared<shader>(disp_conf);

    displayShader->use();
    displayShader->setUniform("tex", units);
}

bool libtrainsim::Video::simulatorRenderWidget::on_render(const Glib::RefPtr<Gdk::GLContext>& context) {
    if (!realized) {
        return FALSE;
    }

    std::scoped_lock lock{GLDataMutex};

    Gtk::GLArea::on_render(context);

    displayTextures[0]->updateImage(decode.getUsableFramebufferBuffer(), decode.getDimensions());

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    displayShader->use();
    auto orth = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f);
    displayShader->setUniform("transform", orth);
    displayShader->setUniform("enabledUnits", displayTextures.size());

    for (unsigned int i = 0; i < displayTextures.size(); i++) {
        displayTextures[i]->bind(i);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();

    return TRUE;
}

void libtrainsim::Video::simulatorRenderWidget::addTexture(std::shared_ptr<texture> newTexture) {
    if (displayTextures.size() == texUnits) {
        std::stringstream ss;
        ss << "For now only ";
        ss << texUnits;
        ss << " display textures are supported, remove one to add this one!";
        throw std::runtime_error(ss.str());
    }

    auto texName = newTexture->getName();

    for (const auto& x : displayTextures) {
        if (x->getName() == texName) {
            throw std::invalid_argument("a texture with this name already exists");
        }
    }

    displayTextures.emplace_back(std::move(newTexture));
}

void libtrainsim::Video::simulatorRenderWidget::removeTexture(const std::string& textureName) {
    if (textureName == "background") {
        throw std::invalid_argument("the background texture cannot be removed");
    }

    for (auto i = displayTextures.begin(); i < displayTextures.end(); i++) {
        if ((*i)->getName() == textureName) {
            displayTextures.erase(i);
            return;
        }
    }

    throw std::invalid_argument("a texture with the name '" + textureName + "' does not exist");
}

std::optional<std::vector<sakurajin::unit_system::time_si>> libtrainsim::Video::simulatorRenderWidget::getNewRendertimes() {
    return decode.getNewRendertimes();
}

void libtrainsim::Video::simulatorRenderWidget::gotoFrame(uint64_t frame_num) {
    //queue a redraw if the requested frame is newer than the currently displayed one.
    if(decode.requestFrame(frame_num)){
        queue_render();
    }
}

libtrainsim::Video::videoReader& libtrainsim::Video::simulatorRenderWidget::getDecoder() {
    return decode;
}
