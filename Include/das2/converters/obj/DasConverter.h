// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasConverter.h - header file for obj file convertion class
// author: Karl-Mihkel Ott

#pragma once

#include <das2/Api.h>
#include <das2/converters/obj/Data.h>
#include <das2/DasStructures.h>
#include <das2/IConverter.h>

#include <cstdint>
#include <unordered_set>
#include <trs/Vector.h>

namespace std {
    template <>
    struct hash<TRS::Vector3<float>> {
        size_t operator()(TRS::Vector3<float>& _vec) {
            size_t seed = 0;
            std::hash<float> hasher;
            seed ^= hasher(_vec.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hasher(_vec.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hasher(_vec.third) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };  
}

namespace das2 {
    namespace obj {

        class DAS2_API DasConverter : public IConverter {
            private:
                const Object& m_obj;
                std::unordered_map<TRS::Vector3<float>, uint32_t> m_mapNormals;
                std::vector<TRS::Vector3<float>> m_smoothenedNormals;
                std::vector<std::array<TRS::Point3D<uint32_t>, 3>> m_triangulizedFaces;

            private:
                void _Convert();
                void _TriangulizeFace(const std::vector<TRS::Point3D<uint32_t>>& _face);
                void _SmoothenNormals(const Group& _group);
                void _ReduceNormalRedundancy(const Group& _group);

            public:
                DasConverter(const Object& _obj, const BinString& _szAuthorName = "", const BinString& _szComment = "", uint8_t _uZLibLevel = 0);
        };

    }
}