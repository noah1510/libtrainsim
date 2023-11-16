#pragma once

#include "videoDecode/videoReader.hpp"

namespace libtrainsim {
    namespace Video {
        class LIBTRAINSIM_EXPORT_MACRO renderWidgetBase : public Gtk::AspectFrame {
          protected:
            /**
             * The settings used by the simulator.
             */
            std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings;

            /**
             * @brief the decoder used to decode the video file into frames
             */
            videoReader decode;

            /**
             * The Logging interface used by this class.
             * This will be set in the constructor and usually is the same as simSettings->getLogger().
             */
            std::shared_ptr<SimpleGFX::logger> LOGGER;

            /**
             * Construct a new generic renderWidgetBase
             * This sets up the decoder and the logger.
             * The aspect frame will be set to the aspect ratio of the video.
             * @note this object can not be constructed directly and should only be used as a base class.
             * @param _simSettings the simulator settings
             */
            explicit renderWidgetBase(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings);

          public:
            /**
             * Get a reference to the decoder used by this object.
             * @return a reference to the decoder used by this object.
             */
            [[nodiscard]]
            videoReader& getDecoder();

            /**
             * @brief jumps to a given frame in the video and display it
             *
             * @param frame_num the number of the frame that should be displayed next
             */
            void gotoFrame(uint64_t frame_num);

            /**
             * A shorthand for getDecoder().getNewRendertimes()
             * @return the latest render time intervals
             */
            [[nodiscard]]
            std::optional<std::vector<sakurajin::unit_system::time_si>> getNewRendertimes();
        };
    } // namespace Video
} // namespace libtrainsim
