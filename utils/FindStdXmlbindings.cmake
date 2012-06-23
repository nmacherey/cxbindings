# Find dadi library
# This module defines:
# STDXMLBINDINGS_FOUND		- whether library was found
# STDXMLBINDINGS_INCLUDE_DIR	- include path
# STDXMLBINDINGS_LIBRARIES	- libraries to link against

# get some hints from pkg-config
if(NOT WIN32)
  include(FindPkgConfig)
  # run quietly since we only retrieve some hints from pkg-config
  pkg_check_modules(STDXMLBINDINGS QUIET stdxmlbindings)
  # returns STDXMLBINDINGS_INCLUDE_DIRS and STDXMLBINDINGS_LIBRARY_DIRS
endif()

find_path(StdXmlbindings_INCLUDE_DIR
  NAMES stdObject.h
  PATHS ${STDXMLBINDINGS_INSTALL_DIR}/include/cxbindings
  ${STDXMLBINDINGS_INCLUDE_DIRS}
)

find_library(StdXmlbindings_LIBRARIES
  NAMES stdxmlbindings
  PATHS_SUFFIXES stdxmlbindings
  PATHS ${STDXMLBINDINGS_INSTALL_DIR}/lib
  ${STDXMLBINDINGS_LIBRARY_DIRS}
)

# defines our variables
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(STDXMLBINDINGS DEFAULT_MSG
  StdXmlbindings_LIBRARIES StdXmlbindings_INCLUDE_DIR)
