#include "renderWidget/renderWidgetBase.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
using namespace SimpleGFX;
namespace fs = std::filesystem;

libtrainsim::Video::renderWidgetBase::renderWidgetBase(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings)
    : Gtk::AspectFrame{},
      simSettings{std::move(_simSettings)},
      decode{_simSettings->getCurrentTrack().getVideoFilePath(), _simSettings->getLogger()},
      LOGGER{simSettings->getLogger()} {

    auto [w, h] = decode.getDimensions();
    set_ratio(w / h);
}


std::optional<std::vector<sakurajin::unit_system::time_si>> libtrainsim::Video::renderWidgetBase::getNewRendertimes() {
    return decode.getNewRendertimes();
}

void libtrainsim::Video::renderWidgetBase::gotoFrame(uint64_t frame_num) {
    // queue a redraw if the requested frame is newer than the currently displayed one.
    if (decode.requestFrame(frame_num)) {
        queue_draw();
    }
}

libtrainsim::Video::videoReader& libtrainsim::Video::renderWidgetBase::getDecoder() {
    return decode;
}
