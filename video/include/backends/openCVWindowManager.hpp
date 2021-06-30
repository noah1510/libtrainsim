#pragma once

#include "genericWindowManager.hpp"
#include "openCVRenderer.hpp"

namespace libtrainsim {
    namespace Video{
        #if defined(HAS_OPENCV_SUPPORT)
            class openCVWindowManager : public genericWindowManager{
            private:
            public:
                openCVWindowManager(openCVRenderer& _renderer);
                ~openCVWindowManager();
                void createWindow(const std::string& windowName);
                void refreshWindow();
            };
        #endif
    }
}
