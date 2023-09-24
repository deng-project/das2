// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.cpp - implementation file for Wavefront OBJ unserializer class
// author: Karl-Mihkel Ott

#include <string>
#include <iostream>
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
                    _stream << std::get<TokenIndex_String>(_val).CString();
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
                    _stream << std::get<TokenIndex_String>(_val).CString();
                    break;

                default:
                    break;
            }

            return _stream;
        }


        Unserializer::Unserializer(std::istream& _stream) :
            cvar::IPlainTextUnserializer<Object>(_stream) 
        {
            _ParseObj();
        }


        std::optional<KeywordToken> Unserializer::_TokenizeKeyword(const BinString& _str) {
            if (m_cObjKeywords.find(_str) != m_cObjKeywords.end())
                return m_cObjKeywords.find(_str)->second;

            return std::nullopt;
        }


        bool Unserializer::_NextToken() {
            m_token.token = std::monostate{};
            m_token.uLine = m_uLineCounter;

            if (m_stream.peek() == -1)
                return false;

            // skip whitespaces
            while (m_stream.peek() != -1 && _Contains(m_stream.peek(), " \t\n\r", 4)) {
                if (static_cast<char>(m_stream.peek()) == '\n')
                    m_uLineCounter++;
                static_cast<void>(m_stream.get());
            }

            if (m_stream.peek() == -1)
                return false;

            // try to extract a word
            std::string sWord;
            while (m_stream.peek() != -1 && !_Contains(m_stream.peek(), " \t\n\r", 4)) {
                sWord += static_cast<char>(m_stream.get());
            }

            if (_TryValueTokenization(_TokenizeKeyword(sWord)) || _TryValueTokenization<BinString>(sWord)) {
                return true;
            }

            std::stringstream ss;
            ss << "Wavefront obj: Unexpected symbol '" << static_cast<char>(m_stream.peek()) << "'at line " << m_uLineCounter;
            throw cvar::SyntaxErrorException(ss.str());
            return false;
        }


        void Unserializer::_SkipLine() {
            while (m_stream.peek() != -1 && m_stream.peek() != '\n') {
                static_cast<void>(m_stream.get());
            }

            m_bTokenRead = _NextToken();
        }


        void Unserializer::_SkipToNextKeyword() {
            while ((m_bTokenRead = _NextToken())) {
                if (m_token.token.index() == TokenIndex_Keyword)
                    break;
            }
        }


        void Unserializer::_ReadFace() {
            size_t i = 0;
            while ((m_bTokenRead = _NextToken())) {
                if (m_token.token.index() != TokenIndex_String)
                    break;

                m_root.groups.back().elements.faces.back().emplace_back(-1, -1, -1);
                auto& face = m_root.groups.back().elements.faces.back().back();
                
                std::string stdStrFace = std::get<BinString>(m_token.token).CString();
                size_t delims[2] = { static_cast<size_t>(-1), static_cast<size_t>(-1) };

                delims[0] = stdStrFace.find('/', 0);

                if (delims[0] != std::string::npos)
                    delims[1] = stdStrFace.find('/', delims[0]+1);
                
                // extract indices
                if (delims[0] != std::string::npos) {
                    face.x = std::stoi(stdStrFace.substr(0, delims[0]))-1;
                    if (delims[1] != std::string::npos) {
                        if (delims[1] - delims[0] > 1)
                            face.y = std::stoi(stdStrFace.substr(delims[0]+1, delims[1] - delims[0] - 1))-1;
                        if (stdStrFace.size() - delims[1] > 1)
                            face.z = std::stoi(stdStrFace.substr(delims[1]+1))-1;
                    }
                    else if (stdStrFace.size() - delims[0] > 1) {
                        face.y = std::stoi(stdStrFace.substr(delims[0]+1))-1;
                    }
                } else {
                    face.x = std::stoi(stdStrFace)-1;
                }

                i++;
            }

            // at least three vertices are needed for a face
            if (i < 3) {
                std::stringstream ss;
                ss << "Wavefront obj: At least 3 vertices are needed to construct a face at line " << m_token.uLine;
                throw cvar::SyntaxErrorException(ss.str());
            }
        }


        void Unserializer::_ReadSmoothing() {
            m_bTokenRead = _NextToken();

            if (!m_bTokenRead) 
                throw cvar::UnexpectedEOFException("No more tokens to validate Wavefront obj syntax");
            
            if (m_token.token.index() != TokenIndex_String) {
                std::stringstream ss;
                ss << "Wavefront obj: Invalid identifier '" << m_token.token << "' at line " << m_token.uLine;
                throw cvar::SyntaxErrorException(ss.str());
            }

            if (std::get<TokenIndex_String>(m_token.token) != "off" && std::stoi(std::get<TokenIndex_String>(m_token.token).CString()) > 0)
                m_root.groups.back().bSmoothing = true;

            m_bTokenRead = _NextToken();
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
                throw cvar::UnexpectedEOFException(ss.str());
            }

            if (m_token.token.index() != TokenIndex_String) {
                std::stringstream ss;
                ss << "Wavefront obj: Unexpected token '" << m_token.token << "' at line " << m_token.uLine;
                throw cvar::UnexpectedEOFException(ss.str());
            }

            m_root.groups.back().szMaterialName = std::get<TokenIndex_String>(m_token.token);

            m_bTokenRead = _NextToken();
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
                    ss << "Wavefront obj: Unexpected token '" << m_token.token << "' at line " << m_token.uLine;
                    throw cvar::SyntaxErrorException(ss.str());
                }

                KeywordToken kwToken = std::get<KeywordToken>(m_token.token);
                switch (kwToken) {
                    case KeywordToken::Comment:
                        _SkipLine();
                        break;

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
                        if (m_root.groups.size() && !m_root.groups.back().elements.faces.empty())
                            m_root.groups.emplace_back();
                        _ReadGroup();
                        break;

                    case KeywordToken::SmoothingGroup:
                        if (m_root.groups.size() && !m_root.groups.back().elements.faces.empty())
                            m_root.groups.emplace_back();
                        _ReadSmoothing();                        
                        break;

                    case KeywordToken::MaterialName:
                        if (m_root.groups.size() && !m_root.groups.back().elements.faces.empty())
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
