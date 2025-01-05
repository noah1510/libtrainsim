#include "videoDecode/videoDecoderBase.hpp"

using namespace sakurajin::unit_system;
using namespace SimpleGFX::gl;
using namespace std::literals;

uint8_t libtrainsim::Video::videoDecoderBase::incrementFramebuffer(uint8_t currentBuffer) const {
    return (currentBuffer + 1) % FRAME_BUFFER_COUNT;
}

libtrainsim::Video::videoDecoderBase::videoDecoderBase(std::filesystem::path                    videoFile,
                                                       std::shared_ptr<SimpleGFX::core::logger> _logger,
                                                       uint64_t                                 _seekCutoff)
    : seekCutoff{_seekCutoff},
      LOGGER{std::move(_logger)} {

    if (!std::filesystem::exists(videoFile) || videoFile.empty()) {
        throw std::invalid_argument("video file does not exist or is empty");
    }

    uri        = videoFile;
    reachedEOF = true;
}

void libtrainsim::Video::videoDecoderBase::startRendering() {
    renderThread = std::async(std::launch::async, sigc::mem_fun(*this, &videoDecoderBase::renderLoopCaller));
}

bool libtrainsim::Video::videoDecoderBase::renderLoopCaller() {
    return renderLoop();
}

bool libtrainsim::Video::videoDecoderBase::renderLoop() {
    do {
        // if there was an error in the last frame exit the render loop
        if (!renderRequestedFrame()) {
            return false;
        }
    } while (!reachedEndOfFile());

    return true;
}

bool libtrainsim::Video::videoDecoderBase::renderRequestedFrame() {
    auto begin = SimpleGFX::core::now();

    // create local copies of nextFrameToGet, currentFrameNumber and seekCutoff
    const uint64_t nextF       = nextFrameToGet;
    const uint64_t currF       = currentFrameNumber;
    const uint64_t _seekCutoff = seekCutoff;

    static bool export_skipped = false;

    // select the next buffer from the active buffer as back buffer
    const auto backBuffer = incrementFramebuffer(activeBuffer);

    // calculate the difference in frames
    // this variable is used to determine if a new frame has to be decoded,
    // if the specified frame should be seek or if frames should be decoded
    // until the difference is 0
    uint64_t diff = nextF - currF;

    // If no new frame is requested just wait and check again
    // in case an export was skipped a buffer swap is performed when possible
    if (diff == 0) {
        if (export_skipped) {
            if (!isExporting) {
                activeBuffer   = backBuffer;
                bufferExported = false;
                export_skipped = false;
                return true;
            }
        }

        // no new frame to render so just wait and check again
        std::this_thread::sleep_for(10us);

        if (export_skipped) {
            if (!isExporting) {
                activeBuffer   = backBuffer;
                bufferExported = false;
                export_skipped = false;
                return true;
            }
        }

        return true;
    }

    try {
        if (diff < _seekCutoff) {
            // for these small skips it is faster to simply decode frame by frame
            while (diff > 0) {
                readNextFrame(backBuffer);
                diff--;
            }
        } else {
            // the next frame is more than 4 seconds in the future
            // in this case av_seek is used to jump to that frame
            seekFrame(backBuffer, nextF);
        }

        // switch to the next framebuffer
        if (isExporting) {
            export_skipped = true;
        } else {
            activeBuffer   = backBuffer;
            bufferExported = false;
            export_skipped = false;
        }

        // update the number of the current frame
        currentFrameNumber = nextF;

        // append the new rendertime
        renderTimeMutex.lock();
        auto dt = SimpleGFX::core::now() - begin;
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
        *LOGGER << SimpleGFX::core::loggingLevel::debug << "waiting for render to finish";
        renderThread.wait();
        renderThread.get();
    }
}


std::shared_ptr<Gdk::Texture> libtrainsim::Video::videoDecoderBase::getUsableTexture(std::shared_ptr<Gdk::Texture> texture) {
    const auto exportBufferID = activeBuffer.load();
    auto [w, h]               = renderSize.getCasted<int>();

    // if a pixbuf was given and the buffer already exported
    // it is assumed that the pixbuf is already up-to-date
    if (bufferExported && texture != nullptr) {
        return texture;
    }

    // copy the decoded frame into the given texture
    copyToBuffer(exportBufferID, texture);

    // mark the buffer as exported
    bufferExported = true;

    // if the pixbuf was not given return the usablePixbuf otherwise return the given pixbuf
    return texture;
}

std::shared_ptr<Gdk::Texture> libtrainsim::Video::videoDecoderBase::getUsableTexture() {
    return getUsableTexture(nullptr);
}

bool libtrainsim::Video::videoDecoderBase::hasNewTexture() {
    return !bufferExported;
}

bool libtrainsim::Video::videoDecoderBase::hasNewFramebuffer() {
    return hasNewTexture();
}

const std::filesystem::path& libtrainsim::Video::videoDecoderBase::getLoadedFile() const {
    return uri;
}

bool libtrainsim::Video::videoDecoderBase::reachedEndOfFile() const {
    return reachedEOF;
}

SimpleGFX::core::dimensions libtrainsim::Video::videoDecoderBase::getDimensions() const {
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
