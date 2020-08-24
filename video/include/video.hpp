
namespace libtrainsim {
    
    class video{
        private:
            video(void);

            static video& getInstance(){
                static video instance;
                return instance;
            };

            void hello_impl();

        public:
            static void hello(){
                return getInstance().hello_impl();
            }
    };
}

