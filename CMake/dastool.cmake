# das2: Improved DENG asset manager library
# licence: Apache, see LICENCE file
# file: dastool.cmake - dastool utility program CMake configuration file
# author: Karl-Mihkel Ott

set(DASTOOL_TARGET dastool)
set(DASTOOL_HEADERS)
set(DASTOOL_SOURCES)

add_executable(${DASTOOL_TARGET}
    ${DASTOOL_HEADERS}
    ${DASTOOL_SOURCES})

add_dependencies(${DASTOOL_TARGET} ${DAS2_TARGET})
target_link_libraries(${DASTOOL_TARGET} PRIVATE ${DAS2_TARGET})
