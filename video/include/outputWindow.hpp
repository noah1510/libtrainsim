#pragma once

#include "renderWidget/renderWidgetBase.hpp"
#include "videoDecode/videoDecoderLibav.hpp"

namespace libtrainsim {
    namespace Video {

        /**
         * @brief This class is resposible for managing all the video material.
         *
         */
        template <videoDecoderClass decoderClass, renderWidgetClass<decoderClass> widgetClass>
        class LIBTRAINSIM_EXPORT_MACRO [[maybe_unused]] outputWindow : public Gtk::Window, public SimpleGFX::eventHandle {
          private:
            widgetClass* mainRenderer = nullptr;

            std::shared_ptr<libtrainsim::core::simulatorConfiguration> simSettings;

            std::shared_ptr<SimpleGFX::logger> LOGGER;

            std::shared_ptr<SimpleGFX::SimpleGL::appLauncher> mainAppLauncher;

          protected:
            bool on_close_request() override {
                *LOGGER << SimpleGFX::loggingLevel::normal << "closing video manager";

                if (has_group()) {
                    auto group = get_group();
                    group->remove_window(*this);
                    set_hide_on_close(false);
                    set_visible(false);
                    auto windowList = group->list_windows();
                    for (auto win = windowList.begin(); win < windowList.end(); win++) {
                        if ((*win)->get_hide_on_close()) {
                            (*win)->set_hide_on_close(false);
                        }
                        (*win)->close();
                        win = windowList.erase(win);
                        group->remove_window(*(*win));
                    }
                }

                return Gtk::Window::on_close_request();
            }

          public:
            /**
             * @brief Construct a new videoManager
             *
             * This is the main render window for the simulator.
             * To construct it a window name
             *
             */
            template <typename... args>
                requires std::constructible_from<widgetClass,
                                                 std::shared_ptr<libtrainsim::core::simulatorConfiguration>,
                                                 std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>,
                                                 args...>
            [[maybe_unused]] explicit outputWindow(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                                   std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher,
                                                   args&&... _args)
                : Gtk::Window{},
                  SimpleGFX::eventHandle(),
                  simSettings{std::move(_simSettings)},
                  LOGGER{simSettings->getLogger()},
                  mainAppLauncher{std::move(_mainAppLauncher)} {

                set_title(simSettings->getCurrentTrack().getName());
                set_default_size(1280, 720);
                set_cursor("none");

                mainRenderer = Gtk::make_managed<widgetClass>(simSettings, mainAppLauncher, &_args...);
                set_child(*mainRenderer);
            }

            [[maybe_unused]] [[nodiscard]]
            widgetClass& getRenderer() noexcept {
                return *mainRenderer;
            };

            /**
             * @brief shorthand for getRenderer()->gotoFrame(frame_num)
             *
             * This advances the video to the specified frame number.
             */
            [[maybe_unused]]
            void gotoFrame(uint64_t frame_num) {
                mainRenderer->gotoFrame(frame_num);
            }

            /**
             * @brief shorthand for getRenderer()->getNewRendertimes()
             * @See renderWidgetBase::getNewRendertimes() for more details
             * @return the latest render times
             */
            [[maybe_unused]] [[nodiscard]]
            std::optional<std::vector<sakurajin::unit_system::time_si>> getNewRendertimes() {
                return mainRenderer->getNewRendertimes();
            }

            bool onEvent(const SimpleGFX::inputEvent& event) override {
                if (event.inputType != SimpleGFX::inputAction::press) {
                    return false;
                }

                const auto actionCases = {"CLOSE", "MAXIMIZE"};
                switch (SimpleGFX::TSwitch(event.name, actionCases)) {
                    case (0):
                        mainAppLauncher->callDeffered(sigc::mem_fun(*this, &outputWindow::close));
                        return false;
                    case (1):
                        mainAppLauncher->callDeffered([this]() {
                            if (is_fullscreen()) {
                                unfullscreen();
                            } else {
                                fullscreen();
                            }
                        });
                        return true;
                    default:
                        return false;
                }
            }
        };
    } // namespace Video
} // namespace libtrainsim
