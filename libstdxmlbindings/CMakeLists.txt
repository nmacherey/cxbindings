include(library)

FILE(GLOB OBJECTS_SRCS src/*.cpp)

set(SRCS ${OBJECTS_SRCS})

INCLUDE_DIRECTORIES (SYSTEM "${LIBXML2_INCLUDE_DIR}")
ADD_DEFINITIONS ("${LIBXML2_DEFINITIONS}")

add_lib(stdxmlbindings ${SRCS})
target_link_libraries(stdxmlbindings ${LIBXML2_LIBRARIES} ${Boost_LIBRARIES})
