#pragma once

#include "Common.h"

#include <string>

namespace UniformType {
// standard
const std::string MODEL_MATRIX = "uModelMatrix";
const std::string VIEW_MATRIX = "uViewMatrix";
const std::string PROJECTION_MATRIX = "uProjectionMatrix";
const std::string INVERSE_MODEL_MATRIX = "uInverseModelMatrix";
const std::string INVERSE_VIEW_MATRIX = "uInverseViewMatrix";
const std::string INVERSE_PROJECTION_MATRIX = "uInverserojectionMatrix";
const std::string TEXTURE_SAMPLER0 = "uTextureSampler0";
const std::string TEXTURE_SAMPLER1 = "uTextureSampler1";
const std::string TEXTURE_SAMPLER2 = "uTextureSampler2";
const std::string TEXTURE_SAMPLER3 = "uTextureSampler3";
const std::string CUBEMAP_SAMPLER0 = "uCubeMapSampler0";
const std::string CUBEMAP_SAMPLER1 = "uCubeMapSampler1";
const std::string CUBEMAP_SAMPLER2 = "uCubeMapSampler2";
const std::string CUBEMAP_SAMPLER3 = "uCubeMapSampler3";
// font
const std::string FONT_UV_OFFSET = "uFontUVOffset";
const std::string FONT_UV_SIZE = "uFontUVSize";
// material
const std::string DIFFUSE_COLOR = "uDiffuseColor";
const std::string AMBIENT_COLOR = "uAmbientColor";
const std::string SPECULAR_COLOR = "uSpecularColor";
const std::string EMISSIVE_COLOR = "uEmissiveColor";
const std::string SHININESS = "uShininess";
const std::string AMBIENT_TEXTURE = "uAmbientTexture";
const std::string DIFFUSE_TEXTURE = "uDiffuseTexture";
const std::string SPECULAR_TEXTURE = "uSpecularTexture";
const std::string EMISSIVE_TEXTURE = "uEmissiveTexture";
const std::string SHININESS_TEXTURE = "uShininessTexture";
const std::string USE_TEXTURE = "uUseTexture";
const std::string REFLECTION = "uReflection";
// lighting
const std::string LIGHT_POSITION0 = "uLightPosition0";
const std::string LIGHT_POSITION1 = "uLightPosition1";
const std::string LIGHT_POSITION2 = "uLightPosition2";
const std::string LIGHT_POSITION3 = "uLightPosition3";
}
