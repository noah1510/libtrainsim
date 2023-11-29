#pragma once

#include "renderWidget/renderWidgetBase.hpp"

namespace libtrainsim {
    namespace Video {
        template <videoDecoderClass decoderClass>
        class renderWidgetPicture : public renderWidgetBase<decoderClass> {
          private:
            Gtk::Picture mainPicture;

            void updateImage() {
                if (this->decode.hasNewPixbuf()) {
                    mainPicture.set_pixbuf(this->decode.getUsablePixbuf());
                }
            }

          public:
            template <typename... decoderArgs>
            explicit renderWidgetPicture(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                         std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher,
                                         decoderArgs&&... decoder_args)
                : libtrainsim::Video::renderWidgetBase<decoderClass>{std::move(_simSettings), std::move(_mainAppLauncher), &decoder_args...},
                  mainPicture{} {

                this->mainPicture.set_expand(true);
                this->set_child(mainPicture);
            }

            void gotoFrame(uint64_t frame_num) override {
                libtrainsim::Video::renderWidgetBase<decoderClass>::gotoFrame(frame_num);

                this->mainAppLauncher->callDeffered(sigc::mem_fun(*this, &renderWidgetPicture::updateImage));
            }
        };
    } // namespace Video
} // namespace libtrainsim
