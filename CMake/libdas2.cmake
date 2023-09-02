# das2: Improved DENG asset manager library
# licence: Apache, see LICENCE file
# file: libdas2.cmake - das2 library CMake configuration file
# author: Karl-Mihkel Ott

set(DAS2_TARGET das2)
set(DAS2_HEADERS)
set(DAS2_SOURCES)

if (NOT DAS2_BUILD_STATIC)
    add_library(SHARED ${DAS2_TARGET}
        ${DAS2_HEADERS}
        ${DAS2_SOURCES})
else()
    add_library(STATIC ${DAS2_TARGET}
        ${DAS2_HEADERS}
        ${DAS2_SOURCES})
endif()
