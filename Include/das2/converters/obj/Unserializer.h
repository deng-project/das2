// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.h - header file for Wavefront OBJ unserializer class
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/ISerializer.h>
#include <das2/Api.h>
#include <das2/converters/obj/Data.h>
#include <variant>

namespace das2 {
    namespace obj {

        template <typename T>
        struct Token {
            std::variant<std::monostate, T, float, uint32_t, BinString> m_token = std::monostate{};
            uint32_t m_uLine = 1;
        };

        enum TokenType : size_t {
            TokenType_Unknown,
            TokenType_Keyword,
            TokenType_Float,
            TokenType_Int,
            TokenType_String
        };

        class DAS2_API UnserializerMtl : public CVar::IUnserializer<Material> {
            private:
                Token<MTLToken> m_token;
                const std::unordered_map<BinString, MTLToken> m_cMTLKeywords = {
                    { "Ka", MTLToken::Phong_Ambient },
                    { "Kd", MTLToken::Phong_Diffuse },
                    { "Ks", MTLToken::Phong_Specular },
                    { "Ns", MTLToken::Phong_SpecularExp },
                    { "d", MTLToken::Transparency },
                    { "map_Ka", MTLToken::Phong_AmbientMapUri },
                    { "map_Kd", MTLToken::Phong_DiffuseMapUri },
                    { "map_Ks", MTLToken::Phong_SpecularMapUri },
                    { "map_Ns", MTLToken::Phong_SpecularExpUri },
                    { "map_d", MTLToken::TransparencyMapUri },
                    { "Pr", MTLToken::PBR_Roughness },
                    { "Pm", MTLToken::PBR_Metallic },
                    { "Ps", MTLToken::PBR_Sheen },
                    { "Pc", MTLToken::PBR_ClearcoatThickness },
                    { "Pcr", MTLToken::PBR_ClearcoatRoughness },
                    { "Ke", MTLToken::PBR_Emissive },
                    { "aniso", MTLToken::PBR_Anisotropy },
                    { "anisor", MTLToken::PBR_AnisotropyRotation },
                    { "map_Pr", MTLToken::PBR_RoughnessMapUri },
                    { "map_Pm", MTLToken::PBR_MetallicMapUri },
                    { "map_Ps", MTLToken::PBR_SheenMapUri },
                    { "map_Ke", MTLToken::PBR_EmissionMapUri },
                    { "norm", MTLToken::PBR_NormalMapUri },
                };
        };

        class DAS2_API Unserializer : public CVar::IUnserializer<Object>  {
            private:
                Token<KeywordToken> m_token;

                const std::unordered_map<BinString, KeywordToken> m_cObjKeywords = {
                    { "v", KeywordToken::GeometryVertex },
                    { "vt", KeywordToken::TextureVertex },
                    { "vn", KeywordToken::VertexNormal },
                    { "vp", KeywordToken::ParameterSpaceVertex },
                    { "cstype", KeywordToken::CSType },
                    { "deg", KeywordToken::Degree },
                    { "bmat", KeywordToken::BasisMatrix },
                    { "step", KeywordToken::StepSize },
                    { "p", KeywordToken::Point },
                    { "l", KeywordToken::Line },
                    { "f", KeywordToken::Face },
                    { "curv", KeywordToken::Curve },
                    { "curv2", KeywordToken::Curve2D },
                    { "surf", KeywordToken::Surface },
                    { "parm", KeywordToken::ParameterValues },
                    { "trim", KeywordToken::OuterTrimmingLoop },
                    { "hole", KeywordToken::InnerTrimmingLoop },
                    { "scrv", KeywordToken::SpecialCurve },
                    { "sp", KeywordToken::SpecialPoint },
                    { "end", KeywordToken::EndStatement },
                    { "con", KeywordToken::Connect },
                    { "g", KeywordToken::GroupName },
                    { "s", KeywordToken::SmoothingGroup },
                    { "mg", KeywordToken::MergingGroup },
                    { "o", KeywordToken::ObjectName },
                    { "bevel", KeywordToken::BevelInterpolation },
                    { "c_interp", KeywordToken::ColorInterpolation },
                    { "d_interp", KeywordToken::DissolveInterpolation },
                    { "lod", KeywordToken::LevelOfDetail },
                    { "usemtl", KeywordToken::MaterialName },
                    { "mtllib", KeywordToken::MaterialLibrary },
                    { "shadow_obj", KeywordToken::ShadowCasting },
                    { "trace_obj", KeywordToken::RayTracing },
                    { "ctech", KeywordToken::CurveApprox },
                    { "stech", KeywordToken::SurfaceApprox }
                };

            private:
                void _ParseObj();

            public:
                Unserializer(std::istream& _stream);
        };
    }
}
