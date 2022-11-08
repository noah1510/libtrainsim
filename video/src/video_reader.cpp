#include "video_reader.hpp"
#include "prefix.hpp"

using namespace sakurajin::unit_system;
using namespace std::literals;

//create a full error message from an av error id
static inline std::string makeAVError ( int errnum ) {
    std::string errMsg;
    errMsg.resize(AV_ERROR_MAX_STRING_SIZE);
    av_make_error_string(errMsg.data(), AV_ERROR_MAX_STRING_SIZE, errnum);
    
    return errMsg;
}

//correct pixel format to longer give swscale warnings
static inline AVPixelFormat correctForDeprecatedPixelFormat(AVPixelFormat pix_fmt) {
    // (YUVJ has been deprecated, change pixel format to regular YUV)
    switch (pix_fmt) {
        case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
        case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
        case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
        case AV_PIX_FMT_YUVJ440P: return AV_PIX_FMT_YUV440P;
        default:                  return pix_fmt;
    }
}

inline void libtrainsim::Video::videoReader::incrementFramebuffer(uint8_t& currentBuffer) const{
    currentBuffer++;
    currentBuffer%=FRAME_BUFFER_COUNT;
}

libtrainsim::Video::videoDecodeSettings::videoDecodeSettings ( libtrainsim::Video::videoReader& VR ) : tabPage{"decodeSettings"}, decoder{VR}, AlgorithmOptions{{
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

void libtrainsim::Video::videoDecodeSettings::displayContent() {
    //get the current flags
    decoder.contextMutex.lock_shared();
    auto currentFlags = decoder.scalingContextParams;
    decoder.contextMutex.unlock_shared();
    
    decoder.frameNumberMutex.lock_shared();
    auto currentCutoff = decoder.seekCutoff;
    decoder.frameNumberMutex.unlock_shared();
    
    //selection for the scaling algorithm
    static size_t comboAlgorithmIndex = 10;
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
            renderSize.x() = av_codec_params->width;
            renderSize.y() = av_codec_params->height;
            auto framerate_tmp = av_format_ctx->streams[i]->avg_frame_rate;
            framerate = static_cast<double>(framerate_tmp.num)/static_cast<double>(framerate_tmp.den);
            std::cout << "video average framerate:" << framerate << " fps" << std::endl;
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
    
    for(auto& buf:frame_data){
        if(buf.size() < static_cast<size_t>( renderSize.x()*renderSize.y()*4)){
            buf.resize(renderSize.x()*renderSize.y()*4);
        }
    }
    
    try{
        readNextFrame();
        std::scoped_lock lock{frameBuffer_mutex};
        copyToBuffer(frame_data[activeBuffer]);
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not read initial frame"));
    }

    renderThread = std::async(std::launch::async, [&](){
        do{
            auto begin = libtrainsim::core::Helper::now();
            
            frameNumberMutex.lock_shared();
            uint64_t nextF = nextFrameToGet;
            uint64_t currF = currentFrameNumber;
            uint64_t _seekCutoff = seekCutoff;
            frameNumberMutex.unlock_shared();
            
            frameBuffer_mutex.lock_shared();
            auto backBuffer = activeBuffer;
            frameBuffer_mutex.unlock_shared();
            
            //select the next buffer from the active buffer as back buffer
            incrementFramebuffer(backBuffer);
            
            uint64_t diff = nextF - currF;
            
            try{
                if(diff == 0){
                    //no new frame to render so just wait and check again
                    std::this_thread::sleep_for(1ms);
                    continue;
                }else if(diff < _seekCutoff){
                    //for these small skips it is faster to simply decode frame by frame
                    while(currF < nextF){
                        readNextFrame();
                        currF++;
                    }
                }else{
                    //the next frame is more than 4 seconds in the future
                    //in this case av_seek is used to jump to that frame
                    seekFrame(nextF);
                }
                
                //update the back buffer
                copyToBuffer(frame_data[backBuffer]);
                
                //switch to the next framebuffer
                frameBuffer_mutex.lock();
                if(bufferExported){
                    incrementFramebuffer(activeBuffer);
                    bufferExported = false;
                }
                frameBuffer_mutex.unlock();
                
                //update the number of the current frame
                frameNumberMutex.lock();
                currentFrameNumber = nextF;
                frameNumberMutex.unlock();

                //append the new rendertime
                EOF_Mutex.lock();
                renderTimes.emplace_back(unit_cast(libtrainsim::core::Helper::now()-begin, prefix::milli));
                EOF_Mutex.unlock(); 
                
            }catch(const std::exception& e){
                libtrainsim::core::Helper::print_exception(e);
                std::scoped_lock lock{EOF_Mutex};
                reachedEOF = true;
                return false;
            }
            
        }while(!reachedEndOfFile());
        
        return true;
    });
    
    auto settingsTab = std::make_shared<videoDecodeSettings>(*this);
    libtrainsim::Video::imguiHandler::addSettingsTab(settingsTab);
}

libtrainsim::Video::videoReader::~videoReader() {
    libtrainsim::Video::imguiHandler::removeSettingsTab("decodeSettings");
    
    if(!reachedEndOfFile()){
        EOF_Mutex.lock();
        reachedEOF = true;
        EOF_Mutex.unlock();
    }
    
    if(renderThread.valid()){
        std::cout << "waiting for render to finish" << std::endl;
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


void libtrainsim::Video::videoReader::readNextFrame() {
    
    
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
    
}

void libtrainsim::Video::videoReader::seekFrame ( uint64_t framenumber ) {
    auto* stream = av_format_ctx->streams[video_stream_index];
    auto ts = (int64_t(framenumber) * stream->r_frame_rate.den *  stream->time_base.den) / (int64_t(stream->r_frame_rate.num) * stream->time_base.num);
    //if(av_seek_frame(av_format_ctx, video_stream_index, framenumber, AVSEEK_FLAG_FRAME) < 0){
    if(av_seek_frame(av_format_ctx, video_stream_index, ts, AVSEEK_FLAG_ANY) < 0){
        throw std::runtime_error("Problem seeking a future frame");
    }

    try{
        readNextFrame();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not retreive the seeked frame"));
    }

}

void libtrainsim::Video::videoReader::copyToBuffer ( std::vector<uint8_t>& frame_buffer ) {

    std::shared_lock<std::shared_mutex> lock{contextMutex};
    
    auto source_pix_fmt = correctForDeprecatedPixelFormat(av_codec_ctx->pix_fmt);
    sws_scaler_ctx = sws_getCachedContext(
        sws_scaler_ctx,
        renderSize.x(), 
        renderSize.y(), 
        source_pix_fmt,
        av_frame->width, 
        av_frame->height, 
        AV_PIX_FMT_RGB0,
        scalingContextParams, 
        NULL, 
        NULL, 
        NULL
    );

    uint8_t* dest[4] = { frame_buffer.data(), NULL, NULL, NULL };
    int dest_linesize[4] = { static_cast<int>(renderSize.x()) * 4, 0, 0, 0 };
    auto hnew = sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    if(hnew != av_frame->height) {throw std::runtime_error("Got a wrong size after scaling.");};
}

const std::vector<uint8_t> & libtrainsim::Video::videoReader::getUsableFramebufferBuffer() {
    std::scoped_lock lock{frameBuffer_mutex};
    
    bufferExported = true;
    return frame_data[activeBuffer];
}


const std::filesystem::path& libtrainsim::Video::videoReader::getLoadedFile() const{
    return uri;
}

bool libtrainsim::Video::videoReader::reachedEndOfFile() {
    std::shared_lock lock{EOF_Mutex};
    return reachedEOF;
}

libtrainsim::Video::dimensions libtrainsim::Video::videoReader::getDimensions() const {
    return renderSize;
}

uint64_t libtrainsim::Video::videoReader::getFrameNumber() {
    std::shared_lock lock{frameNumberMutex};
    return currentFrameNumber;
}

void libtrainsim::Video::videoReader::requestFrame(uint64_t frame_num) {
    std::scoped_lock lock{frameNumberMutex};
    if(frame_num > nextFrameToGet){
        nextFrameToGet = frame_num;
    }
}

std::optional<std::vector<sakurajin::unit_system::base::time_si>> libtrainsim::Video::videoReader::getNewRendertimes() {
    EOF_Mutex.lock();
    auto times = renderTimes;
    renderTimes.clear();
    EOF_Mutex.unlock();
    
    if(times.size() > 0){
        return std::make_optional(times);
    }else{
        return {};
    }
}
