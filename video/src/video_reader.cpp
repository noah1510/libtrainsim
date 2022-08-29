#include "video_reader.hpp"
#include "prefix.hpp"

using namespace sakurajin::unit_system;

std::string libtrainsim::Video::videoReader::makeAVError ( int errnum ) {
    char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}


static AVPixelFormat correct_for_deprecated_pixel_format(AVPixelFormat pix_fmt) {
    // Fix swscaler deprecated pixel format warning
    // (YUVJ has been deprecated, change pixel format to regular YUV)
    switch (pix_fmt) {
        case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
        case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
        case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
        case AV_PIX_FMT_YUVJ440P: return AV_PIX_FMT_YUV440P;
        default:                  return pix_fmt;
    }
}

libtrainsim::Video::videoReader::videoReader(const std::filesystem::path& filename){
    // Open the file using libavformat
    av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        throw std::runtime_error("Couldn't created AVFormatContext");
    }
    
    if(!std::filesystem::exists(filename) || filename.empty()){
        throw std::invalid_argument("video file does not exist or is empty");
    }

    if (avformat_open_input(&av_format_ctx, filename.string().c_str(), NULL, NULL) != 0) {
        throw std::invalid_argument("Couldn't open video file");
    }
    uri = filename;

    // Find the first valid video stream inside the file
    video_stream_index = -1;
    AVCodecParameters* av_codec_params;
    AVCodec* av_codec;
    for (unsigned int i = 0; i < av_format_ctx->nb_streams; ++i) {
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec = const_cast<AVCodec*>( avcodec_find_decoder(av_codec_params->codec_id) );
        if (!av_codec) {
            continue;
        }
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            width = av_codec_params->width;
            height = av_codec_params->height;
            time_base = av_format_ctx->streams[i]->time_base;
            break;
        }
    }
    if (video_stream_index == -1) {
        throw std::invalid_argument("Couldn't find valid video stream inside file");
    }

    // Set up a codec context for the decoder
    av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx) {
        throw std::runtime_error("Couldn't create AVCodecContext");
    }
    if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
        throw std::runtime_error("Couldn't initialize AVCodecContext");
    }
    if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
        throw std::runtime_error("Couldn't open codec");
    }

    av_frame = av_frame_alloc();
    if (!av_frame) {
        throw std::runtime_error("Couldn't allocate AVFrame");
    }
    av_packet = av_packet_alloc();
    if (!av_packet) {
        throw std::runtime_error("Couldn't allocate AVPacket");
    }

    try{
        readNextFrame();
        currentFrameNumber = 0;
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not read initial frame"));
    }
}

libtrainsim::Video::videoReader::~videoReader() {
    
    sws_freeContext(sws_scaler_ctx);
    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avcodec_free_context(&av_codec_ctx);
}


sakurajin::unit_system::base::time_si libtrainsim::Video::videoReader::readNextFrame() {
    auto begin = libtrainsim::core::Helper::now();
    
    // Decode one frame
    int response;
    while (av_read_frame(av_format_ctx, av_packet) >= 0) {
        if (av_packet->stream_index != video_stream_index) {
            av_packet_unref(av_packet);
            continue;
        }

        response = avcodec_send_packet(av_codec_ctx, av_packet);
        if (response < 0) {
            throw std::runtime_error("Failed to decode packet: " + makeAVError(response));
        }

        response = avcodec_receive_frame(av_codec_ctx, av_frame);
        if (response == AVERROR(EAGAIN)) {
            av_packet_unref(av_packet);
            continue;
        } else if(response == AVERROR_EOF){
            av_packet_unref(av_packet);
            reachedEOF = true;
            throw std::runtime_error("reached EOF");
        }else if (response < 0) {
            throw std::runtime_error("Failed to decode packet" + makeAVError(response));
        }

        av_packet_unref(av_packet);
        break;
    }
    
    currentFrameNumber++;
    return unit_cast(libtrainsim::core::Helper::now()-begin);
}

sakurajin::unit_system::base::time_si libtrainsim::Video::videoReader::seekFrame ( uint64_t framenumber ) {
    auto begin = libtrainsim::core::Helper::now();
    
    if(av_seek_frame(av_format_ctx, video_stream_index, framenumber, AVSEEK_FLAG_FRAME) < 0){
        throw std::runtime_error("Problem seeking a future frame");
    }

    try{
        readNextFrame();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not retreive the seeked frame"));
    }

    currentFrameNumber = framenumber;
    return unit_cast(libtrainsim::core::Helper::now()-begin);
}

void libtrainsim::Video::videoReader::copyToBuffer ( uint8_t* frame_buffer ) {
    auto source_pix_fmt = correct_for_deprecated_pixel_format(av_codec_ctx->pix_fmt);
    sws_scaler_ctx = sws_getContext(
        width, 
        height, 
        source_pix_fmt,
        width, 
        height, 
        AV_PIX_FMT_RGB0,
        SWS_SINC, 
        NULL, 
        NULL, 
        NULL
    );
    
    if (!sws_scaler_ctx) {throw std::runtime_error("Couldn't initialize sw scaler");}

    uint8_t* dest[4] = { frame_buffer, NULL, NULL, NULL };
    int dest_linesize[4] = { width * 4, 0, 0, 0 };
    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
}

void libtrainsim::Video::videoReader::copyToBuffer ( std::vector<uint8_t>& frame_buffer ) {
    try{
        if(frame_buffer.size() < static_cast<size_t>( width*height*4)){
            frame_buffer.resize(width*height*4);
        }
        copyToBuffer(frame_buffer.data());
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not copy frame into framebuffer"));
    }
}


const std::filesystem::path& libtrainsim::Video::videoReader::getLoadedFile() const{
    return uri;
}

bool libtrainsim::Video::videoReader::reachedEndOfFile() const {
    return reachedEOF;
}

libtrainsim::Video::dimensions libtrainsim::Video::videoReader::getDimensions() const {
    return {width,height};
}

uint64_t libtrainsim::Video::videoReader::getFrameNumber() const {
    return currentFrameNumber;
}

