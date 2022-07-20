#include "video.hpp"

using namespace std::literals;

libtrainsim::Video::videoManager::videoManager(){
    try{
        initSDL2();
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not create instance"));
    }
}

libtrainsim::Video::videoManager::~videoManager(){
    reset();
}

void libtrainsim::Video::videoManager::initSDL2(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
}

void libtrainsim::Video::videoManager::reset(){
    std::scoped_lock<std::shared_mutex> lock{videoMutex};
    
    try{
        nextFrame.wait();
    }catch(...){}
    
    SDL_DestroyRenderer(sdl_renderer);
    SDL_Quit();
    std::cout << "libtrainsim::video was reset" << std::endl;
}

bool libtrainsim::Video::videoManager::load(const std::filesystem::path& uri){
    return decode.load(uri);
}

const std::filesystem::path& libtrainsim::Video::videoManager::getFilePath() const{
    return decode.getLoadedFile();
}

void libtrainsim::Video::videoManager::createWindow ( const std::string& windowName ) {
    if(currentWindowName != "" || windowFullyCreated){
        throw std::runtime_error("window alread exists");
    }
    
    if(decode.reachedEndOfFile()){
        throw std::runtime_error("renderer is already done. try creating the window with a new renderer.");
    }
    
    if(windowName == ""){
        throw std::invalid_argument("The window name may not be an empty string");
    }

    currentWindowName = windowName;
    screen = SDL_CreateWindow(
        currentWindowName.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        decode.getWidth(),
        decode.getHeight(),
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );

    if (!screen){
        throw std::runtime_error("SDL: could not set video mode - exiting.");
    }
    
    SDL_GL_SetSwapInterval(1);
    auto renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    #ifdef ENDABLE_VSYNC
    if(true){renderFlags |= SDL_RENDERER_PRESENTVSYNC;};
    #endif
    sdl_renderer = SDL_CreateRenderer(screen, -1, renderFlags);
    
    texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        decode.getWidth(),
        decode.getHeight()
    );
    
    lastFrame.reset();
    try{
        lastFrame = decode.scaleFrame(decode.getNextFrame());
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not retrieve next frame"));
    }
    
    windowFullyCreated = true;
}

double libtrainsim::Video::videoManager::getHeight() {
    return decode.getHeight();
}

double libtrainsim::Video::videoManager::getWidth() {
    return decode.getWidth();
}

void libtrainsim::Video::videoManager::gotoFrame ( uint64_t frame_num ) {
    
    std::scoped_lock<std::shared_mutex> lock{videoMutex};

    nextFrameToGet = frame_num > nextFrameToGet ? frame_num : nextFrameToGet;
    
    if(fetchingFrame){
        return;
    }
    
    if(decode.getCurrentFrameNumber() >= nextFrameToGet){
        return;
    }
    
    nextFrame = std::async(std::launch::async, [this](){return decode.gotoFrame(nextFrameToGet);} );
    fetchingFrame = true;
}

bool libtrainsim::Video::videoManager::reachedEndOfFile() {
    return decode.reachedEndOfFile();
}

void libtrainsim::Video::videoManager::refreshWindow() {
    if(!windowFullyCreated){
        return;
    }
    
    videoMutex.lock();
    
    if(fetchingFrame){
        auto status = nextFrame.wait_for(1ms);
        if(status == std::future_status::ready){
            lastFrame = decode.scaleFrame(nextFrame.get());
            fetchingFrame = false;
        }
    }
    
    videoMutex.unlock();
    
    //start fetching the next queued fram asap
    //if no higher frame number is queued then this call does nothing
    gotoFrame(0);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = decode.getWidth();
    rect.h = decode.getHeight();
    
    SDL_UpdateYUVTexture(
        texture,
        &rect,
        lastFrame->data()->data[0],
        lastFrame->data()->linesize[0],
        lastFrame->data()->data[1],
        lastFrame->data()->linesize[1],
        lastFrame->data()->data[2],
        lastFrame->data()->linesize[2]
    );

    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer,texture,NULL,NULL);
    SDL_RenderPresent(sdl_renderer);
}






  
