# TODO: comment
macro(add_exe _NAME)
  	add_definitions(-DMODULE_PREFIX="${CMAKE_SHARED_MODULE_PREFIX}")
	add_definitions(-DMODULE_SUFFIX="${CMAKE_SHARED_MODULE_SUFFIX}")

	#include_directories(${PROJECT_SOURCE_DIR}/${_NAME}/include)
	include_directories( 
		${PROJECT_SOURCE_DIR}/${_NAME}/include
	)

	add_executable(${_NAME} ${ARGN})

	## installation
	INSTALL( TARGETS ${_NAME}  DESTINATION bin )
endmacro()
