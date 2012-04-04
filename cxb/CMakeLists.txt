include(executable)

FILE(GLOB OBJECTS_SRCS *.cpp)

set(SRCS ${OBJECTS_SRCS})

INCLUDE_DIRECTORIES (SYSTEM "${LIBXML2_INCLUDE_DIR}" "../libcxbindings/include" )
ADD_DEFINITIONS ("${LIBXML2_DEFINITIONS}")

add_exe(cxb ${SRCS})
target_link_libraries(cxb ${LIBXML2_LIBRARIES} ${Boost_LIBRARIES} cxbindings)
