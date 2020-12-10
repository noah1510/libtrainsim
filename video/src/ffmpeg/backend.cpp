#include "backends/ffmpeg_sdl.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::backend;

videoFF_SDL::~videoFF_SDL(){
    lastFrame.clear();
    pict.clear();
    av_packet_unref(pPacket);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool videoFF_SDL::load(const std::filesystem::path& uri){
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
    
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
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

const libtrainsim::Frame videoFF_SDL::getNextFrame(){
    if(!videoFullyLoaded){return Frame();};
    
    av_packet_unref(pPacket);
    
    Frame pFrame = Frame();
    pFrame.setBackend(ffmpeg_sdl);
    
    auto ret = av_read_frame(pFormatCtx, pPacket);
    if(ret < 0 || pPacket->stream_index != videoStream){
        return Frame();
    };
    
    ret = avcodec_send_packet(pCodecCtx, pPacket);
    if (ret < 0){
        std::cerr << "Error sending packet for decoding." << std::endl;
        return Frame();
    }
    
    ret = avcodec_receive_frame(pCodecCtx, pFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0){
        return Frame();
    }
    
    currentFrameNumber++;
    return pFrame;
}


void videoFF_SDL::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == "" || !videoFullyLoaded || windowFullyCreated){
        return;
    }

    currentWindowName = windowName;
    screen = SDL_CreateWindow(
                            currentWindowName.c_str(),
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            pCodecCtx->width/2,
                            pCodecCtx->height/2,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
    );

    if (!screen){
        std::cerr << "SDL: could not set video mode - exiting." << std::endl;
        return;
    }
    
    SDL_GL_SetSwapInterval(1);
    auto renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    #ifdef ENDABLE_VSYNC
    if(true){renderFlags |= SDL_RENDERER_PRESENTVSYNC;};
    #endif
    renderer = SDL_CreateRenderer(screen, -1, renderFlags);
    
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
    
    pict.setBackend(ffmpeg);
    av_image_fill_arrays(
        pict.dataFF()->data,
        pict.dataFF()->linesize,
        buffer,
        AV_PIX_FMT_YUV420P,
        pCodecCtx->width,
        pCodecCtx->height,
        32
    );
    
    lastFrame.clear();
    lastFrame = getNextFrame();
    
    windowFullyCreated = true;
}

void videoFF_SDL::refreshWindow(){
    if(!windowFullyCreated){
        return;
    }
    
    sws_scale(
        sws_ctx,
        (uint8_t const * const *)lastFrame.dataFF()->data,
        lastFrame.dataFF()->linesize,
        0,
        pCodecCtx->height,
        pict.dataFF()->data,
        pict.dataFF()->linesize
    );

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = pCodecCtx->width;
    rect.h = pCodecCtx->height;
    
    SDL_UpdateYUVTexture(
        texture,
        &rect,
        pict.dataFF()->data[0],
        pict.dataFF()->linesize[0],
        pict.dataFF()->data[1],
        pict.dataFF()->linesize[1],
        pict.dataFF()->data[2],
        pict.dataFF()->linesize[2]
    );

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}

void videoFF_SDL::displayFrame(const Frame& newFrame){
    if (!(newFrame.getBackend() == libtrainsim::ffmpeg_sdl || newFrame.getBackend() == libtrainsim::ffmpeg) ||
        newFrame.dataFF() == nullptr
    ){
        return;
    }

    lastFrame.clear();
    lastFrame = newFrame;
    refreshWindow();
}

void videoFF_SDL::gotoFrame(uint64_t frameNum){
    if(!videoFullyLoaded){return;};
    //double fps = static_cast<double>(pFormatCtx->streams[videoStream]->r_frame_rate.num) / static_cast<double>(pFormatCtx->streams[videoStream]->r_frame_rate.den);
    //int64_t _time = static_cast<int64_t>( static_cast<double>(frameNum)*fps);
    //av_seek_frame(pFormatCtx, videoStream, frameNum, AVSEEK_FLAG_ANY);
    while (frameNum > currentFrameNumber + 1){
        getNextFrame();
    }
    if(currentFrameNumber+1 == frameNum){displayFrame(getNextFrame());};
}

uint64_t videoFF_SDL::getFrameCount(){
    if(!videoFullyLoaded){return 0;};
    return pCodecCtx->frame_number;
}
double videoFF_SDL::getHight(){
    if(pCodecCtx == nullptr || !videoFullyLoaded){return 0.0;};
    return pCodecCtx->height;
}

double videoFF_SDL::getWidth(){
    if(pCodecCtx == nullptr || !videoFullyLoaded){return 0.0;};
    return pCodecCtx->width;
}
