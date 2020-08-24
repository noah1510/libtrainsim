
namespace libtrainsim {
    
    class control{
        private:
            control(void);

            static control& getInstance(){
                static control instance;
                return instance;
            };

            void hello_impl() const;

        public:
            static void hello(){
                return getInstance().hello_impl();
            }
    };
}

