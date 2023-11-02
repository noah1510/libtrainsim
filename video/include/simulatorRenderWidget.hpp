#pragma once

#include "video_reader.hpp"

namespace libtrainsim {
    namespace Video {
        class simulatorRenderWidget : public Gtk::GLArea {
          private:
            std::atomic<bool>         realized = false;
            std::atomic<unsigned int> texUnits = 0;

            unsigned int VBO = 0, VAO = 0, EBO = 0;

            std::shared_mutex GLDataMutex;

            void loadBuffers();

            // generate the source of the display shader and compile it
            void generateDisplayShader();

            /**
             * @brief the shader used to render the video into a texture
             */
            std::shared_ptr<SimpleGFX::SimpleGL::shader> displayShader = nullptr;

            // all the textures that are displayed on the output texture
            std::vector<std::shared_ptr<SimpleGFX::SimpleGL::texture>> displayTextures;

            std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings;

            /**
             * @brief the decoder used to decode the video file into frames
             */
            videoReader decode;
          protected:

            bool                         on_render(const Glib::RefPtr<Gdk::GLContext>& context) override;
            Glib::RefPtr<Gdk::GLContext> on_create_context() override;

          public:
            explicit simulatorRenderWidget(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings);
            videoReader& getDecoder();

            void                         on_realize() override;
            void                         on_unrealize() override;

            // bool on_tick(const Glib::RefPtr<Gdk::FrameClock>& frame_clock);

            /**
             * @brief adds a texture to be rendered on top of the video
             */
            void addTexture(std::shared_ptr<SimpleGFX::SimpleGL::texture> newTexture);

            /**
             * @brief remove a texture from being rendered
             */
            void removeTexture(const std::string& textureName);

            /**
             * @brief jumps to a given frame in the video and display it
             *
             * @param frame_num the number of the frame that should be displayed next
             */
            void gotoFrame(uint64_t frame_num);

            // the rendertimes of the video
            std::optional<std::vector<sakurajin::unit_system::time_si>> getNewRendertimes();
        };
    } // namespace Video
} // namespace libtrainsim
