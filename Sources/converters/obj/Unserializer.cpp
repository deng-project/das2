// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.cpp - implementation file for Wavefront OBJ unserializer class
// author: Karl-Mihkel Ott

#include <string>
#include <das2/converters/obj/Unserializer.h>

namespace das2 {
    namespace obj {

        const char* _KeywordTokenToString(KeywordToken _token) {
            const char* arrLookup[] = { 
                "v", "vt", "vn", "vp", "cstype", "deg",
                "bmat", "step", "p", "l", "f", 
                "curv", "curv2", "surf", "parm", "trim", 
                "hole", "scrv", "sp", "end", "con", "g", 
                "s", "mg", "o", "bevel", "c_interp",
                "d_interp", "lod", "usemtl", "mtllib", "shadow_obj",
                "trace_obj", "ctech", "stech"
            };

            return arrLookup[static_cast<size_t>(_token)];
        }

        const char* _MTLTokenToString(MTLToken _token) {
            const char* arrLookup[] = {
                "Ka", "Kd", "Ks", "Ns", "d", "map_Ka",
                "map_Kd", "map_Ks", "map_Ns", "map_d", "Pr",
                "Pm", "Ps", "Pc", "Pcr", "Ke",
                "aniso", "anisor", "map_Pr", "map_Pm", "map_Ps",
                "map_Ke", "norm"
            };

            return arrLookup[static_cast<size_t>(_token)];
        }

        std::ostream& operator<<(std::ostream& _stream, std::variant<std::monostate, KeywordToken, BinString>& _val) {
            switch (_val.index()) {
                case TokenIndex_Keyword:
                    _stream << _KeywordTokenToString(std::get<TokenIndex_Keyword>(_val));
                    break;

                case TokenIndex_String:
                    _stream << std::get<TokenIndex_String>(_val);
                    break;

                default:
                    break;  
            }

            return _stream;
        }

        std::ostream& operator<<(std::ostream& _stream, std::variant<std::monostate, MTLToken, BinString>& _val) {
            switch (_val.index()) {
                case TokenIndex_Keyword:
                    _stream << _MTLTokenToString(std::get<TokenIndex_Keyword>(_val));
                    break;

                case TokenIndex_String:
                    _stream << std::get<TokenIndex_String>(_val);
                    break;

                default:
                    break;
            }

            return _stream;
        }
        
        Unserializer::Unserializer(std::istream& _stream) :
            CVar::IUnserializer<Object>(_stream) 
        {
            _ParseObj();
        }


        bool Unserializer::_NextToken() {
            m_token.token = std::monostate{};
            m_token.uLine = m_uLineCounter;

            if (m_stream.peek() == -1)
                return false;

            while (m_stream.peek() != -1 && _Contains(m_stream.peek(), " \t\n\r", 4)) {
                if (static_cast<char>(m_stream.peek()) == '\n')
                    m_uLineCounter++;
                static_cast<void>(m_stream.get());
            }

            if (m_stream.peek() == -1)
                return false;

            if (_TryValueTokenization(_TokenizeKeyword()) || _TryValueTokenization(_TokenizeString<BinString>(false))) {
                return true;
            }

            std::stringstream ss;
            ss << "Wavefront obj: Unexpected symbol '" << static_cast<char>(m_stream.peek()) << "'at line " << m_uLineCounter;
            throw CVar::SyntaxErrorException(ss.str());
            return false;
        }

        void Unserializer::_SkipToNextKeyword() {
            while ((m_bTokenRead = _NextToken())) {
                if (m_token.token.index() == TokenIndex_Keyword)
                    break;
            }
        }

        void Unserializer::_ReadSmoothing() {
            m_bTokenRead = _NextToken();

            if (!m_bTokenRead) 
                throw CVar::UnexpectedEOFException("No more tokens to validate Wavefront obj syntax");
            
            if (m_token.token.index() != TokenIndex_String) {
                std::stringstream ss;
                ss << "Wavefront obj: Invalid identifier '" << m_token.token << "' at line " << m_token.uLine;
                throw CVar::SyntaxErrorException(ss.str());
            }

            if (std::get<TokenIndex_String>(m_token.token) != "off" && std::stoi(std::get<TokenIndex_String>(m_token.token).CString()) > 0)
                m_root.groups.back().bSmoothing = true;
        }

        void Unserializer::_ReadGroup() {
            while ((m_bTokenRead = _NextToken()) && m_token.token.index() == TokenIndex_String)
                m_root.groups.back().groupNames.push_back(std::get<TokenIndex_String>(m_token.token));
        }

        void Unserializer::_ReadMaterialName() {
            uint32_t uPrevLine = m_token.uLine;
            if (!(m_bTokenRead = _NextToken())) {
                std::stringstream ss;
                ss << "Wavefront obj: Unexpected end of file at line " << uPrevLine;
                throw CVar::UnexpectedEOFException(ss.str());
            }

            if (m_token.token.index() != TokenIndex_String) {
                std::stringstream ss;
                ss << "Wavefront obj: Unexpected token '" << m_token.token << "' at line " << m_token.uLine;
                throw CVar::UnexpectedEOFException(ss.str());
            }

            m_root.groups.back().szMaterialName = std::get<TokenIndex_String>(m_token.token);
        }

        void Unserializer::_ReadMaterialLibrary() {
            while ((m_bTokenRead = _NextToken())) {
                if (m_token.token.index() != TokenIndex_String) {
                    break;
                }

                m_root.materials.emplace(std::get<TokenIndex_String>(m_token.token), Material{});
            }
        }

        void Unserializer::_ParseObj() {
            m_bTokenRead = _NextToken();
            m_root.groups.emplace_back();

            while (m_bTokenRead) {
                if (m_token.token.index() != TokenIndex_Keyword) {
                    std::stringstream ss;
                    ss << "Wavefront obj: Unexpected token '" << m_token.token << "'";
                    CVar::SyntaxErrorException(ss.str());
                }

                KeywordToken kwToken = std::get<KeywordToken>(m_token.token);
                switch (kwToken) {
                    case KeywordToken::GeometryVertex:
                        m_root.vertices.geometricVertices.push_back(_ReadVector<TRS::Vector4<float>>(3, 4));
                        break;

                    case KeywordToken::TextureVertex:
                        m_root.vertices.textureVertices.push_back(_ReadVector<TRS::Vector3<float>>(1, 3));
                        break;

                    case KeywordToken::VertexNormal:
                        m_root.vertices.vertexNormals.push_back(_ReadVector<TRS::Vector3<float>>(3, 3));
                        break;

                    case KeywordToken::Face:
                        m_root.groups.back().elements.faces.emplace_back();
                        _ReadFace();
                        break;

                    case KeywordToken::GroupName:
                    case KeywordToken::ObjectName:
                        m_root.groups.emplace_back();
                        _ReadGroup();
                        break;

                    case KeywordToken::SmoothingGroup:
                        m_root.groups.emplace_back();
                        _ReadSmoothing();                        
                        break;

                    case KeywordToken::MaterialName:
                        m_root.groups.emplace_back();
                        _ReadMaterialName();
                        break;

                    case KeywordToken::MaterialLibrary:
                        _ReadMaterialLibrary();
                        break;

                    default:
                        _SkipToNextKeyword();
                        break;
                }
            }
        }
    }
}
