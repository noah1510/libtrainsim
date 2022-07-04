#include "backends/ffmpegRenderer.hpp"
#include "frame.hpp"

using namespace libtrainsim;
using namespace libtrainsim::Video;

#if defined(HAS_FFMPEG_SUPPORT)

ffmpegRenderer::~ffmpegRenderer(){
    av_packet_unref(pPacket);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

}

bool ffmpegRenderer::load(const std::filesystem::path& uri){
    loadedFile = uri;

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
    pCodec = const_cast<AVCodec*>( avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id) );
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
    if (ret < 0){
        std::cerr << "Could not open codec." << std::endl;
        return false;
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
    
    pPacket = av_packet_alloc();
    if (pPacket == NULL){
        std::cout << "Could not alloc packet." << std::endl;
        return false;
    }
    
    numBytes = av_image_get_buffer_size(
        AV_PIX_FMT_YUV420P,
        pCodecCtx->width,
        pCodecCtx->height,
        32
    );
    
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    
    endOfFile = false;

    return true; 
}

std::shared_ptr<libtrainsim::Frame> ffmpegRenderer::getNextFrame(){
    if(endOfFile){return std::make_shared<libtrainsim::Frame>();};
    
    av_packet_unref(pPacket);
    
    auto pFrame = std::make_shared<libtrainsim::Frame>();
    pFrame->setBackend(renderer_ffmpeg);
    
    auto ret = av_read_frame(pFormatCtx, pPacket);
    if(ret < 0 || pPacket->stream_index != videoStream){
        return std::make_shared<libtrainsim::Frame>();
    };
    
    ret = avcodec_send_packet(pCodecCtx, pPacket);
    if (ret < 0){
        std::cerr << "Error sending packet for decoding." << std::endl;
        return std::make_shared<libtrainsim::Frame>();
    }
    
    ret = avcodec_receive_frame(pCodecCtx, pFrame->dataFF());
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0){
        if(ret == AVERROR_EOF){endOfFile = true;};
        return std::make_shared<libtrainsim::Frame>();
    }
    
    currentFrameNumber++;
    return pFrame;
}


std::shared_ptr<libtrainsim::Frame> ffmpegRenderer::gotoFrame(uint64_t frameNum){
    if(endOfFile){return std::make_shared<libtrainsim::Frame>();};
    //double fps = static_cast<double>(pFormatCtx->streams[videoStream]->r_frame_rate.num) / static_cast<double>(pFormatCtx->streams[videoStream]->r_frame_rate.den);
    //int64_t _time = static_cast<int64_t>( static_cast<double>(frameNum)*fps);
    //av_seek_frame(pFormatCtx, videoStream, frameNum, AVSEEK_FLAG_ANY);
    while (frameNum > currentFrameNumber + 1){
        getNextFrame();
    }
    
    if(currentFrameNumber+1 == frameNum){
        return getNextFrame();
    };
    
    return std::make_shared<libtrainsim::Frame>();
}

uint64_t ffmpegRenderer::getFrameCount(){
    if(endOfFile){return 0;};
    return pCodecCtx->frame_number;
}
double ffmpegRenderer::getHight(){
    if(pCodecCtx == nullptr || endOfFile){return 0.0;};
    return pCodecCtx->height;
}

double ffmpegRenderer::getWidth(){
    if(pCodecCtx == nullptr || endOfFile){return 0.0;};
    return pCodecCtx->width;
}

std::shared_ptr<libtrainsim::Frame> ffmpegRenderer::scaleFrame(std::shared_ptr<libtrainsim::Frame> frame){
    if(pCodecCtx == nullptr || endOfFile){return std::make_shared<libtrainsim::Frame>();};
    auto retval = std::make_shared<libtrainsim::Frame>(); 
    initFrame(retval);
    
    sws_scale(
        sws_ctx,
        (uint8_t const * const *)frame->dataFF()->data,
        frame->dataFF()->linesize,
        0,
        pCodecCtx->height,
        retval->dataFF()->data,
        retval->dataFF()->linesize
    );
    
    return retval;
}

void ffmpegRenderer::initFrame(std::shared_ptr<libtrainsim::Frame> frame){
    if(pCodecCtx == nullptr || endOfFile || frame == nullptr){return;};
    
    frame->setBackend(renderer_ffmpeg);
    av_image_fill_arrays(
        frame->dataFF()->data,
        frame->dataFF()->linesize,
        buffer,
        AV_PIX_FMT_YUV420P,
        pCodecCtx->width,
        pCodecCtx->height,
        32
    );
}

#endif

