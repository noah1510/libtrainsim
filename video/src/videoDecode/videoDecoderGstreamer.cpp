#include "videoDecode/videoDecoderGstreamer.hpp"

using namespace sakurajin::unit_system;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;


libtrainsim::Video::videoDecoderGstreamer::videoDecoderGstreamer(std::filesystem::path              _videoFile,
                                                                 std::shared_ptr<SimpleGFX::logger> _logger,
                                                                 uint64_t                           _seekCutoff,
                                                                 uint64_t                           threadCount)
    // set the seek cutoff to 1 since gstreamer does not support advancing by one frame
    : videoDecoderBase{std::move(_videoFile), std::move(_logger), 0} {

    // if gstreamer is already initialized skip this
    if (!gst_is_initialized()) {
        // init gstreamer and throw if it fails
        if (!gst_init_check(NULL, NULL, NULL)) {
            throw std::runtime_error("Could not initialize gstreamer");
        }
    }

    // init the gstreamer pipleine elements
    source = gst_element_factory_make("uridecodebin", "source");
#ifdef USE_VIDEORATE
    videorate = gst_element_factory_make("videorate", "videorate");
#endif
    convert = gst_element_factory_make("videoconvert", "convert");
    sink    = gst_element_factory_make("gdkpixbufsink", "sink");

    pipeline = gst_pipeline_new("test-pipeline");

    // check if all elements could be created
#ifdef USE_VIDEORATE
    if (!pipeline || !source || !videorate || !convert || !sink) {
        throw std::runtime_error("Not all elements could be created.");
    }
#else
    if (!pipeline || !source || !convert || !sink) {
        throw std::runtime_error("Not all elements could be created.");
    }
#endif

    // add all elements to the pipeline and link the converter and sink
#ifdef USE_VIDEORATE
    gst_bin_add_many(GST_BIN(pipeline), source, videorate, convert, sink, NULL);
    if (!gst_element_link_many(videorate, convert, sink, NULL)) {
        gst_object_unref(pipeline);
        throw std::runtime_error("Elements could not be linked.");
    }
#else
    gst_bin_add_many(GST_BIN(pipeline), source, convert, sink, NULL);
    if (!gst_element_link_many(convert, sink, NULL)) {
        gst_object_unref(pipeline);
        throw std::runtime_error("Elements could not be linked.");
    }
#endif

    // g_object_set(videorate, "rate", 1.0, NULL);
    g_object_set(sink, "post-messages", false, "show-preroll-frame", true, "enable-last-sample", true, NULL);

    g_object_set(source, "uri", ("file://" + uri.string()).c_str(), NULL);
    g_signal_connect(source, "pad-added", G_CALLBACK(videoDecoderGstreamer::pad_added_callback_connector), this);

    auto ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
    // auto ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        gst_object_unref(pipeline);
        throw std::runtime_error("Unable to set the pipeline to the playing state.");
    }

    // this is a hack to wait for the pad_added_handler to be called
    // this essentially stalls until the video pad has been added to the pipeline
    //@TODO: find a better way to do this that allows for an exception to be thrown if the pad is not added
    while (reachedEOF) {
        std::this_thread::sleep_for(1ms);
    }

    auto bus     = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    bus_watch_id = gst_bus_add_watch(bus, videoDecoderGstreamer::handleMessages_connector, this);
    gst_object_unref(bus);

    bool      supports_default_format = false;
    bool      supports_time_format    = false;
    GstQuery* query                   = nullptr;
    query                             = gst_query_new_formats();
    if (gst_element_query(pipeline, query)) {
        unsigned int format_count;
        gst_query_parse_n_formats(query, &format_count);

        *LOGGER << SimpleGFX::loggingLevel::debug << "There are " << format_count << " formats supported by this stream.";

        for (uint i = 0; i < format_count; i++) {
            GstFormat format;
            gst_query_parse_nth_format(query, i, &format);
            *LOGGER << SimpleGFX::loggingLevel::debug << "Format " << i << " is " << gst_format_get_name(format);

            if (format == GST_FORMAT_DEFAULT) {
                supports_default_format = true;
            }

            if (format == GST_FORMAT_TIME) {
                supports_time_format = true;
            }
        }
    } else {
        *LOGGER << SimpleGFX::loggingLevel::error << "formats query failed.";
    }
    gst_query_unref(query);

    if (!supports_default_format) {
        throw std::runtime_error("Default format is not supported");
    }

    // start the rendering thread
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

    if (bus_watch_id != 0) {
        g_source_remove(bus_watch_id);
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

void libtrainsim::Video::videoDecoderGstreamer::pad_added_handler(GstElement* src, GstPad* new_pad) {
#ifdef USE_VIDEORATE
    GstPad* sink_pad = gst_element_get_static_pad(videorate, "sink");
#else
    GstPad* sink_pad = gst_element_get_static_pad(convert, "sink");
#endif

    *LOGGER << SimpleGFX::loggingLevel::normal << "Received new pad '" << GST_PAD_NAME(new_pad) << "' from '" << GST_ELEMENT_NAME(src);

    /* If our converter is already linked, we have nothing to do here */
    if (gst_pad_is_linked(sink_pad)) {
        *LOGGER << SimpleGFX::loggingLevel::error << "We are already linked. Ignoring.";

        /* Unreference the sink pad */
        gst_object_unref(sink_pad);
        return;
    }

    /* Check the new pad's type */
    GstCaps*      new_pad_caps   = gst_pad_get_current_caps(new_pad);
    GstStructure* new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    const gchar*  new_pad_type   = gst_structure_get_name(new_pad_struct);

    /* Unreference the new pad's caps, if we got them */
    if (new_pad_caps != NULL) {
        gst_caps_unref(new_pad_caps);
    }

    // chcek if the new pad is a video pad
    if (!g_str_has_prefix(new_pad_type, "video/x-raw")) {
        *LOGGER << SimpleGFX::loggingLevel::error << "It has type '" << new_pad_type << "' which is not raw video. Ignoring.";

        /* Unreference the sink pad */
        gst_object_unref(sink_pad);
        return;
    }

    int w = 0;
    int h = 0;
    // get the video size and set it as the render size
    // return if there was an error
    if (!gst_structure_get_int(new_pad_struct, "width", &w) || !gst_structure_get_int(new_pad_struct, "height", &h)) {
        *LOGGER << SimpleGFX::loggingLevel::error << "No width/height available";
        return;
    }

    *LOGGER << SimpleGFX::loggingLevel::normal << "video size is: " << w << "x" << h;
    renderSize = {w, h};

    /* Attempt the link */
    GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED(ret)) {
        *LOGGER << SimpleGFX::loggingLevel::error << "Type is '" << new_pad_type << "' but link failed.";
    } else {
        *LOGGER << SimpleGFX::loggingLevel::normal << "Link succeeded (type '" << new_pad_type << "').";
    }

    // mark the pipeline as paused
    gst_element_set_state(pipeline, GST_STATE_PAUSED);

    // after linking the video source mark the pipeline as being able to play
    reachedEOF = false;

    /* Unreference the sink pad */
    gst_object_unref(sink_pad);
}

