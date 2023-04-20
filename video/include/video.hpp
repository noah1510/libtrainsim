#pragma once

#include "video_reader.hpp"

namespace libtrainsim {
    namespace Video{

    class simulatorWindowGLArea : public Gtk::GLArea{
        private:
            unsigned int tickID = 0;
            bool realized = false;
            unsigned int VBO = 0, VAO = 0, EBO = 0;
            unsigned int texUnits = 0;

            std::shared_mutex dataMutex;

            void loadBuffers();

            //generate the source of the display shader and compile it
            void generateDisplayShader();

            /**
            * @brief the shader used to render the video into a texture
            */
            std::shared_ptr<SimpleGFX::SimpleGL::shader> displayShader = nullptr;

            //all the textures that are displayed on the output texture
            std::vector< std::shared_ptr<SimpleGFX::SimpleGL::texture> > displayTextures;

            std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings;

            /**
            * @brief the decoder used to decode the video file into frames
            */
            videoReader decode;

        public:
            simulatorWindowGLArea(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings);
            videoReader& getDecoder();

            void on_realize() override;
            void on_unrealize() override;
            bool on_render(const Glib::RefPtr<Gdk::GLContext>& context) override;

            bool on_tick(const Glib::RefPtr<Gdk::FrameClock>& frame_clock);
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

            //the the rendertimes of the video
            std::optional< std::vector<sakurajin::unit_system::time_si> > getNewRendertimes();
        };

        /**
        * @brief This class is resposiblie for managing all the video material.
        * 
        */
        class LIBTRAINSIM_EXPORT_MACRO videoManager : public Gtk::Window, public SimpleGFX::eventHandle{
            private:
                
                //a mutex to secure this class for multithreading
                std::shared_mutex videoMutex;

                simulatorWindowGLArea* mainGLArea = nullptr;
                Gtk::AspectFrame* areaFrame = nullptr;

                std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings;

            public:
                /**
                * @brief Construct a new videoManager
                *
                * This is the main render window for the simulator.
                * To construct it a window name
                * 
                */
                videoManager(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings);

                /**
                * @brief Destroy the video object, on destruction everything will be reset.
                * 
                */
                ~videoManager();

                videoReader& getDecoder();

                /**
                * @brief jumps to a given frame in the video and display it
                * 
                * @param frame_num the number of the frame that should be displayed next
                */
                void gotoFrame(uint64_t frame_num);
                
                /**
                 * @brief adds a texture to be rendered on top of the video
                 */
                void addTexture(std::shared_ptr<SimpleGFX::SimpleGL::texture> newTexture);
                
                /**
                 * @brief remove a texture from being rendered
                 */
                void removeTexture(const std::string& textureName);
                
                //the the rendertimes of the video
                std::optional< std::vector<sakurajin::unit_system::time_si> > getNewRendertimes();

                bool onEvent(const SimpleGFX::inputEvent& event) override;

                bool on_close_request() override;
                
        };
    }
}

