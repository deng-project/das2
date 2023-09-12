// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasConverter.h - header file for obj file convertion class
// author: Karl-Mihkel Ott

#pragma once

#include <das2/Api.h>
#include <das2/converters/obj/Data.h>
#include <das2/DasStructures.h>


namespace das2 {
    namespace obj {

        class DAS2_API IConverter {
            protected:
                Header m_header;

            public:
                IConverter(const BinString& _szAuthorName = "", const BinString& _szComment = "", uint8_t _bZlibLevel = 0);
        };

        class DAS2_API DasConverter : IConverter {
            private:
                const Object& m_obj;

            public:
                DasConverter(const Object& _obj);
        };

    }
}