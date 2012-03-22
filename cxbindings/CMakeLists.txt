include(library)

FILE(GLOB OBJECTS_SRCS src/*.cpp src/handlers/*.cpp src/generators/*.cpp)

set(SRCS ${OBJECTS_SRCS})

INCLUDE_DIRECTORIES (SYSTEM "${LIBXML2_INCLUDE_DIR}")
ADD_DEFINITIONS ("${LIBXML2_DEFINITIONS}")

add_lib(cxbindings ${SRCS})
target_link_libraries(cxbindings ${LIBXML2_LIBRARIES})
