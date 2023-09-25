// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.h - das2 unserializer class header file
// author: Karl-Mihkel Ott

#pragma once

#include <istream>

#include <das2/Api.h>
#include <das2/DasStructures.h>

namespace das2 {
    class DAS2_API Unserializer {
        private:
            std::istream& m_stream;
            Model m_model;

        private:
            void _ReadCompressed();
            void _ReadUncompressed(std::istream& _stream);

        public:
            Unserializer(std::istream& _stream);
            void Unserialize();
            inline Model&& Get() {
                return std::move(m_model);
            }
    };
}