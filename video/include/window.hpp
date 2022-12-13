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
        public:
            window(const std::string& _windowName);
            virtual ~window();
            //draw can be overridden but usually that is not needed.
            virtual void draw();
            virtual void drawContent();
            bool isVisible() const;
            void show();
        };
    }
}
