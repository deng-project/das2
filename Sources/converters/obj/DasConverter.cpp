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


        void DasConverter::_Reindex(size_t _uTriangulizedOffset) {
            uint32_t uMaxIndex = 0;
            uint8_t bNoVerticesMask = 0;

            for (auto faceIt = m_triangulizedFaces.begin() + _uTriangulizedOffset; faceIt != m_triangulizedFaces.end(); faceIt++) {
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
                        unifiedVertex.normalVertex.first = m_obj.vertices.vertexNormals[vIt->z].first;
                        unifiedVertex.normalVertex.second = m_obj.vertices.vertexNormals[vIt->z].second;
                        unifiedVertex.normalVertex.third = m_obj.vertices.vertexNormals[vIt->z].third;
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
                            m_reindexedNormals.push_back(unifiedVertex.normalVertex);

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
            m_model.meshGroups.back().Initialize();
            m_model.meshGroups.back().meshes.resize(m_obj.groups.size());

            for (uint32_t i = 0; i < m_model.meshGroups.back().meshes.size(); i++)
                m_model.meshGroups.back().meshes[i] = i;

            // for each group
            for (auto groupIt = m_obj.groups.begin(); groupIt != m_obj.groups.end(); groupIt++) {
                m_model.meshes.emplace_back();
                m_model.meshes.back().Initialize();
                m_model.meshes.back().bMaterialType = MaterialType_Unknown;
                m_model.meshes.back().uMaterialId = -1;

                if (m_obj.vertices.geometricVertices.size()) {
                    m_model.meshes.back().uPositionVertexBufferOffset = static_cast<uint32_t>(m_reindexedVertexPositions.size() * sizeof(TRS::Vector3<float>));
                }

                // for each face try to triangulize it
                size_t uTriangulizationOffset = m_triangulizedFaces.size();
                for (auto faceIt = groupIt->elements.faces.begin(); faceIt != groupIt->elements.faces.end(); faceIt++) {
                    _TriangulizeFace(*faceIt);
                }

                // check if smooth shading is enabled 
                if (groupIt->bSmoothing) {
                    _SmoothenNormals(uTriangulizationOffset);
                }

                size_t uPrevCount = m_indices.size();
                
                m_model.meshes.back().uIndexBufferOffset = static_cast<uint32_t>(uPrevCount * sizeof(uint32_t));
                _Reindex(uTriangulizationOffset);
                m_model.meshes.back().uDrawCount = static_cast<uint32_t>(m_indices.size() - uPrevCount);
            }

            // set vertex offsets correctly
            m_model.buffer.Initialize();
            uint32_t uVertexBufferOffset = m_model.buffer.PushRange(m_reindexedVertexPositions.begin(), m_reindexedVertexPositions.end());
            uint32_t uUVBufferOffset = m_model.buffer.PushRange(m_reindexedUVPositions.begin(), m_reindexedUVPositions.end());
            uint32_t uVertexNormalBufferOffset = m_model.buffer.PushRange(m_reindexedNormals.begin(), m_reindexedNormals.end());
            uint32_t uIndexBufferOffset = m_model.buffer.PushRange(m_indices.begin(), m_indices.end());

            for (auto it = m_model.meshes.begin(); it != m_model.meshes.end(); it++) {
                it->uPositionVertexBufferOffset = uVertexBufferOffset;

                if (m_obj.vertices.textureVertices.size()) {
                    for (size_t i = 0; i < it->arrUVBufferOffsets.size(); i++) {
                        it->arrUVBufferOffsets[i] = uUVBufferOffset;
                    }
                }

                if (m_obj.vertices.vertexNormals.size()) {
                    it->uVertexNormalBufferOffset = uVertexNormalBufferOffset;
                }

                it->uIndexBufferOffset += uIndexBufferOffset;
            }

            m_model.nodes.emplace_back();
            m_model.nodes.back().Initialize();
            m_model.nodes.back().uMeshGroupId = 0;
        
            m_model.scenes.emplace_back();
            m_model.scenes.back().Initialize();
            m_model.scenes.back().rootNodes.push_back(0);
        }
    }
}