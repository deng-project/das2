# das2: Improved DENG asset manager library
# licence: Apache, see LICENCE file
# file: ParseWavefrontObj.cmake - Wavefront obj parser programm build configuration
# author: Karl-Mihkel Ott

set(PARSE_WAVEFRONT_OBJ_TARGET ParseWavefrontObj)
set(PARSE_WAVEFRONT_OBJ_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/converters/obj/ParseWavefrontObj.h)
set(PARSE_WAVEFRONT_OBJ_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/converters/obj/ParseWavefrontObj.cpp)

add_executable(${PARSE_WAVEFRONT_OBJ_TARGET} 
    ${PARSE_WAVEFRONT_OBJ_HEADERS}
    ${PARSE_WAVEFRONT_OBJ_SOURCES})

add_dependencies(${PARSE_WAVEFRONT_OBJ_TARGET}
    ${DAS2_TARGET})

target_link_libraries(${PARSE_WAVEFRONT_OBJ_TARGET}
    PUBLIC ${DAS2_TARGET})