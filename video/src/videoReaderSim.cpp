#include "videoReaderSim.hpp"

using namespace sakurajin::unit_system;
using namespace SimpleGFX::SimpleGL;
using namespace std::literals;

/*
libtrainsim::Video::videoDecodeSettings::videoDecodeSettings ( libtrainsim::Video::videoReader& VR ) : tabPage{"decodeSettings"},
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

libtrainsim::Video::videoReaderSim::videoReaderSim(std::shared_ptr<libtrainsim::core::simulatorConfiguration> _simSettings, uint64_t threadCount, uint64_t _seekCutoff)
    :videoReader{_simSettings->getCurrentTrack().getVideoFilePath(), _simSettings->getLogger(), threadCount, _seekCutoff}, simSettings{std::move(_simSettings)}{

}
