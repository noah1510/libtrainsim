#include "videoDecode/videoDecoderLibav.hpp"

using namespace sakurajin::unit_system;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

// create a full error message from an av error id
static inline std::string makeAVError(int errnum) {
    std::string errMsg;
    errMsg.resize(AV_ERROR_MAX_STRING_SIZE);
    av_make_error_string(errMsg.data(), AV_ERROR_MAX_STRING_SIZE, errnum);

    return errMsg;
}

// correct pixel format to longer give swscale warnings
static inline AVPixelFormat correctForDeprecatedPixelFormat(AVPixelFormat pix_fmt) {
    // (YUVJ has been deprecated, change pixel format to regular YUV)
    switch (pix_fmt) {
        case AV_PIX_FMT_YUVJ420P:
            return AV_PIX_FMT_YUV420P;
        case AV_PIX_FMT_YUVJ422P:
            return AV_PIX_FMT_YUV422P;
        case AV_PIX_FMT_YUVJ444P:
            return AV_PIX_FMT_YUV444P;
        case AV_PIX_FMT_YUVJ440P:
            return AV_PIX_FMT_YUV440P;
        default:
            return pix_fmt;
    }
}

/*
libtrainsim::Video::videoDecodeSettings::videoDecodeSettings ( libtrainsim::Video::videoDecoderLibav& VR ) : tabPage{"decodeSettings"},
decoder{VR}, AlgorithmOptions{{
    {"fast bilinear", SWS_FAST_BILINEAR},
    {"bilinear", SWS_BILINEAR},
    {"bicubic", SWS_BICUBIC},
    {"experimental", SWS_X},
    {"point (nearest neighbor)", SWS_POINT},
    {"area", SWS_AREA},
    {"bicubic luma, bilinear chroma", SWS_BICUBLIN},
    {"gauss", SWS_GAUSS},
    {"sinc", SWS_SINC},
    {"lanczos", SWS_LANCZOS},
    {"spline", SWS_SPLINE}
}},AlgorithmDetailsOptions{{
    {"Print SWS Info", SWS_PRINT_INFO},
    {"accurate rounding", SWS_ACCURATE_RND},
    {"Bite exact output", SWS_BITEXACT},
    {"error diffusion", SWS_ERROR_DIFFUSION},
    {"Enable direct BGR", SWS_DIRECT_BGR},
    {"Enable full chorma interpolation", SWS_FULL_CHR_H_INT},
    {"Enable full chroma input", SWS_FULL_CHR_H_INP}
}}{};

void libtrainsim::Video::videoDecodeSettings::content() {
    //get the current flags
    decoder.contextMutex.lock_shared();
    auto currentFlags = decoder.scalingContextParams;
    decoder.contextMutex.unlock_shared();

    decoder.frameNumberMutex.lock_shared();
    auto currentCutoff = decoder.seekCutoff;
    decoder.frameNumberMutex.unlock_shared();

    //selection for the scaling algorithm
    static size_t comboAlgorithmIndex = 9;
    if(ImGui::BeginCombo("Select the scaling algorithm", AlgorithmOptions.at(comboAlgorithmIndex).first.c_str() )){
        for(size_t i = 0; i < AlgorithmOptions.size();i++){
            if(ImGui::Selectable(AlgorithmOptions.at(i).first.c_str(), comboAlgorithmIndex == i)){
                comboAlgorithmIndex = i;
            }
        }

        ImGui::EndCombo();
    }

    //selection for all of the algorithm details
    static std::array<bool,7> algorithmDetailSelections {false, false, false, false, false, false, false};
    if(ImGui::BeginCombo("Select algorithm details", "expand here")){
        for(size_t i = 0; i < AlgorithmDetailsOptions.size();i++){
            ImGui::Checkbox(std::get<0>(AlgorithmDetailsOptions.at(i)).c_str(), &algorithmDetailSelections[i]);
        }
        ImGui::EndCombo();
    }

    //a slider for the seek cutoff
    int cutoff = static_cast<int>(currentCutoff);
    ImGui::SliderInt("Change the Cutoff for when to seek frames", &cutoff, 2*decoder.framerate, 20*decoder.framerate);

    //display detailed video stats
    ImGui::Text("Detailed Video Information: ");
    ImGui::Text("    average framerate: %f", decoder.framerate);
    ImGui::Text("    frame number: %d", decoder.av_codec_ctx->frame_number);

    //apply the selected flags
    int newFlags = 0;

    newFlags |= AlgorithmOptions[comboAlgorithmIndex].second;
    for(size_t i = 0; i < AlgorithmDetailsOptions.size();i++){
        if(algorithmDetailSelections[i]){
            newFlags |= std::get<1>(AlgorithmDetailsOptions[i]);
        }
    }

    //update the scaling flags if they are different
    if(newFlags != currentFlags){
        std::scoped_lock<std::shared_mutex> lock{decoder.contextMutex};
        decoder.scalingContextParams = newFlags;
    }

    //update the cutoff if it is changed
    if(cutoff != static_cast<int>(currentCutoff)){
        std::scoped_lock<std::shared_mutex> lock{decoder.frameNumberMutex};
        decoder.seekCutoff = cutoff;
    }
}
*/

