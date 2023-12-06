#include "videoDecode/videoDecoderBase.hpp"

using namespace sakurajin::unit_system;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

uint8_t libtrainsim::Video::videoDecoderBase::incrementFramebuffer(uint8_t currentBuffer) const {
    return (currentBuffer + 1) % FRAME_BUFFER_COUNT;
}

libtrainsim::Video::videoDecoderBase::videoDecoderBase(std::filesystem::path              videoFile,
                                                       std::shared_ptr<SimpleGFX::logger> _logger,
                                                       uint64_t                           _seekCutoff)
    : seekCutoff{_seekCutoff},
      LOGGER{std::move(_logger)} {

    if (!std::filesystem::exists(videoFile) || videoFile.empty()) {
        throw std::invalid_argument("video file does not exist or is empty");
    }

    uri = videoFile;
    reachedEOF = true;
}

void libtrainsim::Video::videoDecoderBase::startRendering() {
    auto [w, h] = renderSize.getCasted<int>();
    for (auto& buf : frame_data) {
        buf = Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, w, h);
    }

    renderThread = std::async(std::launch::async, sigc::mem_fun(*this, &videoDecoderBase::renderLoopCaller));
}

bool libtrainsim::Video::videoDecoderBase::renderLoopCaller() {
    return renderLoop();
}

bool libtrainsim::Video::videoDecoderBase::renderLoop() {
    do {
        //if there was an error in the last frame exit the render loop
        if(!renderRequestedFrame()){
            return false;
        }
    } while (!reachedEndOfFile());

    return true;
}

bool libtrainsim::Video::videoDecoderBase::renderRequestedFrame() {
    auto begin = SimpleGFX::chrono::now();

    // create local copies of nextFrameToGet, currentFrameNumber and seekCutoff
    const uint64_t nextF       = nextFrameToGet;
    const uint64_t currF       = currentFrameNumber;
    const uint64_t _seekCutoff = seekCutoff;

    // select the next buffer from the active buffer as back buffer
    const auto backBuffer = incrementFramebuffer(activeBuffer);

    // calculate the difference in frames
    // this variable is used to determine if a new frame has to be decoded,
    // if the specified frame should be seek or if frames should be decoded
    // until the difference is 0
    uint64_t diff = nextF - currF;

    try {
        if (diff == 0) {
            // no new frame to render so just wait and check again
            std::this_thread::sleep_for(1ms);
            return true;
        } else if (diff < _seekCutoff) {
            // for these small skips it is faster to simply decode frame by frame
            while (diff > 0) {
                readNextFrame();
                diff--;
            }
        } else {
            // the next frame is more than 4 seconds in the future
            // in this case av_seek is used to jump to that frame
            seekFrame(nextF);
        }

        // update the back buffer
        fillInternalPixbuf(frame_data[backBuffer]);

        // switch to the next framebuffer
        if (bufferExported) {
            activeBuffer   = incrementFramebuffer(activeBuffer);
            bufferExported = false;
        }

        // update the number of the current frame
        currentFrameNumber = nextF;

        // append the new rendertime
        renderTimeMutex.lock();
        auto dt = SimpleGFX::chrono::now() - begin;
        renderTimes.emplace_back(unit_cast(dt));
        renderTimeMutex.unlock();

    } catch (...) {
        // if an error happened set EOF and exit the render loop
        LOGGER->logCurrrentException();
        reachedEOF = true;
        return false;
    }

    return true;
}

libtrainsim::Video::videoDecoderBase::~videoDecoderBase() {
    // imguiHandler::removeSettingsTab("decodeSettings");

    if (!reachedEndOfFile()) {
        reachedEOF = true;
    }

    if (renderThread.valid()) {
        *LOGGER << SimpleGFX::loggingLevel::debug << "waiting for render to finish";
        renderThread.wait();
        renderThread.get();
    }
}


void libtrainsim::Video::videoDecoderBase::readNextFrame() {}

void libtrainsim::Video::videoDecoderBase::seekFrame(uint64_t framenumber) {}

void libtrainsim::Video::videoDecoderBase::copyToBuffer(std::vector<uint8_t>& frame_buffer) {}

void libtrainsim::Video::videoDecoderBase::fillInternalPixbuf(std::shared_ptr<Gdk::Pixbuf>& pixbuf) {
    std::vector<uint8_t> rawBuffer;
    auto [w, h] = renderSize.getCasted<int>();
    rawBuffer.resize(w * h * 4);
    copyToBuffer(rawBuffer);

    // create a pixbuf from the data
    pixbuf = Gdk::Pixbuf::create_from_data(rawBuffer.data(), Gdk::Colorspace::RGB, true, 8, w, h, w * 4);
}

std::shared_ptr<Gdk::Pixbuf> libtrainsim::Video::videoDecoderBase::getUsablePixbuf(std::shared_ptr<Gdk::Pixbuf> pixbuf) {
    const auto exportBufferID = activeBuffer.load();
    auto [w, h]               = renderSize.getCasted<int>();

    // if a pixbuf was given and the buffer already exported
    // it is assumed that the pixbuf is already up-to-date
    if (bufferExported && pixbuf != nullptr) {
        return pixbuf;
    }

    //move the pixbuf to the return value
    auto useablePixbuf = std::move(frame_data[exportBufferID]);

    // if a pixbuf was given copy the data into it
    if (pixbuf != nullptr) {
        useablePixbuf->copy_area(0, 0, w, h, pixbuf, 0, 0);
    }

    // mark the buffer as exported
    bufferExported = true;

    //if the pixbuf was not given return the usablePixbuf otherwise return the given pixbuf
    return pixbuf == nullptr ? std::move(useablePixbuf) : std::move(pixbuf);
}

bool libtrainsim::Video::videoDecoderBase::hasNewPixbuf() {
    return !bufferExported;
}

bool libtrainsim::Video::videoDecoderBase::hasNewFramebuffer() {
    return hasNewPixbuf();
}

const std::filesystem::path& libtrainsim::Video::videoDecoderBase::getLoadedFile() const {
    return uri;
}

bool libtrainsim::Video::videoDecoderBase::reachedEndOfFile() const {
    return reachedEOF;
}

dimensions libtrainsim::Video::videoDecoderBase::getDimensions() const {
    return renderSize;
}

uint64_t libtrainsim::Video::videoDecoderBase::getFrameNumber() {
    return currentFrameNumber;
}

bool libtrainsim::Video::videoDecoderBase::requestFrame(uint64_t frame_num) {
    if (frame_num > nextFrameToGet) {
        nextFrameToGet = frame_num;
        return true;
    }
    return false;
}

std::optional<std::vector<sakurajin::unit_system::time_si>> libtrainsim::Video::videoDecoderBase::getNewRendertimes() {
    renderTimeMutex.lock();
    auto times = renderTimes;
    renderTimes.clear();
    renderTimeMutex.unlock();

    if (times.empty()) {
        return std::nullopt;
    } else {
        return std::make_optional(times);
    }
}
