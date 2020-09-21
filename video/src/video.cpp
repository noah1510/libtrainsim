#include "video.hpp"
#include <iostream>

#include "av.h"
#include "ffmpeg.h"
#include "codec.h"
#include "packet.h"
#include "stream.h"
#include "videorescaler.h"
#include "audioresampler.h"
#include "avutils.h"

#include "format.h"
#include "formatcontext.h"
#include "codec.h"
#include "codeccontext.h"

libtrainsim::video::video(void){
    av::init();
    av::setFFmpegLoggingLevel(AV_LOG_DEBUG);
}

void libtrainsim::video::hello_impl() const{
    std::cout<<"Hello from the video singleton"<<std::endl;
}

void libtrainsim::video::reset(){
    if (ictx.isOpened()){ictx.close();};
    if (vdec.isOpened()){vdec.close();};
    
    vst.reset();
    ictx.reset();
    ec.clear();
    count = 0;
    vdec.reset();
    loadedFile = "";
    videoStream = -1;
}

bool libtrainsim::video::load_impl(const std::filesystem::path& uri){
    loadedFile = uri;

    ictx.openInput(uri, ec);
    if (ec) {
        std::cerr << "Can't open input" << std::endl;
        reset();
        return false;
    }

    if(!setVideoStream()){
        reset();
        return false;
    }

    if(!setVideoDecoderContext()){
        reset();
        return false;
    }

    if(!init_decoding()){
        return false;
    }

    return true;
}

bool libtrainsim::video::setVideoDecoderContext(){
    if (vst.isValid()) {
        vdec = av::VideoDecoderContext(vst);

        av::Codec codec = av::findDecodingCodec(vdec.raw()->codec_id);

        vdec.setCodec(codec);
        vdec.setRefCountedFrames(true);

        vdec.open({{"threads", "1"}}, av::Codec(), ec);
        //vdec.open(ec);
        if (ec) {
            std::cerr << "Can't open codec\n";
            return false;
        }
        return true;
    }
    return false;
}

bool libtrainsim::video::setVideoStream(){
    ictx.findStreamInfo(ec);
    if (ec) {
        std::cerr << "Can't find streams: " << ec << ", " << ec.message() << std::endl;
        return false;
    }

    for (size_t i = 0; i < ictx.streamsCount(); ++i) {
        auto st = ictx.stream(i);
        if (st.mediaType() == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            vst = st;
            break;
        }
    }

    std::cerr << videoStream << std::endl;

    if (vst.isNull()) {
        std::cerr << "Video stream not found\n";
        reset();
        return false;
    }

    if (vst.isValid()){
        return true;
    }

    reset();

    return false;
}

bool libtrainsim::video::init_decoding(){
    while(auto pkt = ictx.readPacket(ec)){

        if (ec) {
            std::cerr << "Packet reading error: " << ec << ", " << ec.message() << std::endl;
            return false;
        }

        if (pkt.streamIndex() != videoStream) {
            continue;
        }

        av::VideoFrame frame = vdec.decode(pkt, ec);

        count++;

        if (ec) {
            std::cerr << "Error: " << ec << ", " << ec.message() << std::endl;
            return false;
        } else if (!frame) {
            std::cerr << "Empty frame\n";
        }
    }

    return true;
}

av::VideoDecoderContext& libtrainsim::video::getDecoder_impl(){

    return vdec;
}

av::Stream& libtrainsim::video::getVideoStream_impl(){
    
    return vst;
}

av::VideoFrame&& libtrainsim::video::getNextFrame_impl(){
    auto frame = vdec.decode(av::Packet(), ec);
    if (ec) {
        std::cerr << "Error: " << ec << ", " << ec.message() << std::endl;
        return std::move(av::VideoFrame());
    }
    if (!frame)
        return std::move(av::VideoFrame());
    return std::move(frame);
}
