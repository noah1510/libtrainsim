#include "renderWidget/renderWidgetPicture.hpp"

using namespace std::literals;
using namespace SimpleGFX::SimpleGL;
using namespace SimpleGFX;
namespace fs = std::filesystem;

libtrainsim::Video::renderWidgetPicture::renderWidgetPicture(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings,
                                                   std::shared_ptr<SimpleGFX::SimpleGL::appLauncher>          _mainAppLauncher)
    : libtrainsim::Video::renderWidgetBase{std::move(_simSettings), std::move(_mainAppLauncher)},
      mainPicture{} {

    mainPicture.set_expand(true);
    set_child(mainPicture);
}
