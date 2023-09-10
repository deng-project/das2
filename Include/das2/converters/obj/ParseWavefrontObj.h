// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: ParseWavefrontObj.h - wavefront obj parser application class header
// author: Karl-Mihkel Ott

#pragma once

#include <das2/converters/obj/Unserializer.h>

namespace das2 {
    namespace demo {
        
        class ParseWavefrontObj {
            private:
                das2::obj::Unserializer *m_pUnserializer = nullptr;

            public:
                ParseWavefrontObj(const char* _szFileName);
                ~ParseWavefrontObj();
                void PrintOutput();
        };
    }
}