#include "outputWindow.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
namespace fs = std::filesystem;

libtrainsim::Video::videoManager::videoManager(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings)
    : Gtk::Window{},
      SimpleGFX::eventHandle(),
      simSettings{std::move(_simSettings)}{

    set_title(simSettings->getCurrentTrack().getName());
    set_default_size(1280, 720);
    set_cursor("none");

    mainGLArea = Gtk::make_managed<simulatorRenderWidget>(simSettings);
    set_child(*mainGLArea);
}

bool libtrainsim::Video::videoManager::onEvent(const SimpleGFX::inputEvent& event) {
    if (event.inputType != SimpleGFX::inputAction::press) {
        return false;
    }

    switch (SimpleGFX::SimpleGL::GLHelper::stringSwitch(event.name, {"CLOSE", "MAXIMIZE"})) {
        case (0):
            close();
            return false;
        case (1):
            if (is_fullscreen()) {
                unfullscreen();
            } else {
                fullscreen();
            }
            return true;
        default:
            return false;
    }
}

bool libtrainsim::Video::videoManager::on_close_request() {
    *simSettings->getLogger() << SimpleGFX::loggingLevel::normal << "closing video manager";

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

libtrainsim::Video::videoManager::~videoManager() {
    *simSettings->getLogger() << SimpleGFX::loggingLevel::debug << "locking video manager to prevent draw calls while destroying";
    std::scoped_lock<std::shared_mutex> lock{videoMutex};
}

void libtrainsim::Video::videoManager::gotoFrame(uint64_t frame_num) {
    mainGLArea->gotoFrame(frame_num);
}

void libtrainsim::Video::videoManager::addTexture(std::shared_ptr<texture> newTexture) {
    try {
        mainGLArea->addTexture(std::move(newTexture));
    } catch (...) {
        std::throw_with_nested(std::runtime_error("error adding texture"));
    }
}

void libtrainsim::Video::videoManager::removeTexture(const std::string& textureName) {
    try {
        mainGLArea->removeTexture(textureName);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("error removing texture"));
    }
}

std::optional<std::vector<sakurajin::unit_system::time_si>> libtrainsim::Video::videoManager::getNewRendertimes() {
    return mainGLArea->getNewRendertimes();
}

libtrainsim::Video::videoReader& libtrainsim::Video::videoManager::getDecoder() {
    return mainGLArea->getDecoder();
}
