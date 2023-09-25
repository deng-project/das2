# das2: Improved DENG asset manager library
# licence: Apache, see LICENCE file
# file: DasDump.cmake - das file dump program cmake configuration file
# author: Karl-Mihkel Ott

set(DAS_DUMP_TARGET DasDump)
set(DAS_DUMP_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/das2/demos/DasDump.h)
set(DAS_DUMP_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/demos/DasDump.cpp)

add_executable(${DAS_DUMP_TARGET}
    ${DAS_DUMP_HEADERS}
    ${DAS_DUMP_SOURCES})

add_dependencies(${DAS_DUMP_TARGET}
    ${DAS2_TARGET})

target_link_libraries(${DAS_DUMP_TARGET}
    PRIVATE ${DAS2_TARGET})