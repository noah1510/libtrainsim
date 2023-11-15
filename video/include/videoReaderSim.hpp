#pragma once


#include "videoReader.hpp"
namespace libtrainsim {
    namespace Video {
        class videoReaderSim;

        /**
         * @brief the settings page for the video decoder settings
         *
         */
        /*class LIBTRAINSIM_EXPORT_MACRO videoDecodeSettings : public SimpleGFX::SimpleGL::tabPage{
          private:
            void content() override;
            videoReader& decoder;
            const std::array< std::pair<std::string, int>, 11> AlgorithmOptions;
            const std::array< std::tuple<std::string, int>, 7> AlgorithmDetailsOptions;
          public:
            videoDecodeSettings(videoReader& VR);
        };*/

        /**
         * @brief a class the handle asynchronous video decode from a single file
         *
         * This class handles all of the video decode needs for libtrainsim.
         * Once a new object is constructed it starts a worker thread which does the actual decoding.
         * Outside of this class a new frame can be requested from the video decoder. However that new
         * requested frame must be further in the video than the currently displayed one.
         *
         * @note if you want to check if the video decoder has quit use the reachedEndOfFile function.
         *
         */
        class LIBTRAINSIM_EXPORT_MACRO videoReaderSim : public videoReader{
            // friend class videoDecodeSettings;
          private:

            /**
             * @brief A pointer to the simulator settings
             */
            std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings = nullptr;

          public:
            /**
             * @brief create a new video decoder for a given video file
             *
             * @param filename the path to the file that should be played back by this object
             * @param threadCount the number of threads that should be used for video decode, 0 for autodetect
             */
            explicit videoReaderSim(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings, uint64_t threadCount = 0, uint64_t _seekCutoff = 200);
        };
    } // namespace Video
} // namespace libtrainsim
