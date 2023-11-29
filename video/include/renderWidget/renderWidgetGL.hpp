#pragma once

#include "renderWidget/displayFragShader.hpp"
#include "renderWidget/renderWidgetBase.hpp"

namespace libtrainsim {
    namespace Video {
        template <videoDecoderClass decoderClass>
        class LIBTRAINSIM_EXPORT_MACRO renderWidgetGL : public renderWidgetBase<decoderClass> {
          private:
            Gtk::GLArea mainGLArea;

            std::atomic<bool>         realized = false;
            std::atomic<unsigned int> texUnits = 0;

            unsigned int VBO = 0, VAO = 0, EBO = 0;

            std::shared_mutex GLDataMutex;

            /**
             * @brief the shader used to render the video into a texture
             */
            std::shared_ptr<SimpleGFX::SimpleGL::shaderProgram> displayShader = nullptr;

            // all the textures that are displayed on the output texture
            std::vector<std::shared_ptr<SimpleGFX::SimpleGL::sglTexture>> displayTextures;

          public:
            template <typename... decoderArgs>
            explicit renderWidgetGL(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                    std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher,
                                    decoderArgs&&... decoder_args)
                : libtrainsim::Video::renderWidgetBase<videoDecoderLibav>{std::move(_simSettings),
                                                                          std::move(_mainAppLauncher),
                                                                          &decoder_args...},
                  mainGLArea{} {

                mainGLArea.set_expand(true);
                mainGLArea.set_has_depth_buffer(false);
                mainGLArea.set_has_stencil_buffer(false);
                mainGLArea.set_auto_render(true);

                mainGLArea.signal_realize().connect(sigc::mem_fun(*this, &renderWidgetGL::on_realize_glarea));
                mainGLArea.signal_unrealize().connect(sigc::mem_fun(*this, &renderWidgetGL::on_unrealize_glarea), true);
                mainGLArea.signal_render().connect(sigc::mem_fun(*this, &renderWidgetGL::on_render_glarea), false);

                this->set_child(mainGLArea);
            }

            /**
             * @brief adds a texture to be rendered on top of the video
             */
            void addTexture(std::shared_ptr<SimpleGFX::SimpleGL::sglTexture> newTexture) {
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
                this->mainAppLauncher->callDeffered(sigc::mem_fun(*this, &renderWidgetGL::queue_draw));
            }

            /**
             * @brief remove a texture from being rendered
             */
            void removeTexture(const std::string& textureName) {
                if (textureName == "background") {
                    throw std::invalid_argument("the background texture cannot be removed");
                }

                for (auto i = displayTextures.begin(); i < displayTextures.end(); i++) {
                    if ((*i)->getName() == textureName) {
                        displayTextures.erase(i);
                        this->mainAppLauncher->callDeffered(sigc::mem_fun(*this, &renderWidgetGL::queue_draw));
                        return;
                    }
                }

                throw std::invalid_argument("a texture with the name '" + textureName + "' does not exist");
            }

          protected:
            void on_realize_glarea() {
                mainGLArea.make_current();
                Glib::RefPtr<Gdk::GLContext> ctx;
                try {
                    mainGLArea.throw_if_error();
                    ctx = mainGLArea.get_context();

                    int major, minor;
                    ctx->get_version(major, minor);
                    bool useES = ctx->get_use_es();
                    *(this->LOGGER) << SimpleGFX::loggingLevel::detail << "Context created with version " << major << "." << minor
                                    << (useES ? " ES" : " CORE");

                    SimpleGFX::SimpleGL::glErrorCheck();
                } catch (...) {
                    this->LOGGER->logCurrrentException(true);
                    std::throw_with_nested(std::runtime_error("cannot create GL context"));
                }

                texUnits = SimpleGFX::SimpleGL::getMaxTextureUnits();

                SimpleGFX::SimpleGL::sglTextureProperties bgProps{"", true, "background"};
                auto                                      bgTexture = std::make_shared<SimpleGFX::SimpleGL::sglTexture>(bgProps);

                try {
                    loadBuffers();
                } catch (...) {
                    this->LOGGER->logCurrrentException(true);
                    std::throw_with_nested(std::runtime_error("error creating data buffers"));
                }

                // load the display shader and set the default values
                try {
                    generateDisplayShader(ctx);
                } catch (...) {
                    this->LOGGER->logCurrrentException(true);
                    std::throw_with_nested(std::runtime_error("cannot init display shader"));
                }

                bgTexture->finish(ctx);
                addTexture(bgTexture);

                realized = true;
            }

            void on_unrealize_glarea() {
                if (!realized) {
                    return;
                }

                std::scoped_lock lock{this->GLDataMutex};

                mainGLArea.make_current();
                if (mainGLArea.has_error()) {
                    return;
                }

                // unrealize all opengl stuff while the context still exists

                displayTextures[0]->freeGL();
                displayTextures.clear();
                displayShader.reset();

                this->realized = false;
            }

            void loadBuffers() {
                if (VAO != 0 || VBO != 0 || EBO != 0) {
                    return;
                }
                std::scoped_lock lock{GLDataMutex};

                //---------------init vertex buffers---------------
                float vertices[] = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f};
                unsigned int indices[] = {0, 1, 3, 1, 2, 3};

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

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindVertexArray(0);

                glFlush();
            }

            // generate the source of the display shader and compile it
            void generateDisplayShader(Glib::RefPtr<Gdk::GLContext> ctx) {
                auto vert = SimpleGFX::SimpleGL::DefaultShaders::basicVertex::getInstance();
                auto frag = std::make_shared<displayFragShader>(texUnits.load());

                displayShader = std::make_shared<SimpleGFX::SimpleGL::shaderProgram>();
                displayShader->addPart(vert);
                displayShader->addPart(frag);
                try {
                    displayShader->link(ctx);
                } catch (...) {
                    this->LOGGER->logCurrrentException(true);
                    std::throw_with_nested(std::runtime_error("cannot link display shader"));
                }

                std::vector<int> units{};
                units.reserve(texUnits);
                for (unsigned int i = 0; i < texUnits; i++) {
                    units.emplace_back(i);
                }

                displayShader->use(ctx);
                // displayShader->useUnsafe();
                displayShader->setUniform("tex", units);
                glFlush();
            }

            bool on_render_glarea(const Glib::RefPtr<Gdk::GLContext>& context) {
                if (!realized) {
                    return FALSE;
                }

                std::scoped_lock lock{GLDataMutex};

                try {
                    SimpleGFX::SimpleGL::glErrorCheck();
                } catch (...) {
                    this->LOGGER->logCurrrentException(true);
                }

                if (this->decode.hasNewFramebuffer()) {
                    displayTextures[0]->updateImage(context, this->decode.getUsablePixbuf());
                }

                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);

                try {
                    displayShader->use(context);
                    // displayShader->useUnsafe();
                } catch (...) {
                    this->LOGGER->logCurrrentException(false);
                    return FALSE;
                }
                auto orth = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f);
                displayShader->setUniform("transform", orth);
                displayShader->setUniform("enabledUnits", displayTextures.size());

                for (unsigned int i = 0; i < displayTextures.size(); i++) {
                    displayTextures[i]->bind(context, i);
                }

                // int oldVAO = 0;
                // glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVAO);

                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                glFlush();

                // glBindVertexArray(oldVAO);
                // glUseProgram (0);

                return TRUE;
            }
        };
    } // namespace Video
} // namespace libtrainsim
