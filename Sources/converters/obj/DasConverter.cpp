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
            _Convert();
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


        void DasConverter::_SmoothenNormals(const Group& _group) {
            // key: vertex position index
            // value vector: all normal ids
            std::unordered_map<uint32_t, std::pair<std::vector<uint32_t>, uint32_t>> matchingVertexMap;

            for (auto faceIt = _group.elements.faces.begin(); faceIt != _group.elements.faces.end(); faceIt++) {
                for (auto vIt = faceIt->begin(); vIt != faceIt->end(); vIt++) {
                    matchingVertexMap[vIt->x].first.push_back(vIt->z);
                }
            }

            // substitude normals
            for (auto mapIt = matchingVertexMap.begin(); mapIt != matchingVertexMap.end(); mapIt++) {
                TRS::Vector3<float> meanNormal;
                for (auto matchIt = mapIt->second.first.begin(); matchIt != mapIt->second.first.end(); matchIt++) {
                    if (*matchIt != static_cast<uint32_t>(-1))
                        meanNormal += m_obj.vertices.vertexNormals[*matchIt];
                    else {
                        throw das2::ConvertionLogicException("Wavefront obj: Cannot use smooth shading for faces that have no normals");
                    }
                }
                meanNormal.Normalise();

                // check if meanNormal is present in maps
                if (m_mapNormals.find(meanNormal) != m_mapNormals.end())
                    mapIt->second.second = m_mapNormals[meanNormal];
                else {
                    uint32_t uNormalId = static_cast<uint32_t>(m_smoothenedNormals.size());
                    m_smoothenedNormals.push_back(meanNormal);
                    m_mapNormals[meanNormal] = uNormalId;
                    mapIt->second.second = uNormalId;
                }
            }

            // replace vertex normals
            for (auto it = m_triangulizedFaces.begin(); it != m_triangulizedFaces.end(); it++) {
                for (auto vIt = it->begin(); vIt != it->end(); vIt++) {
                    if (matchingVertexMap.find(vIt->x) != matchingVertexMap.end())
                        vIt->z = matchingVertexMap[vIt->z].second;
                }
            }
        }


        void DasConverter::_ReduceNormalRedundancy(const Group& _group) {
            // for each face in group
            for (auto faceIt = m_triangulizedFaces.begin(); faceIt != m_triangulizedFaces.end(); faceIt++) {
                // for each vertex in face
                for (auto vIt = faceIt->begin(); vIt != faceIt->end(); vIt++) {
                    if (vIt->z != static_cast<uint32_t>(-1)) {
                        if (m_mapNormals.find(m_obj.vertices.vertexNormals[vIt->z]) != m_mapNormals.end()) {
                            vIt->z = m_mapNormals[m_obj.vertices.vertexNormals[vIt->z]];
                        } else {
                            uint32_t uNormalId = static_cast<uint32_t>(m_smoothenedNormals.size());
                            m_smoothenedNormals.push_back(m_obj.vertices.vertexNormals[vIt->z]);
                            vIt->z = uNormalId;
                        }
                    }
                }
            }
        }


        void DasConverter::_Reindex() {
            uint32_t uMaxIndex = 0;
            uint8_t bNoVerticesMask = 0;

            for (auto faceIt = m_triangulizedFaces.begin(); faceIt != m_triangulizedFaces.end(); faceIt++) {
                for (auto vIt = faceIt->begin(); vIt != faceIt->end(); vIt++) {
                    UnifiedVertex unifiedVertex = {};
                    if (vIt->x != static_cast<uint32_t>(-1)) {
                        unifiedVertex.positionVertex.first = m_obj.vertices.geometricVertices[vIt->x].first;
                        unifiedVertex.positionVertex.second = m_obj.vertices.geometricVertices[vIt->x].second;
                        unifiedVertex.positionVertex.third = m_obj.vertices.geometricVertices[vIt->x].third;
                    }
                    else {
                        throw ConvertionLogicException("Wavefront obj: Mesh must have psoition vertex present in its faces");
                    }

                    if (vIt->y != static_cast<uint32_t>(-1)) {
                        if (bNoVerticesMask & (1 << 1))
                            throw ConvertionLogicException("Wavefront obj: Mesh must have uniform amount of vertex attributes accross all faces!");
                        
                        unifiedVertex.textureVertex.first = m_obj.vertices.textureVertices[vIt->y].first;
                        unifiedVertex.textureVertex.second = m_obj.vertices.textureVertices[vIt->y].second;
                    }
                    else if (!m_obj.vertices.textureVertices.empty())
                        throw ConvertionLogicException("Wavefront obj: Mesh must have uniform amount of vertex attributes accross all faces!");
                    else
                        bNoVerticesMask |= (1 << 1);

                    if (vIt->z != static_cast<uint32_t>(-1)) {
                        if (bNoVerticesMask & (1 << 2))
                            throw ConvertionLogicException("Wavefront obj: Mesh must have uniform amount of vertex attributes accross all faces!");
                        unifiedVertex.normalVertex.first = m_smoothenedNormals[vIt->z].first;
                        unifiedVertex.normalVertex.second = m_smoothenedNormals[vIt->z].second;
                        unifiedVertex.normalVertex.third = m_smoothenedNormals[vIt->z].third;
                    }
                    else if (!m_obj.vertices.vertexNormals.empty())
                        throw ConvertionLogicException("Wavefront obj: Mesh must have uniform amount of vertex attributes accross all faces!");
                    else bNoVerticesMask |= (1 << 2);

                    // check if vertex exists in map
                    if (m_reindexMap.find(unifiedVertex) != m_reindexMap.end()) {
                        if (vIt->x != static_cast<uint32_t>(-1))
                            m_reindexedVertexPositions.push_back(unifiedVertex.positionVertex);
                        if (vIt->y != static_cast<uint32_t>(-1))
                            m_reindexedUVPositions.push_back(unifiedVertex.textureVertex);
                        if (vIt->z != static_cast<uint32_t>(-1))
                            m_reindexedVertexPositions.push_back(unifiedVertex.positionVertex);

                        m_indices.push_back(uMaxIndex++);
                        m_reindexMap[unifiedVertex] = m_indices.back();
                    } else {
                        m_indices.push_back(m_reindexMap[unifiedVertex]);
                    }
                }
            }
        }


        void DasConverter::_Convert() {
            m_model.meshGroups.emplace_back();
            m_model.meshGroups.back().bStructure = StructureIdentifier_Buffer;
            m_model.meshGroups.back().uMeshCount = m_obj.groups.size();
            m_model.meshGroups.back().szName = "Wavefront obj mesh";
            m_model.meshGroups.back().pMeshes = new uint32_t[m_model.meshGroups.back().uMeshCount];

            for (uint32_t i = 0; i < m_model.meshGroups.back().uMeshCount; i++)
                m_model.meshGroups.back().pMeshes[i] = i;

            // for each group
            for (auto groupIt = m_obj.groups.begin(); groupIt != m_obj.groups.end(); groupIt++) {
                m_model.meshes.emplace_back();
                m_model.meshes.back().bStructure = StructureIdentifier_Mesh;
                m_model.meshes.back().bMaterialType = MaterialType_Unknown;
                m_model.meshes.back().uMaterialId = -1;

                if (m_obj.vertices.geometricVertices.size()) {
                    m_model.meshes.back().uPositionVertexBufferId = 0;
                    m_model.meshes.back().uPositionVertexBufferOffset = static_cast<uint32_t>(m_reindexedVertexPositions.size() * sizeof(TRS::Vector3<float>));
                }

                // for each face try to triangulize it
                for (auto faceIt = groupIt->elements.faces.begin(); faceIt != groupIt->elements.faces.end(); faceIt++) {
                    _TriangulizeFace(*faceIt);
                }

                // check if smooth shading is enabled 
                if (groupIt->bSmoothing) {
                    _SmoothenNormals(*groupIt);
                } else {
                    _ReduceNormalRedundancy(*groupIt);
                }

                size_t uPrevCount = m_indices.size();
                _Reindex();

                m_model.meshes.back().uDrawCount = static_cast<uint32_t>(m_indices.size() - uPrevCount);
            }

            // set vertex offsets correctly
            for (auto it = m_model.meshes.begin(); it != m_model.meshes.end(); it++) {
                it->uPositionVertexBufferId = 0;
                it->uPositionVertexBufferOffset = 0;

                if (m_obj.vertices.textureVertices.size()) {
                    for (size_t i = 0; i < 8; i++) {
                        it->arrUVBufferIds[i] = 0;
                        it->arrUVBufferOffsets[i] = static_cast<uint32_t>(m_reindexedVertexPositions.size() * sizeof(TRS::Vector2<float>));
                    }
                }

                if (m_obj.vertices.vertexNormals.size()) {
                    it->uSurfaceNormalBufferId = 0;
                    it->uSurfaceNormalBufferOffset += static_cast<uint32_t>(m_reindexedVertexPositions.size() * sizeof(TRS::Vector3<float>) + 
                                                                            m_reindexedUVPositions.size() * sizeof(TRS::Vector2<float>));
                }
            }


            m_model.buffer.bStructure = StructureIdentifier_Buffer;
            m_model.buffer.uLength = static_cast<uint32_t>(m_reindexedVertexPositions.size() * sizeof(TRS::Vector3<float>) + 
                                                           m_reindexedUVPositions.size() * sizeof(TRS::Vector2<float>) + 
                                                           m_reindexedUVPositions.size() * sizeof(TRS::Vector3<float>) +
                                                           m_indices.size() * sizeof(uint32_t));
            m_model.buffer.pData = new char[m_model.buffer.uLength]{};

            std::memcpy(m_model.buffer.pData, m_reindexedVertexPositions.data(), m_reindexedVertexPositions.size() * sizeof(TRS::Vector3<float>));
            size_t uOffset = m_reindexedVertexPositions.size() * sizeof(TRS::Vector3<float>);

            m_reindexedVertexPositions = std::vector<TRS::Vector3<float>>();

            if (m_reindexedUVPositions.size()) {
                std::memcpy(m_model.buffer.pData + uOffset, m_reindexedUVPositions.data(), m_reindexedUVPositions.size() * sizeof(TRS::Vector2<float>));
                uOffset += m_reindexedUVPositions.size() * sizeof(TRS::Vector2<float>);
            }
            m_reindexedUVPositions = std::vector<TRS::Vector2<float>>();

            if (m_reindexedNormals.size()) {
                std::memcpy(m_model.buffer.pData + uOffset, m_reindexedNormals.data(), m_reindexedNormals.size() * sizeof(TRS::Vector3<float>));
                uOffset += m_reindexedNormals.size() * sizeof(TRS::Vector3<float>);
            }
            m_reindexedNormals = std::vector<TRS::Vector3<float>>();

            // copy indices
            if (m_indices.size()) {
                std::memcpy(m_model.buffer.pData + uOffset, m_indices.data(), m_indices.size() * sizeof(uint32_t));
            }
            m_indices = std::vector<uint32_t>();
        }
    }
}