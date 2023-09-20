// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.h - header file for Wavefront OBJ unserializer class
// author: Karl-Mihkel Ott

#pragma once

#include <sstream>
#include <string>
#include <cvar/ISerializer.h>
#include <cvar/SerializerExceptions.h>
#include <das2/Api.h>
#include <das2/converters/obj/Data.h>
#include <variant>

namespace das2 {
    namespace obj {

        template <typename T>
        struct Token {
            std::variant<std::monostate, T, BinString> token = std::monostate{};
            uint32_t uLine = 1;
        };

        std::ostream& operator<<(std::ostream& _stream, std::variant<std::monostate, KeywordToken, BinString>& _val);
        std::ostream& operator<<(std::ostream& _stream, std::variant<std::monostate, MTLToken, BinString>& _val);

        enum TokenIndex : size_t {
            TokenIndex_Unknown,
            TokenIndex_Keyword,
            TokenIndex_String
        };

        class DAS2_API UnserializerMtl : public cvar::IPlainTextUnserializer<Material> {
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


        class DAS2_API Unserializer : public cvar::IPlainTextUnserializer<Object>  {
            private:
                Token<KeywordToken> m_token;
                uint32_t m_uLineCounter = 1;
                bool m_bTokenRead = false;  // internal state

                const std::unordered_map<BinString, KeywordToken> m_cObjKeywords = {
                    { "#", KeywordToken::Comment },
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
                std::optional<KeywordToken> _TokenizeKeyword();
                bool _NextToken();
                void _SkipLine();
                void _SkipToNextKeyword();
                void _ReadFace();
                void _ReadSmoothing();
                void _ReadGroup();
                void _ReadMaterialName();
                void _ReadMaterialLibrary();
                void _ParseObj();

                template <typename T>
                bool _TryValueTokenization(const std::optional<T>& _token) {
                    if (_token.has_value()) {
                        m_token.token = _token.value();
                        m_token.uLine = m_uLineCounter;
                        return true;
                    }

                    return false;
                }

                // _uMin: minimum required elements to fill
                template <typename T>
                T _ReadVector(size_t _uMin, size_t _uN) {
                    T vec;
                    uint32_t uLastLine = 1;

                    size_t i = 0;
                    for (i = 0; i < _uN; i++) {
                        uLastLine = m_token.uLine;
                        if (!_NextToken()) {
                            break;
                        }

                        if (m_token.token.index() != TokenIndex_String && i < _uMin) {
                            std::stringstream ss;
                            ss << "Wavefront obj: Invalid token '" << m_token.token << "' at line " << m_token.uLine;
                            throw cvar::SyntaxErrorException(ss.str());
                        } else if (m_token.token.index() != TokenIndex_String && i >= _uMin) {
                            return vec;
                        }

                        vec[i] = static_cast<decltype(vec.first)>(std::stof(std::get<TokenIndex_String>(m_token.token).CString()));
                    }

                    if (i < _uMin) {
                        std::stringstream ss;
                        ss << "Wavefront obj: Unexpected end of file at line " << m_token.uLine;
                        throw cvar::UnexpectedEOFException(ss.str());
                    } else if (i == _uN) {
                        _NextToken();
                    }
                    
                    return vec;
                }

            public:
                Unserializer(std::istream& _stream);
        };
    }
}
