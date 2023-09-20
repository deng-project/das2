#pragma once

#include <ostream>
#include <das2/Api.h>
#include <das2/DasStructures.h>
#include <cvar/ISerializer.h>

namespace das2 {
    
    class DAS2_API IConverter {
        protected:
            Model m_model;

        public:
            IConverter(const BinString& _szAuthorName = "", const BinString& _szComment = "", uint8_t _bZlibLevel = 0) {
                m_model.header.Initialize();
                m_model.header.szAuthorName = _szAuthorName;
                m_model.header.szComment = _szComment;
                m_model.header.bZlibLevel = _bZlibLevel;
            }

            inline Model&& GetModel() {
                return std::move(m_model);
            }
    };
}