void libtrainsim::Video::videoDecoderGstreamer::copyToBuffer(std::shared_ptr<Gdk::Pixbuf>& pixbuf) {
    GdkPixbuf* c_pixbuf = nullptr;
    g_object_get(G_OBJECT(sink), "last-pixbuf", &c_pixbuf, nullptr);

    pixbuf = Glib::wrap(c_pixbuf, true);
}
bool libtrainsim::Video::videoDecoderGstreamer::handleMessages(GstBus* bus, GstMessage* msg) {

    if (GST_MESSAGE_TYPE(msg) != GST_MESSAGE_TAG) {
        *LOGGER << SimpleGFX::loggingLevel::debug << "got message: " << GST_MESSAGE_TYPE_NAME(msg);
    } else {
        if (logTags) {
            *LOGGER << SimpleGFX::loggingLevel::debug << "got tag message";
            GstTagList* tags = NULL;

            gst_message_parse_tag(msg, &tags);
            *LOGGER << SimpleGFX::loggingLevel::debug << "Got tags from element '" << GST_OBJECT_NAME(msg->src)
                    << "';content: " << gst_tag_list_to_string(tags);
            // handle_tags (tags);
            gst_tag_list_unref(tags);
        }
    }

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        GError* err        = nullptr;
        gchar*  debug_info = nullptr;
        gst_message_parse_error(msg, &err, &debug_info);
        *LOGGER << SimpleGFX::loggingLevel::error << "Error received from element " << GST_OBJECT_NAME(msg->src) << ": " << err->message;

        if (debug_info) {
            *LOGGER << SimpleGFX::loggingLevel::debug << "Debugging information: " << debug_info;
        }

        reachedEOF = true;

        // free memory
        g_clear_error(&err);
        g_free(debug_info);

        return false;
    } // End of GST_MESSAGE_ERROR

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS) {
        *LOGGER << SimpleGFX::loggingLevel::normal << "End-Of-Stream reached.";
        reachedEOF = true;
        return false;
    } // End of GST_MESSAGE_EOS

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STATE_CHANGED) {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
        *LOGGER << SimpleGFX::loggingLevel::debug << "Pipeline state changed from " << gst_element_state_get_name(old_state) << " to "
                << gst_element_state_get_name(new_state);
    } // End of GST_MESSAGE_STATE_CHANGED

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STEP_START) {
        gboolean  stepActive       = false;
        GstFormat stepFormat       = GST_FORMAT_UNDEFINED;
        uint64_t  stepDistance     = 0;
        double    stepRate         = 1.0;
        gboolean  stepFlush        = false;
        gboolean  stepIntermediate = false;

        gst_message_parse_step_start(msg, &stepActive, &stepFormat, &stepDistance, &stepRate, &stepFlush, &stepIntermediate);
        *LOGGER << SimpleGFX::loggingLevel::debug << "Step start: active: " << stepActive << " format: " << stepFormat
                << " distance: " << stepDistance << " rate: " << stepRate << " flush: " << stepFlush
                << " intermediate: " << stepIntermediate;

        // mark that a step can be performed again
        if (stepActive) {
            isStepping         = false;
            currentFrameNumber = static_cast<uint64_t>(currentFrameNumber + stepDistance);
            // gst_element_set_state(pipeline, GST_STATE_PLAYING);
        }
    } // End of GST_MESSAGE_STEP_START

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STEP_DONE) {
        GstFormat stepFormat       = GST_FORMAT_UNDEFINED;
        uint64_t  stepDistance     = 0;
        double    stepRate         = 1.0;
        gboolean  stepFlush        = false;
        gboolean  stepIntermediate = false;
        uint64_t  stepDuration     = 0;
        gboolean  reachedEOS       = false;

        gst_message_parse_step_done(msg, &stepFormat, &stepDistance, &stepRate, &stepFlush, &stepIntermediate, &stepDuration, &reachedEOS);
        *LOGGER << SimpleGFX::loggingLevel::debug << "Step done: format: " << stepFormat << " distance: " << stepDistance
                << " rate: " << stepRate << " flush: " << stepFlush << " intermediate: " << stepIntermediate
                << " duration: " << stepDuration << " eos: " << reachedEOS;

        // mark that a step can be performed again
        isStepping = false;

        if (reachedEOS) {
            reachedEOF = true;
        }

        // select the next buffer from the active buffer as back buffer
        currentFrameNumber = static_cast<uint64_t>(stepDuration);

    } // End of GST_MESSAGE_STEP_DONE

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_SEGMENT_DONE) {

        GstFormat stepFormat = GST_FORMAT_UNDEFINED;
        int64_t  position   = 0;
        gst_message_parse_segment_done(msg, &stepFormat, &position);

        *LOGGER << SimpleGFX::loggingLevel::debug << "segment done: format: " << stepFormat << " position: " << position;

        currentFrameNumber = static_cast<uint64_t>(position);
        isStepping = false;
    } // End of GST_MESSAGE_SEGMENT_DONE

    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ASYNC_DONE) {
        *LOGGER << SimpleGFX::loggingLevel::debug << "async done. Copying frame";

        const auto backBuffer = incrementFramebuffer(activeBuffer);
        // select the next buffer from the active buffer as back buffer
        copyToBuffer(frame_data[backBuffer]);

        // switch to the next framebuffer in case the old one was exported
        if (bufferExported) {
            activeBuffer   = backBuffer;
            bufferExported = false;
        }
    } // End of GST_MESSAGE_ASYNC_DONE

    return !reachedEOF;
}

