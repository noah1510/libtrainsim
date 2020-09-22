#include <string>

namespace libtrainsim {
    
    class control{
        private:
            control(void);

            static control& getInstance(){
                static control instance;
                return instance;
            };

            std::string hello_impl() const;

        public:
            static std::string hello(){
                return getInstance().hello_impl();
            }
    };
}

