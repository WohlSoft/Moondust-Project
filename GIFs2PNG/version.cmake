# Major
set(GIFS2PNG_VERSION_1 3)
# Minor
set(GIFS2PNG_VERSION_2 2)
# Revision
set(GIFS2PNG_VERSION_3 0)
# Patch
set(GIFS2PNG_VERSION_4 0)
# Type of version: "-alpha","-beta","-dev", or "" aka "release"
set(GIFS2PNG_VERSION_REL "")

add_definitions(-DGIFS2PNG_VERSION_1=${GIFS2PNG_VERSION_1})
add_definitions(-DGIFS2PNG_VERSION_2=${GIFS2PNG_VERSION_2})
add_definitions(-DGIFS2PNG_VERSION_3=${GIFS2PNG_VERSION_3})
add_definitions(-DGIFS2PNG_VERSION_4=${GIFS2PNG_VERSION_4})
add_definitions(-DGIFS2PNG_VERSION_REL=${GIFS2PNG_VERSION_REL})

set(GIFS2PNG_VERSION_STRING "${GIFS2PNG_VERSION_1}.${GIFS2PNG_VERSION_2}")

if(NOT ${GIFS2PNG_VERSION_3} EQUAL 0 OR NOT ${GIFS2PNG_VERSION_4} EQUAL 0)
    string(CONCAT GIFS2PNG_VERSION_STRING "${GIFS2PNG_VERSION_STRING}" ".${GIFS2PNG_VERSION_3}")
endif()

if(NOT ${GIFS2PNG_VERSION_4} EQUAL 0)
    string(CONCAT GIFS2PNG_VERSION_STRING "${GIFS2PNG_VERSION_STRING}" ".${GIFS2PNG_VERSION_4}")
endif()

if(NOT "${GIFS2PNG_VERSION_REL}" STREQUAL "")
    string(CONCAT GIFS2PNG_VERSION_STRING "${GIFS2PNG_VERSION_STRING}" "${GIFS2PNG_VERSION_REL}")
endif()

message("== GIFs2PNG version ${GIFS2PNG_VERSION_STRING} ==")
