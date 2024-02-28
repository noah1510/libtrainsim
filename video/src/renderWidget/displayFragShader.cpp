#include "renderWidget/displayFragShader.hpp"

using namespace SimpleGFX::SimpleGL;

static constexpr const shaderStage stage = shaderStage::FRAGMENT;

static void addVersion(std::stringstream& source, glVersion version){
    source << getGLShaderVersion(version);
}

static void addCommonParts(std::stringstream& source, unsigned int texUnits){
    source << R""""(
        precision mediump float;
        layout(location = 0) out vec4 FragColor;
        in vec2 TexCoord;
        in vec2 Coord;
    )"""";

    source << "    uniform sampler2D tex[" << texUnits << "];\n";
    source << R""""(
        uniform uint enabledUnits;
        void main(){
            vec4 outColor;
            FragColor = vec4(0.0,0.0,0.0,0.0);
    )"""";
}

static std::shared_ptr<shaderPart> makeCompatShader(glVersion version, unsigned int texUnits){
    std::stringstream fragmentSource;
    addVersion(fragmentSource, version);
    addCommonParts(fragmentSource, texUnits);

    std::vector<int> units{};
    units.reserve(texUnits);
    for (unsigned int i = 0; i < texUnits; i++) {
        units.emplace_back(i);
        fragmentSource << "if(enabledUnits > " << i << "u){";
        fragmentSource << "    outColor = texture(tex[" << i << "], TexCoord);";
        fragmentSource << "    FragColor = mix(FragColor, outColor, outColor.a);";
        fragmentSource << "}else{return;}";
    }

    fragmentSource << "}" << std::endl;
    return std::make_shared<shaderPart>(fragmentSource.str(), stage, version);
}

static std::shared_ptr<shaderPart> makeModernShader(glVersion version, unsigned int texUnits){
    std::stringstream fragmentSource;
    addVersion(fragmentSource, version);
    addCommonParts(fragmentSource, texUnits);

    fragmentSource << R""""(
            for(uint i = 0u; i < enabledUnits; i++){
                outColor = texture(tex[i], TexCoord);
                FragColor = mix(FragColor, outColor, outColor.a);
            }
        }
    )"""";
    return std::make_shared<shaderPart>(fragmentSource.str(), stage, version);
}

static inline bool useCompatShader(const glVersion& version){
    return isGLES(version) || version == glVersion::GL_33_CORE;
}

libtrainsim::Video::displayFragShader::displayFragShader(unsigned int texUnits) : shaderPartGroup{stage}{
    const auto& versions = getAllGLVersions();

    for(const auto& version: versions){
        if(useCompatShader(version)){
            addPart(makeCompatShader(version, texUnits));
        }else{
            addPart(makeModernShader(version, texUnits));
        }

    }
}

