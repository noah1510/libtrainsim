#pragma once

#include "renderWidget/renderWidgetBase.hpp"

namespace libtrainsim {
    namespace Video {
        template <videoDecoderClass decoderClass>
        class renderWidgetPicture : public renderWidgetBase<decoderClass> {
          private:
            Gtk::Picture mainPicture;

            unsigned int interval_ms = 10;

            bool updateImage() {
                if (this->decode.hasNewPixbuf()) {
                    mainPicture.set_pixbuf(this->decode.getUsablePixbuf());
                }

                return true;
            }

          public:
            template <typename... decoderArgs>
            explicit renderWidgetPicture(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                         std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher,
                                         decoderArgs&&... decoder_args)
                : libtrainsim::Video::renderWidgetBase<decoderClass>{std::move(_simSettings),
                                                                     std::move(_mainAppLauncher),
                                                                     &decoder_args...},
                  mainPicture{} {

                this->mainPicture.set_expand(true);
                this->set_child(mainPicture);

                Glib::signal_timeout().connect(sigc::mem_fun(*this, &renderWidgetPicture::updateImage), interval_ms);
            }

            /*void gotoFrame(uint64_t frame_num) override {
                libtrainsim::Video::renderWidgetBase<decoderClass>::gotoFrame(frame_num);
            }*/
        };
    } // namespace Video
} // namespace libtrainsim