bool libtrainsim::Video::videoDecoderGstreamer::renderLoop() {
    auto begin = SimpleGFX::chrono::now();

    do {

        int64_t currentPipelineFrame = 0;
        /* Query the current position of the stream */
#ifdef USE_VIDEORATE
        g_object_get(G_OBJECT(videorate), "in", &currentPipelineFrame, NULL);
#else
        // get the current frame from the pipeline and update the current frame number
        // if that is not possible just update the buffer content
        if (gst_element_query_position(pipeline, GST_FORMAT_DEFAULT, &currentPipelineFrame)) {
            currentFrameNumber = static_cast<uint64_t>(currentPipelineFrame);
        }
#endif

        // create local copies of nextFrameToGet, currentFrameNumber and seekCutoff
        const int64_t nextF = static_cast<int64_t>(nextFrameToGet.load());
        const int64_t currF = static_cast<int64_t>(currentPipelineFrame ? currentPipelineFrame : currentFrameNumber.load());

        // get the pipeline state
        /*
        GstState pipeline_state    = GST_STATE_VOID_PENDING;
        GstState pipeline_pending  = GST_STATE_VOID_PENDING;
        auto     state_switch_type = gst_element_get_state(pipeline, &pipeline_state, &pipeline_pending, GST_NSECOND * 10);
        switch (state_switch_type) {
            case (GST_STATE_CHANGE_FAILURE):
                *LOGGER << SimpleGFX::loggingLevel::error << "could not get pipeline state";
                reachedEOF = true;
                return false;
            case (GST_STATE_CHANGE_NO_PREROLL):
                *LOGGER << SimpleGFX::loggingLevel::debug << "pipeline state is no preroll. So no data update might be visible";
            case (GST_STATE_CHANGE_ASYNC):
                if (pipeline_pending != GST_STATE_VOID_PENDING) {
                    //*LOGGER << SimpleGFX::loggingLevel::debug << "pipeline state is still changing from "
                    //        << gst_element_state_get_name(pipeline_state) << " to " << gst_element_state_get_name(pipeline_pending);
                    break;
                }
            case (GST_STATE_CHANGE_SUCCESS):
                // check if the returned queried frame is the same as the requested or newer
                // in that case pause the pipeline
                // if (currF >= nextF && pipeline_state == GST_STATE_PLAYING) {
                // gst_element_set_state(pipeline, GST_STATE_PAUSED);
                //}
                break;
            default:
                *LOGGER << SimpleGFX::loggingLevel::error << "unknown pipeline state";
                reachedEOF = true;
                return false;
        }*/


        // if the pipeline is paused and the next frame is newer than the current one
        // start playing on the pipeline
        /*if (currentPipelineFrame < nextF && pipeline_state == GST_STATE_PAUSED) {
            gst_element_set_state(pipeline, GST_STATE_PLAYING);
        }*/

        // perform a step if needed
        if (!isStepping) {
            auto diff = nextF - currF;

            if (diff > 0) {
                /*
                auto stepEvent = gst_event_new_step(GST_FORMAT_DEFAULT, diff, 10.0, true, false);
                if (!gst_element_send_event(pipeline, stepEvent)) {
                    *LOGGER << SimpleGFX::loggingLevel::error << "could not send step event";
                    reachedEOF = true;
                    return false;
                }*/

                auto seek_flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_TRICKMODE);
                auto seek_event = gst_event_new_seek(10.0, GST_FORMAT_DEFAULT, seek_flags, GST_SEEK_TYPE_SET, nextF, GST_SEEK_TYPE_END, 0);
                if (!gst_element_send_event(sink, seek_event)) {
                    *LOGGER << SimpleGFX::loggingLevel::error << "could not send seek event";
                    reachedEOF = true;
                    return false;
                }

                isStepping = true;
            }
        }

        std::this_thread::sleep_for(1ms);

    } while (!reachedEOF);

    return true;
}

/*
void libtrainsim::Video::videoDecoderGstreamer::readNextFrame() {}

void libtrainsim::Video::videoDecoderGstreamer::seekFrame(uint64_t framenumber) {}
 */
