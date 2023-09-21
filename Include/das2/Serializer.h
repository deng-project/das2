// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Serializer.h - das2 serializer class header file
// author: Karl-Mihkel Ott

#pragma once 

#include <das2/Api.h>
#include <das2/DasStructures.h>
#include <ostream>

namespace das2 {

    class DAS2_API Serializer {
        private:
            std::ostream& m_stream;
            const Model& m_model;

        private:
            template <typename T>
            void _StreamUncompressedArray(const std::vector<T>& _vec, std::ostream& _stream) {
                for (auto it = _vec.begin(); it != _vec.end(); it++) {
                    m_stream << *it;
                }
            }

            void _StreamUncompressed(std::ostream& _stream);
            void _StreamCompressed();

        public:
            Serializer(std::ostream& _stream, const Model& _model) :
                m_stream(_stream),
                m_model(_model) {}
            void Serialize();
    };
}
