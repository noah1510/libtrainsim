#pragma once

#include "renderWidget/renderWidgetBase.hpp"

namespace libtrainsim {
    namespace Video {
        class LIBTRAINSIM_EXPORT_MACRO renderWidgetPicture : public renderWidgetBase {
          private:
            Gtk::Picture mainPicture;

            void updateImage();

          public:
            explicit renderWidgetPicture(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                    std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher);

            void gotoFrame(uint64_t frame_num) override;

        };
    } // namespace Video
} // namespace libtrainsim
