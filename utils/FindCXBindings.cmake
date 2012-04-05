#    Find dadi library

# This module defines:
# CXBINDINGS_FOUND		- whether library was found
# CXBINDINGS_INCLUDE_DIR	- include path
# CXBINDINGS_LIBRARIES	- libraries to link against

# get some hints from pkg-config
if(NOT WIN32)
  include(FindPkgConfig)
  # run quietly since we only retrieve some hints from pkg-config
  pkg_check_modules(CXBINDINGS QUIET cxbindings)
  # returns CXBINDINGS_INCLUDE_DIRS and CXBINDINGS_LIBRARY_DIRS
endif()

find_path(CXBINDINGS_INCLUDE_DIR
  NAMES cxbindings/cxbindings/CXBindings.h
  PATHS ${CXBINDINGS_INSTALL_DIR}/include
  ${CXBINDINGS_INCLUDE_DIR}
)

find_library(CXBINDINGS_LIBRARIES
  NAMES $cxbindings
  PATHS_SUFFIXES cxbindings
  PATHS ${CXBINDINGS_INSTALL_DIR}/lib
  ${CXBINDINGS_LIBRARY_DIRS}
)

# defines our variables
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CXBINDINGS DEFAULT_MSG
  CXBINDINGS_LIBRARIES CXBINDINGS_INCLUDE_DIR)
