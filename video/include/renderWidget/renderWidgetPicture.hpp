#pragma once

#include "renderWidget/renderWidgetBase.hpp"

namespace libtrainsim {
    namespace Video {
        class LIBTRAINSIM_EXPORT_MACRO renderWidgetPicture : public renderWidgetBase {
          private:
            Gtk::Picture mainPicture;

          public:
            explicit renderWidgetPicture(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                    std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher);

        };
    } // namespace Video
} // namespace libtrainsim
