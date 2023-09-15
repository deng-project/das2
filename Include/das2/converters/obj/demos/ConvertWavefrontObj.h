// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: ConvertWavefrontObj.h - header of demo program to convert wavefront obj files into das2 files
// author: Karl-Mihkel Ott

#include <cstddef>
#include <trs/Iterators.h>
#include <das2/converters/obj/Unserializer.h>
#include <das2/converters/obj/DasConverter.h>
#include <das2/Serializer.h>

namespace das2 {
    namespace demo {
        class ConvertWavefrontObj {
            private:
                das2::obj::Unserializer* m_pUnserializer = nullptr;
                das2::obj::DasConverter* m_pDasConverter = nullptr;
                das2::Serializer* m_pSerializer = nullptr;

            public:
                ConvertWavefrontObj(const char* _szDestFileName, const char* _szInputFileName, uint8_t _bZlibCompression);
                ~ConvertWavefrontObj();
        };
    }
}