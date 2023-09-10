// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.cpp - implementation file for Wavefront OBJ unserializer class
// author: Karl-Mihkel Ott

#include <das2/converters/obj/Unserializer.h>

namespace das2 {
    namespace obj {
        
        Unserializer::Unserializer(std::istream& _stream) :
            CVar::IUnserializer<Object>(_stream) 
        {
            _ParseObj();
        }


        void Unserializer::_ParseObj() {
            
        }
    }
}
