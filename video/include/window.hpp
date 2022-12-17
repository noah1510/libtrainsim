#pragma once

#include <string>

namespace libtrainsim{
    namespace Video{
        class window{
        protected:
            std::string windowName;
            bool beginDraw();
            void endDraw();
            bool showWindow = true;
            bool closableWindow = true;
            int flags = 0;
            window(const std::string& _windowName);
            window() = delete;
            virtual void drawContent();
        public:
            virtual ~window();
            //draw can be overridden but usually that is not needed.
            virtual void draw();
            bool isVisible() const;
            void show();
        };
    }
}
