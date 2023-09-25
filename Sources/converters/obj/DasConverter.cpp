// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasConverter.h - implementation file for obj file convertion class
// author: Karl-Mihkel Ott

#include <queue>
#include <unordered_set>
#include <das2/converters/obj/DasConverter.h>
#include <das2/Exceptions.h>

#define PI 3.14159f

namespace das2 {
    namespace obj {

        DasConverter::DasConverter(const Object& _obj, const BinString& _szAuthorName, const BinString& _szComment, uint8_t _bZLibLevel) :
            IConverter(_szAuthorName, _szComment, _bZLibLevel),
            m_obj(_obj)
        {
            _CreateModel();
        }


        void DasConverter::_TriangulizeFace(const std::vector<TRS::Point3D<uint32_t>>& _face) {
            std::deque<TRS::Point3D<uint32_t>> qFaces;
            // add vertices to queue
            for (auto it = _face.begin(); it != _face.end(); it++)
                qFaces.push_back(*it);

            // initially pick 3 vertices 1, 2, 3
            // remove vertex 2 from the set and recursively perform the same algorithm until the amount of vertices divides by 3
            size_t uRemainingVertices = _face.size();
            while (!qFaces.empty()) {
                m_triangulizedFaces.emplace_back();
                m_triangulizedFaces.back()[0] = qFaces.front();
                qFaces.pop_front();
                m_triangulizedFaces.back()[1] = qFaces.front();
                qFaces.pop_back();
                m_triangulizedFaces.back()[2] = qFaces.front();
                qFaces.pop_back();
                
                if (!qFaces.empty()) {
                    qFaces.push_front(m_triangulizedFaces.back()[2]);
                    qFaces.push_back(m_triangulizedFaces.back()[0]);
                }
            }
        }


        void DasConverter::_SmoothenNormals(size_t _uTriangulizationOffset) {
            // key: vertex position index
            // value vector: all normal ids
            std::unordered_map<uint32_t, std::vector<uint32_t>> matchingVertexMap;
            for (auto faceIt = m_triangulizedFaces.begin() + _uTriangulizationOffset; faceIt != m_triangulizedFaces.end(); faceIt++) {
                for (auto vIt = faceIt->begin(); vIt != faceIt->end(); vIt++) {
                    matchingVertexMap[vIt->x].push_back(vIt->z);
                }
            }

            // substitude normals
            for (auto mapIt = matchingVertexMap.begin(); mapIt != matchingVertexMap.end(); mapIt++) {
                TRS::Vector3<float> meanNormal;
                for (auto matchIt = mapIt->second.begin(); matchIt != mapIt->second.end(); matchIt++) {
                    if (*matchIt != static_cast<uint32_t>(-1))
                        meanNormal += m_obj.vertices.vertexNormals[*matchIt];
                    else {
                        throw das2::ConvertionLogicException("Wavefront obj: Cannot use smooth shading for faces that have no normals");
                    }
                }
                meanNormal.Normalise();

                for (auto matchIt = mapIt->second.begin(); matchIt != mapIt->second.end(); matchIt++) {
                    m_obj.vertices.vertexNormals[*matchIt] = meanNormal;
                }
            }
        }


