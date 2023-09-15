# das2: Improved DENG asset manager library
# licence: Apache, see LICENCE file
# file: ConvertWavefrontObj.cmake - Wavefront obj convertion demo program build configuration
# author: Karl-Mihkel Ott

set(CONVERT_WAVEFRONT_OBJ_TARGET ConvertWavefrontObj)
set(CONVERT_WAVEFRONT_OBJ_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/converters/obj/demos/ConvertWavefrontObj.h)
set(CONVERT_WAVEFRONT_OBJ_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/converters/obj/demos/ConvertWavefrontObj.cpp)

add_executable(${CONVERT_WAVEFRONT_OBJ_TARGET}
    ${CONVERT_WAVEFRONT_OBJ_HEADERS}
    ${CONVERT_WAVEFRONT_OBJ_SOURCES})

add_dependencies(${CONVERT_WAVEFRONT_OBJ_TARGET}
    ${DAS2_TARGET})

target_link_libraries(${CONVERT_WAVEFRONT_OBJ_TARGET}
    PUBLIC ${DAS2_TARGET})