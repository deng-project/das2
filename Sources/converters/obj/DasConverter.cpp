// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasConverter.h - implementation file for obj file convertion class
// author: Karl-Mihkel Ott

#include <queue>
#include <unordered_set>
#include <das2/converters/obj/DasConverter.h>

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


        void DasConverter::_Convert() {
            // for each group
            for (auto groupIt = m_obj.groups.begin(); groupIt != m_obj.groups.end(); groupIt++) {
                // for each face try to triangulize it
                for (auto faceIt = groupIt->elements.faces.begin(); faceIt != groupIt->elements.faces.end(); faceIt++) {
                    _TriangulizeFace(*faceIt);
                }

                // check if smooth shading is enabled 
                if(groupIt->bSmoothing) {
                    _SmoothenNormals(*groupIt);
                } else {
                    _ReduceNormalRedundancy(*groupIt);
                }
            }
        }
    }
}