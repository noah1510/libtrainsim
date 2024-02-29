#include "videoDecode/videoDecoderVlc.hpp"

using namespace sakurajin::unit_system;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

libtrainsim::Video::videoDecoderVlc::videoDecoderVlc(std::filesystem::path              _videoFile,
                                                       std::shared_ptr<SimpleGFX::logger> _logger,
                                                       uint64_t                           _seekCutoff,
                                                       uint64_t                           threadCount)
    : videoDecoderBase{std::move(_videoFile), std::move(_logger), _seekCutoff} {

    try{
        initVlc();
    }catch(...){
        LOGGER->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("Failed to create video decoder"));
    }

    *LOGGER << SimpleGFX::loggingLevel::debug << "Video has a framerate of " << ((double)fps_num / (double)fps_den) << " fps";

    auto [w, h] = renderSize.getCasted<int>();
    player->setVideoFormat("RV24", w, h, w * 3);
    player->setVideoCallbacks(
        sigc::mem_fun(*this, &videoDecoderVlc::lockBuffer),
        sigc::mem_fun(*this, &videoDecoderVlc::unlockBuffer),
        sigc::mem_fun(*this, &videoDecoderVlc::displayBuffer)
    );

    player->setRate(5.0);
    player->play();
    player->pause();

    readNextFrame();

    reachedEOF = false;
    startRendering();
}

libtrainsim::Video::videoDecoderVlc::~videoDecoderVlc() {
    if (!reachedEndOfFile()) {
        reachedEOF = true;
    }

    if (renderThread.valid()) {
        *LOGGER << SimpleGFX::loggingLevel::debug << "waiting for render to finish";
        renderThread.wait();
        renderThread.get();
    }

#ifdef LIBTRAINSIM_VLC4_MODE
    player->stopAsync();
#elifdef LIBTRAINSIM_VLC3_MODE
    player->stop();
#else
#error "No VLC version defined"
#endif
    player.reset();
    player = nullptr;
}

#ifdef LIBTRAINSIM_VLC4_MODE
#warning "VLC 4 mode is not tested"
void libtrainsim::Video::videoDecoderVlc::initVlc() {
    auto instance = VLC::Instance(0, nullptr);

    // load the media from the file and parse the metadata/container
    auto media = VLC::Media(uri, VLC::Media::FromPath);
    media.parseRequest(instance, VLC::Media::ParseFlags::Local | VLC::Media::ParseFlags::FetchLocal, 0);

    player = std::make_unique<VLC::MediaPlayer>(instance, media);

    // make sure the media is parsed
    do{
        auto parsedStatus = media.parsedStatus(instance);
        if(parsedStatus == VLC::Media::ParsedStatus::Failed){
            throw std::runtime_error("Error parsing media");
        }
        if(parsedStatus == VLC::Media::ParsedStatus::Done){
            break;
        }
    }while(true);

    auto tracks = player->tracks(VLC::MediaTrack::Type::Video, false);
    if (tracks.empty()) {
        throw std::runtime_error("No video track found in file");
    }

    auto& selectedTrack = std::find_if(tracks.begin(), tracks.end(), [](const auto& track) {
        return track->selected();
    });

    if(selectedTrack == tracks.end()){
        throw std::runtime_error("Somehow no video track is selected");
    }

    fps_num = selectedTrack->fps_num;
    fps_den = selectedTrack->fps_den;

    int x = static_cast<int>(selectedTrack->width());
    int y = static_cast<int>(selectedTrack->height());
    renderSize = {x, y};
}

#elifdef LIBTRAINSIM_VLC3_MODE
void libtrainsim::Video::videoDecoderVlc::initVlc() {
    auto instance = VLC::Instance(0, nullptr);

    // load the media from the file and parse the metadata/container
    auto media = VLC::Media(instance, uri.string(), VLC::Media::FromPath);
    media.parseWithOptions(VLC::Media::ParseFlags::Local | VLC::Media::ParseFlags::FetchLocal, 0);

    // make sure the media is parsed
    do{
        auto parsedStatus = media.parsedStatus();
        if(parsedStatus == VLC::Media::ParsedStatus::Failed){
            throw std::runtime_error("Error parsing media");
        }
        if(parsedStatus == VLC::Media::ParsedStatus::Done){
            break;
        }
    }while(true);

    // get the tracks from the media object
    auto trackList = media.tracks();
    if (trackList.empty()) {
        throw std::runtime_error("No track found in file");
    }

    //select the first video track
    auto selectedTrack = std::find_if(trackList.begin(), trackList.end(), [](const auto& track) {
        return track.type() == VLC::MediaTrack::Type::Video;
    });

    if (selectedTrack == trackList.end()) {
        throw std::runtime_error("No video Track found in file");
    }
    videoTrackID = selectedTrack->id();

    //get the framerate and the size of the video
    fps_num = selectedTrack->fpsNum();
    fps_den = selectedTrack->fpsDen();

    int x = static_cast<int>(selectedTrack->width());
    int y = static_cast<int>(selectedTrack->height());
    renderSize = {x, y};

    //create the media player and set the video track
    player = std::make_unique<VLC::MediaPlayer>(media);
    //if (player->setVideoTrack(videoTrackID) < 0) {
    //    throw std::runtime_error("Selecting the track was not possible");
    //}
}
#endif

void libtrainsim::Video::videoDecoderVlc::readNextFrame() {
    player->nextFrame();
}

void libtrainsim::Video::videoDecoderVlc::seekFrame(uint64_t framenumber) {
    //This calculates the timestamp in ms for the given frame number
    //The formula is based on the assumption that the framerate is constant
    //It is calculated using framenumber * 1000 / fps
    //fps is calculated as fps_num / fps_den

    uint64_t ts_ms = (framenumber * fps_den * 1000) / fps_num;
#ifdef LIBTRAINSIM_VLC4_MODE
    player->setTime(ts_ms, false);
#elifdef LIBTRAINSIM_VLC3_MODE
    player->setTime(ts_ms);
#endif
}

void libtrainsim::Video::videoDecoderVlc::copyToBuffer(std::shared_ptr<Gdk::Pixbuf>& pixbuf) {
    std::scoped_lock lock{renderSurfaceMutex};

    pixbuf = renderSurface;
    renderSurface = nullptr;
}

//bool libtrainsim::Video::videoDecoderVlc::renderLoop() {}

void* libtrainsim::Video::videoDecoderVlc::lockBuffer(void** p_pixels) {
    renderSurfaceMutex.lock();

    auto [w,h] = renderSize.getCasted<int>();
    if (renderSurface == nullptr || renderSurface->get_width() != w || renderSurface->get_height() != h) {
        renderSurface = Gdk::Pixbuf::create(Gdk::Colorspace::RGB, false, 8, w, h);
    }

    *p_pixels = renderSurface->get_pixels();
    return nullptr;
}

void libtrainsim::Video::videoDecoderVlc::unlockBuffer(void* id, void* const* p_pixels) {
    renderSurfaceMutex.unlock();
}

void libtrainsim::Video::videoDecoderVlc::displayBuffer(void* id) {}