        void DasConverter::_GenerateVertexNormals(const std::pair<size_t, size_t>& _draw) {
            m_reindexedNormals.reserve(m_reindexedNormals.size() + _draw.second * 3);

            for (size_t i = _draw.first; i < _draw.second; i++) {
                TRS::Vector3<float> v1= {
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][0].x][0],
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][0].x][1],
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][0].x][2],
                };

                TRS::Vector3<float> v2 = {
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][1].x][0],
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][1].x][1],
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][1].x][2],
                };

                TRS::Vector3<float> v3 = {
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][2].x][0],
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][2].x][1],
                    m_obj.vertices.geometricVertices[m_triangulizedFaces[i][2].x][2],
                };

                auto normal = TRS::Vector3<float>::Cross(v2 - v1, v3 - v1);
                normal.Normalise();
                m_reindexedNormals.emplace_back(normal);
                m_reindexedNormals.emplace_back(normal);
                m_reindexedNormals.emplace_back(normal);
            }
        }


        void DasConverter::_OmitIndices(const std::pair<size_t, size_t>& _draw) {
            for (size_t i = _draw.first; i < _draw.second; i++) {
                for (auto vIt = m_triangulizedFaces[i].begin(); vIt != m_triangulizedFaces[i].end(); vIt++) {
                    if (vIt->x != -1) {
                        TRS::Vector3<float> v = {
                            m_obj.vertices.geometricVertices[vIt->x][0],
                            m_obj.vertices.geometricVertices[vIt->x][1],
                            m_obj.vertices.geometricVertices[vIt->x][2],
                        };
                        m_reindexedVertexPositions.emplace_back(v);
                    }

                    if (vIt->y != -1) {
                        TRS::Vector2<float> v = {
                            m_obj.vertices.textureVertices[vIt->y][0],
                            m_obj.vertices.textureVertices[vIt->y][1],
                        };

                        m_reindexedUVPositions.emplace_back(v);
                    }

                    if (vIt->z != -1)
                        m_reindexedNormals.emplace_back(m_obj.vertices.vertexNormals[vIt->z]);
                }
            }
        }


        void DasConverter::_Convert() {
            // first: triangulizationOffset
            // second: draw count
            std::vector<std::pair<size_t, size_t>> twoAttrGroups;

            // for each group
            for (auto groupIt = m_obj.groups.begin(); groupIt != m_obj.groups.end(); groupIt++) {
                // for each face try to triangulize it
                size_t uTriangulizationOffset = m_triangulizedFaces.size();
                for (auto faceIt = groupIt->elements.faces.begin(); faceIt != groupIt->elements.faces.end(); faceIt++) {
                    _TriangulizeFace(*faceIt);
                }

                auto& firstVertex = groupIt->elements.faces.front().front();
                // check if smooth shading is enabled 
                if (firstVertex.z == -1 && groupIt->bSmoothing) {
                    _SmoothenNormals(uTriangulizationOffset);
                }

                // check if there are only position vertices
                // if so then attempt to reindex them
                if (firstVertex.x != -1 && firstVertex.y == -1) {
                    twoAttrGroups.emplace_back(std::make_pair(uTriangulizationOffset, m_triangulizedFaces.size() - uTriangulizationOffset));
                }
                else if (firstVertex.x != -1) {
                    m_model.meshes.emplace_back();
                    m_model.meshes.back().Initialize();
                    m_model.meshes.back().uDrawCount = static_cast<uint32_t>((m_triangulizedFaces.size() - uTriangulizationOffset) * 3);
                    m_model.meshes.back().uPositionVertexBufferOffset = static_cast<uint32_t>(m_reindexedVertexPositions.size() * sizeof(TRS::Vector3<float>));
                    
                    for (auto uvIt = m_model.meshes.back().arrUVBufferOffsets.begin(); uvIt != m_model.meshes.back().arrUVBufferOffsets.end(); uvIt++)
                        *uvIt = static_cast<uint32_t>(m_reindexedUVPositions.size() * sizeof(TRS::Vector2<float>));
                    
                    m_model.meshes.back().uVertexNormalBufferOffset = static_cast<uint32_t>(m_reindexedNormals.size() * sizeof(TRS::Vector3<float>));
                    _OmitIndices(std::make_pair(uTriangulizationOffset, m_triangulizedFaces.size() - uTriangulizationOffset));
                }
            }

            // for each two attribute group
            for (auto groupIt = twoAttrGroups.begin(); groupIt != twoAttrGroups.end(); groupIt++) {
                auto& firstVertex = m_triangulizedFaces[groupIt->first].front();

                m_model.meshes.emplace_back();
                m_model.meshes.back().uPositionVertexBufferOffset = m_reindexedVertexPositions.size();
                m_model.meshes.back().uPositionVertexBufferOffset = m_reindexedNormals.size();
                m_model.meshes.back().uDrawCount = static_cast<uint32_t>(groupIt->second);

                // vertex normals are missing
                if (firstVertex.z == -1) {
                    _GenerateVertexNormals(*groupIt);
                }

                _OmitIndices(*groupIt);
            }

            // set vertex offsets correctly
            m_model.buffer.Initialize();
            uint32_t uVertexBufferOffset = m_model.buffer.PushRange(m_reindexedVertexPositions.begin(), m_reindexedVertexPositions.end());
            uint32_t uUVBufferOffset = m_model.buffer.PushRange(m_reindexedUVPositions.begin(), m_reindexedUVPositions.end());
            uint32_t uVertexNormalBufferOffset = m_model.buffer.PushRange(m_reindexedNormals.begin(), m_reindexedNormals.end());
            uint32_t uIndexBufferOffset = m_model.buffer.PushRange(m_indices.begin(), m_indices.end());

            for (auto it = m_model.meshes.begin(); it != m_model.meshes.end(); it++) {
                it->uPositionVertexBufferOffset += uVertexBufferOffset;

                if (m_obj.vertices.textureVertices.size()) {
                    for (size_t i = 0; i < it->arrUVBufferOffsets.size(); i++) {
                        it->arrUVBufferOffsets[i] += uUVBufferOffset;
                    }
                }

                if (m_obj.vertices.vertexNormals.size()) {
                    it->uVertexNormalBufferOffset += uVertexNormalBufferOffset;
                }
            }
        }

        void DasConverter::_CreateModel() {
            m_model.meshGroups.resize(m_obj.groups.size());
            m_model.nodes.resize(m_obj.groups.size());

            for (size_t i = 0; i < m_obj.groups.size(); i++) {
                std::string sConcatName;
                for (auto it = m_obj.groups[i].groupNames.begin(); it != m_obj.groups[i].groupNames.end(); it++) {
                    if (it != m_obj.groups[i].groupNames.begin())
                        sConcatName += '_';
                    sConcatName += it->CString();
                }

                m_model.meshGroups[i].Initialize();
                m_model.meshGroups[i].szName = sConcatName;
                m_model.meshGroups[i].meshes.emplace_back(static_cast<uint32_t>(i));
                m_model.nodes[i].szName = sConcatName;
                m_model.nodes[i].uMeshGroupId = i;
            }

            _Convert();

            m_model.nodes.resize(m_model.meshGroups.size());
            for (size_t i = 0; i < m_model.nodes.size(); i++) {
                m_model.nodes[i].Initialize();
                m_model.nodes.back().uMeshGroupId = static_cast<uint32_t>(i);
            }

            m_model.scenes.emplace_back();
            m_model.scenes.back().Initialize();

            m_model.scenes.back().rootNodes.reserve(m_model.nodes.size());
            for (size_t i = 0; i < m_model.nodes.size(); i++)
                m_model.scenes.back().rootNodes.push_back(static_cast<uint32_t>(i));
        }
    }
}