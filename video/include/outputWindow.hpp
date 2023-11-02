#pragma once

#include "video_reader.hpp"
#include "simulatorRenderWidget.hpp"

namespace libtrainsim {
    namespace Video {

        /**
         * @brief This class is resposible for managing all the video material.
         *
         */
        class LIBTRAINSIM_EXPORT_MACRO [[maybe_unused]] videoManager : public Gtk::Window, public SimpleGFX::eventHandle {
          private:
            // a mutex to secure this class for multithreading
            std::shared_mutex videoMutex;

            simulatorRenderWidget* mainGLArea = nullptr;
            Gtk::AspectFrame*      areaFrame  = nullptr;

            std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings;

          public:
            /**
             * @brief Construct a new videoManager
             *
             * This is the main render window for the simulator.
             * To construct it a window name
             *
             */
            [[maybe_unused]] explicit videoManager(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings);

            /**
             * @brief Destroy the video object, on destruction everything will be reset.
             *
             */
            ~videoManager() override;

            [[maybe_unused]] [[nodiscard]]
            videoReader& getDecoder();

            /**
             * @brief jumps to a given frame in the video and display it
             *
             * @param frame_num the number of the frame that should be displayed next
             */
            [[maybe_unused]]
            void gotoFrame(uint64_t frame_num);

            /**
             * @brief adds a texture to be rendered on top of the video
             */
            [[maybe_unused]]
            void addTexture(std::shared_ptr<SimpleGFX::SimpleGL::texture> newTexture);

            /**
             * @brief remove a texture from being rendered
             */
            [[maybe_unused]]
            void removeTexture(const std::string& textureName);

            // the rendertimes of the video
            [[maybe_unused]] [[nodiscard]]
            std::optional<std::vector<sakurajin::unit_system::time_si>> getNewRendertimes();

            bool onEvent(const SimpleGFX::inputEvent& event) override;

            bool on_close_request() override;
        };
    } // namespace Video
} // namespace libtrainsim
