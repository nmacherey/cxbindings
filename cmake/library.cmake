# TODO: comment
macro(add_lib _NAME)
  	add_definitions(-DMODULE_PREFIX="${CMAKE_SHARED_MODULE_PREFIX}")
	add_definitions(-DMODULE_SUFFIX="${CMAKE_SHARED_MODULE_SUFFIX}")

	#include_directories(${PROJECT_SOURCE_DIR}/${_NAME}/include)
	include_directories( 
		${PROJECT_SOURCE_DIR}/${_NAME}/include
	)

	add_library(${_NAME} ${ARGN})

	set_target_properties(${_NAME} PROPERTIES
  	VERSION ${CXBINDINGS_VERSION}
  	SOVERSION ${CXBINDINGS_VERSION_MAJOR})

	## installation
	install(TARGETS ${_NAME}
  		COMPONENT runtime
  		ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  		LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  		RUNTIME DESTINATION ${LIB_INSTALL_DIR})

	install(DIRECTORY "include/"
	  DESTINATION "${INCLUDE_INSTALL_DIR}/cxbindings/${_NAME}"
	  COMPONENT development
	  FILES_MATCHING PATTERN "*.h")
endmacro()
