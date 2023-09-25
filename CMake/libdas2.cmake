# das2: Improved DENG asset manager library
# licence: Apache, see LICENCE file
# file: libdas2.cmake - das2 library CMake configuration file
# author: Karl-Mihkel Ott

set(DAS2_TARGET das2)
set(DAS2_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/Api.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/DasStructures.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/Serializer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/Unserializer.h)
set(DAS2_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/DasStructures.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Serializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Unserializer.cpp)

# Third-party format converter support
if (DAS2_WAVEFRONT_OBJ)
	set (WAVEFRONT_OBJ_HEADERS
		${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/converters/obj/DasConverter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/Exceptions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/converters/obj/Data.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/converters/obj/Unserializer.h)
	set (WAVEFRONT_OBJ_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/converters/obj/DasConverter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources/converters/obj/Unserializer.cpp)
		
    list(APPEND DAS2_HEADERS ${WAVEFRONT_OBJ_HEADERS})
    list(APPEND DAS2_SOURCES ${WAVEFRONT_OBJ_SOURCES})
	
	source_group("WavefrontObj" FILES ${WAVEFRONT_OBJ_HEADERS} ${WAVEFRONT_OBJ_SOURCES})
endif()

if (NOT DAS2_BUILD_STATIC)
    add_library(${DAS2_TARGET} SHARED
        ${DAS2_HEADERS}
        ${DAS2_SOURCES})
	target_compile_definitions(${DAS2_TARGET}
		PRIVATE DAS2_EXPORT_LIBRARY)
else()
    add_library(${DAS2_TARGET} STATIC
        ${DAS2_HEADERS}
        ${DAS2_SOURCES})
	target_compile_definitions(${DAS2_TARGET}
		PUBLIC DAS2_BUILD_STATIC)
endif()

add_dependencies(${DAS2_TARGET} cvar)

find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(zstd CONFIG REQUIRED)
message(STATUS "")

if (WIN32 AND MSVC)
	target_compile_definitions(${DAS2_TARGET}
		PRIVATE DAS2_EXPORT_LIBRARY)
endif()

target_link_libraries(${DAS2_TARGET}
    PUBLIC cvar
    PRIVATE Boost::boost 
    PRIVATE Boost::iostreams
    PRIVATE $<IF:$<TARGET_EXISTS:zstd::libzstd_shared>,zstd::libzstd_shared,zstd::libzstd_static>)

target_include_directories(${DAS2_TARGET}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/trs-headers/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cvar/Include)