libtrainsim::Video::videoDecoderLibav::videoDecoderLibav(std::filesystem::path              _videoFile,
                                                       std::shared_ptr<SimpleGFX::logger> _logger,
                                                       uint64_t                           _seekCutoff,
                                                       uint64_t                           threadCount)
    : videoDecoderBase{std::move(_videoFile), std::move(_logger), _seekCutoff} {
    /*
    //find all of the hardware devices
    std::vector<AVHWDeviceType> deviceTypes;
    AVHWDeviceType lastType = AV_HWDEVICE_TYPE_NONE;
    while ((lastType = av_hwdevice_iterate_types(lastType)) != AV_HWDEVICE_TYPE_NONE){
        deviceTypes.emplace_back(lastType);
    }

    for(size_t i = 0; i < deviceTypes.size(); i++){
        std::cout << "Supported HWDevice: " << av_hwdevice_get_type_name(deviceTypes[i]) << std::endl;
    }
    */

    // Open the file using libavformat
    av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        throw std::runtime_error("Couldn't created AVFormatContext");
    }

    if (avformat_open_input(&av_format_ctx, uri.string().c_str(), nullptr, nullptr) != 0) {
        throw std::invalid_argument("Couldn't open video file");
    }

    *LOGGER << SimpleGFX::loggingLevel::normal << "opened video file: " << uri;

    // Find the first valid video stream inside the file
    video_stream_index = -1;
    AVCodecParameters* av_codec_params;
    AVCodec*           av_codec;

    for (unsigned int i = 0; i < av_format_ctx->nb_streams; ++i) {
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec        = const_cast<AVCodec*>(avcodec_find_decoder(av_codec_params->codec_id));
        if (!av_codec) {
            continue;
        }
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = static_cast<int>(i);
            renderSize         = dimensions{av_codec_params->width, av_codec_params->height};
            auto framerate_tmp = av_format_ctx->streams[i]->avg_frame_rate;
            framerate          = static_cast<double>(framerate_tmp.num) / static_cast<double>(framerate_tmp.den);
            *LOGGER << SimpleGFX::loggingLevel::normal << "video average framerate:" << framerate << " fps";
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
    if (threadCount == 0) {
        // get the number of total threads from ffmpeg
        // if there are less that 4 threads available only use 1
        // otherwise use as many as possible (minus 2) and  up to 16 since more
        // than that seems might cause problems (according to the mpv devs)
        threadCount = av_cpu_count();
        if (threadCount < 4) {
            threadCount = 1;
        } else {
            threadCount -= 2;
        }
    }

    threadCount                = std::clamp<uint64_t>(threadCount, 1, 16);
    av_codec_ctx->thread_count = static_cast<int>(threadCount);
    av_codec_ctx->thread_type  = FF_THREAD_SLICE;
    *LOGGER << SimpleGFX::loggingLevel::normal << "video decode on " << threadCount << " threads.";

    if (avcodec_open2(av_codec_ctx, av_codec, nullptr) < 0) {
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

    reachedEOF = false;
    startRendering();

    // auto settingsTab = std::make_shared<videoDecodeSettings>(*this);
    // imguiHandler::addSettingsTab(settingsTab);
}

libtrainsim::Video::videoDecoderLibav::~videoDecoderLibav() {
    // imguiHandler::removeSettingsTab("decodeSettings");

    if (!reachedEndOfFile()) {
        reachedEOF = true;
    }

    if (renderThread.valid()) {
        *LOGGER << SimpleGFX::loggingLevel::debug << "waiting for render to finish";
        renderThread.wait();
        renderThread.get();
    }

    sws_freeContext(sws_scaler_ctx);
    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avcodec_free_context(&av_codec_ctx);
}


void libtrainsim::Video::videoDecoderLibav::readNextFrame() {
    // Decode one frame
    int response;
    while (av_read_frame(av_format_ctx, av_packet) >= 0) {
        if (av_packet->stream_index != video_stream_index) {
            av_packet_unref(av_packet);
            continue;
        }

        response = avcodec_send_packet(av_codec_ctx, av_packet);
        if (response < 0) {
            *LOGGER << SimpleGFX::loggingLevel::error << "Failed to decode packet: " << makeAVError(response);
            av_packet_unref(av_packet);
            continue;
        }

        response = avcodec_receive_frame(av_codec_ctx, av_frame);
        if (response == AVERROR(EAGAIN)) {
            av_packet_unref(av_packet);
            continue;
        } else if (response == AVERROR_EOF) {
            av_packet_unref(av_packet);
            reachedEOF = true;
            throw std::runtime_error("reached EOF");
        } else if (response < 0) {
            throw std::runtime_error("Failed to decode packet" + makeAVError(response));
        }

        av_packet_unref(av_packet);
        break;
    }
}

void libtrainsim::Video::videoDecoderLibav::seekFrame(uint64_t framenumber) {
    auto* stream = av_format_ctx->streams[video_stream_index];
    auto  ts     = (int64_t(framenumber) * stream->r_frame_rate.den * stream->time_base.den) /
              (int64_t(stream->r_frame_rate.num) * stream->time_base.num);
    // if(av_seek_frame(av_format_ctx, video_stream_index, framenumber, AVSEEK_FLAG_FRAME) < 0){
    if (av_seek_frame(av_format_ctx, video_stream_index, ts, AVSEEK_FLAG_ANY) < 0) {
        throw std::runtime_error("Problem seeking a future frame");
    }

    try {
        readNextFrame();
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Could not retreive the seeked frame"));
    }
}

void libtrainsim::Video::videoDecoderLibav::copyToBuffer(std::shared_ptr<Gdk::Pixbuf>& pixbuf) {

    std::vector<uint8_t> rawBuffer;
    auto [w, h] = renderSize.getCasted<int>();
    rawBuffer.resize(w * h * 4);

    std::shared_lock<std::shared_mutex> lock{contextMutex};

    auto source_pix_fmt = correctForDeprecatedPixelFormat(av_codec_ctx->pix_fmt);
    sws_scaler_ctx      = sws_getCachedContext(sws_scaler_ctx,
                                          w,
                                          h,
                                          source_pix_fmt,
                                          av_frame->width,
                                          av_frame->height,
                                          AV_PIX_FMT_RGB0,
                                          scalingContextParams,
                                          nullptr,
                                          nullptr,
                                          nullptr);

    uint8_t* dest[4]          = {rawBuffer.data(), nullptr, nullptr, nullptr};
    int      dest_linesize[4] = {static_cast<int>(renderSize.x()) * 4, 0, 0, 0};
    auto     hnew             = sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    if (hnew != av_frame->height) {
        throw std::runtime_error("Got a wrong size after scaling.");
    }

    pixbuf = Gdk::Pixbuf::create_from_data(rawBuffer.data(), Gdk::Colorspace::RGB, true, 8, w, h, w * 4);
}
