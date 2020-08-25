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
        return false;
    }

    return true;
}

av::VideoDecoderContext& libtrainsim::video::getDecoder_impl(){

    return vdec;
}

av::Stream& libtrainsim::video::getVideoStream_impl(){
    
    return vst;
}

/*av::Packet libtrainsim::video::getPacket_impl(){
    auto pkt = ictx.readPacket(ec);
    if (ec) {
        std::clog << "Packet reading error: " << ec << ", " << ec.message() << std::endl;
        return pkt;
    }

    bool flushDecoder = false;
    // !EOF
    if (pkt) {
        if (pkt.streamIndex() != videoStream) {
            return pkt;
        }

        std::clog << "Read packet: pts=" << pkt.pts() << ", dts=" << pkt.dts() << " / " << pkt.pts().seconds() << " / " << pkt.timeBase() << " / st: " << pkt.streamIndex() << std::endl;
    } else {
        flushDecoder = true;
    }

    count++;

    return pkt;
}*/