#pragma once

#include "videoDecode/videoDecoderBase.hpp"

namespace libtrainsim {
    namespace Video {
        template <videoDecoderClass decoderClass>
        class renderWidgetBase : public Gtk::AspectFrame {
          protected:
            /**
             * The settings used by the simulator.
             */
            std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings;

            /**
             * @brief the decoder used to decode the video file into frames
             */
            decoderClass decode;

            /**
             * @brief The Logging interface used by this class.
             * This will be set in the constructor and usually is the same as simSettings->getLogger().
             */
            std::shared_ptr<SimpleGFX::logger> LOGGER;

            /**
             * @brief The appLauncher used by this class.
             * It is needed to queue api calls to happen in the main thread.
             * This allows gotoFrame to be called from any thread without crashing the program.
             */
            std::shared_ptr<SimpleGFX::SimpleGL::appLauncher> mainAppLauncher;

            /**
             * Construct a new generic renderWidgetBase
             * This sets up the decoder and the logger.
             * The aspect frame will be set to the aspect ratio of the video.
             * @note this object can not be constructed directly and should only be used as a base class.
             * @param _simSettings the simulator settings
             */
            template <typename... decoderArgs>
            explicit renderWidgetBase(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                      std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher,
                                      decoderArgs&&... decoder_args)
                : Gtk::AspectFrame{},
                  simSettings{std::move(_simSettings)},
                  decode{simSettings->getCurrentTrack().getVideoFilePath(), simSettings->getLogger(), &decoder_args...},
                  LOGGER{simSettings->getLogger()},
                  mainAppLauncher{std::move(_mainAppLauncher)} {

                auto [w, h] = decode.getDimensions();
                set_ratio(w / h);
            }

          public:
            /**
             * Get a reference to the decoder used by this object.
             * @return a reference to the decoder used by this object.
             */
            [[nodiscard]]
            decoderClass& getDecoder(){
                return decode;
            }

            /**
             * @brief jumps to a given frame in the video and display it
             *
             * @param frame_num the number of the frame that should be displayed next
             */
            virtual void gotoFrame(uint64_t frame_num){
                // queue a redraw if the requested frame is newer than the currently displayed one.
                if (decode.requestFrame(frame_num)) {
                    mainAppLauncher->callDeffered(sigc::mem_fun(*this, &renderWidgetBase::queue_draw));
                }
            }

            /**
             * A shorthand for getDecoder().getNewRendertimes()
             * @return the latest render time intervals
             */
            [[nodiscard]]
            std::optional<std::vector<sakurajin::unit_system::time_si>> getNewRendertimes(){
                return decode.getNewRendertimes();
            }
        };

        template<class widgetClass, class decoderClass>
        concept renderWidgetClass = std::is_base_of<::libtrainsim::Video::renderWidgetBase<decoderClass>, widgetClass>::value;
    } // namespace Video
} // namespace libtrainsim
