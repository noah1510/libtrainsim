#include "backends/ffmpeg_sdl.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::backend;

videoOpenFF_SDL::~videoOpenFF_SDL(){
    lastFrame = Frame();
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool videoOpenFF_SDL::load(const std::filesystem::path& uri){
    loadedFile = uri;
    videoFullyLoaded = false;

    auto ret = avformat_open_input(&pFormatCtx, loadedFile.string().c_str(), NULL, NULL);
    if (ret < 0){
        std::cerr << "Could not open file: " << loadedFile << std::endl;
        return false;
    }
    
    ret = avformat_find_stream_info(pFormatCtx, NULL);
    if (ret < 0){
        std::cerr << "Could not find stream information for file: " << loadedFile << std::endl;
        return false;
    }

    av_dump_format(pFormatCtx, 0, loadedFile.string().c_str(), 0);
    
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1){
        std::cerr << "Could not find video stream." << std::endl;
        return false;
    }

    pCodec = NULL;
    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
    if (pCodec == NULL){
        std::cerr << "Unsupported codec!" << std::endl;
        return false;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar);
    if (ret != 0){
        std::cerr << "Could not copy codec context." << std::endl;
        return false;
    }

    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0)
    {
        std::cerr << "Could not open codec." << std::endl;
        return false;
    }
    
    videoFullyLoaded = true;
    lastFrame = Frame();
    lastFrame.setBackend(ffmpeg_sdl);

    return true; 
}

const libtrainsim::Frame videoOpenFF_SDL::getNextFrame(){
    //TODO implement function to retrieve next frame
    
    return Frame();
}


void videoOpenFF_SDL::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == "" || !videoFullyLoaded || windowFullyCreated){
        return;
    }

    currentWindowName = windowName;
    screen = SDL_CreateWindow(
                            "SDL Video Player",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            pCodecCtx->width/2,
                            pCodecCtx->height/2,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!screen){
        std::cerr << "SDL: could not set video mode - exiting." << std::endl;
        return;
    }
    
    SDL_GL_SetSwapInterval(1);
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    
    texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_YV12,
                SDL_TEXTUREACCESS_STREAMING,
                pCodecCtx->width,
                pCodecCtx->height
            );
    
    pPacket = av_packet_alloc();
    if (pPacket == NULL){
        std::cout << "Could not alloc packet." << std::endl;
        return;
    }

    sws_ctx = sws_getContext(
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        AV_PIX_FMT_YUV420P,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );
    
    numBytes = av_image_get_buffer_size(
                AV_PIX_FMT_YUV420P,
                pCodecCtx->width,
                pCodecCtx->height,
                32
            );
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    
    windowFullyCreated = true;
}

void videoOpenFF_SDL::refreshWindow(){
    if(windowFullyCreated){
        //TODO display the frame
    }
}

void videoOpenFF_SDL::displayFrame(const Frame& newFrame){
    if (!(newFrame.getBackend() == libtrainsim::ffmpeg_sdl || newFrame.getBackend() == libtrainsim::ffmpeg) ||
        newFrame.dataFF() == nullptr
    ){
        return;
    } 

    lastFrame = newFrame;
    refreshWindow();
}

void videoOpenFF_SDL::updateWindow(){
    displayFrame(getNextFrame());
}
