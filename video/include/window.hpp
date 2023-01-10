#pragma once

#include <string>
#include "helper.hpp"

namespace libtrainsim{
    namespace Video{
        class LIBTRAINSIM_EXPORT_MACRO window{
        protected:
            std::string windowName;
            bool beginDraw();
            void endDraw();
            bool showWindow = true;
            bool closableWindow = true;
            int flags = 0;
            window(const std::string& _windowName, bool registerWindow = true);
            window() = delete;
            window(window&&) = delete;
            window(const window&) = delete;
            virtual void content();
        public:
            virtual ~window();
            //draw can be overridden but usually that is not needed.
            virtual void draw();
            const std::string& getName() const;
            bool isVisible() const;
            void show();
        };
    }
}
