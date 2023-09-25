// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.h - das2 unserializer class implementation file
// author: Karl-Mihkel Ott

#include <das2/Unserializer.h>

namespace das2 {

    Unserializer::Unserializer(std::istream& _stream) :
        m_stream(_stream) {}


    void Unserializer::Unserialize() {
        // read header
        
    }
}