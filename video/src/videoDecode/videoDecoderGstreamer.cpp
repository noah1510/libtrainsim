#include "videoDecode/videoDecoderGstreamer.hpp"

using namespace sakurajin::unit_system;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;


libtrainsim::Video::videoDecoderGstreamer::videoDecoderGstreamer(std::filesystem::path              _videoFile,
                                                                 std::shared_ptr<SimpleGFX::logger> _logger,
                                                                 uint64_t                           _seekCutoff,
                                                                 uint64_t                           threadCount)
    : videoDecoderBase{std::move(_videoFile), std::move(_logger), _seekCutoff} {

    // if gstreamer is already initialized skip this
    if (!gst_is_initialized()) {
        // init gstreamer and throw if it fails
        if (!gst_init_check(NULL, NULL, NULL)) {
            throw std::runtime_error("Could not initialize gstreamer");
        }
    }

    // init the gstreamer pipleine elements
    data.source  = gst_element_factory_make("uridecodebin", "source");
    data.convert = gst_element_factory_make("videoconvert", "convert");
    data.sink    = gst_element_factory_make("gdkpixbufsink", "sink");

    data.pipeline = gst_pipeline_new("test-pipeline");

    // check if all elements could be created
    if (!data.pipeline || !data.source || !data.convert || !data.sink) {
        throw std::runtime_error("Not all elements could be created.");
    }

    // add all elements to the pipeline and link the converter and sink
    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.convert, data.sink, NULL);
    if (!gst_element_link_many(data.convert, data.sink, NULL)) {
        gst_object_unref(data.pipeline);
        throw std::runtime_error("Elements could not be linked.");
    }

    g_object_set(data.source, "uri", uri.string().c_str(), NULL);


    reachedEOF = false;
    startRendering();
}

libtrainsim::Video::videoDecoderGstreamer::~videoDecoderGstreamer() {
    if (!reachedEndOfFile()) {
        reachedEOF = true;
    }

    if (renderThread.valid()) {
        *LOGGER << SimpleGFX::loggingLevel::debug << "waiting for render to finish";
        renderThread.wait();
        renderThread.get();
    }

    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);
}


void libtrainsim::Video::videoDecoderGstreamer::readNextFrame() {}

void libtrainsim::Video::videoDecoderGstreamer::seekFrame(uint64_t framenumber) {}

void libtrainsim::Video::videoDecoderGstreamer::copyToBuffer(std::vector<uint8_t>& frame_buffer) {}
