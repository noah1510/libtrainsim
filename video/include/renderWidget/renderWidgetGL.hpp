#pragma once

#include "renderWidget/displayFragShader.hpp"
#include "renderWidget/renderWidgetBase.hpp"

#include "videoDecode/videoDecoderLibav.hpp"

namespace libtrainsim {
    namespace Video {
        class LIBTRAINSIM_EXPORT_MACRO renderWidgetGL : public renderWidgetBase<videoDecoderLibav> {
          private:
            Gtk::GLArea mainGLArea;

            std::atomic<bool>         realized = false;
            std::atomic<unsigned int> texUnits = 0;

            unsigned int VBO = 0, VAO = 0, EBO = 0;

            std::shared_mutex GLDataMutex;

            void loadBuffers();

            // generate the source of the display shader and compile it
            void generateDisplayShader(Glib::RefPtr<Gdk::GLContext> ctx);

            /**
             * @brief the shader used to render the video into a texture
             */
            std::shared_ptr<SimpleGFX::SimpleGL::shaderProgram> displayShader = nullptr;

            // all the textures that are displayed on the output texture
            std::vector<std::shared_ptr<SimpleGFX::SimpleGL::sglTexture>> displayTextures;

          protected:
            bool on_render_glarea(const Glib::RefPtr<Gdk::GLContext>& context);
            void on_realize_glarea();
            void on_unrealize_glarea();

          public:
            explicit renderWidgetGL(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                    std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher);

            /**
             * @brief adds a texture to be rendered on top of the video
             */
            void addTexture(std::shared_ptr<SimpleGFX::SimpleGL::sglTexture> newTexture);

            /**
             * @brief remove a texture from being rendered
             */
            void removeTexture(const std::string& textureName);
        };
    } // namespace Video
} // namespace libtrainsim
