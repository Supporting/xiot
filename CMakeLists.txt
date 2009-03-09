cmake_minimum_required(VERSION 2.6)

project(x3dLoader)
 
# Setup output paths.
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

if (CMAKE_CL_64)
	SET(WIN_ARCH win64)
else (CMAKE_CL_64)
	if (WIN32)
	SET(WIN_ARCH win32)
	endif (WIN32)
endif (CMAKE_CL_64)

message(STATUS "Windows architecture is: ${WIN_ARCH}")

SET(CMAKE_MODULE_PATH "${x3dLoader_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")


add_subdirectory(src)
add_subdirectory(tests)

#Documentation
option(BUILD_X3DLOADER_DOCS "Build Doxygen documentation" OFF)
if(BUILD_X3DLOADER_DOCS)
  include(TargetDoc)
  if (WIN32)
	install(FILES ./doc/README_SDK9 DESTINATION . RENAME README)
	if(EXISTS ${PROJECT_BINARY_DIR}/doc/html)
		install(DIRECTORY ${PROJECT_BINARY_DIR}/doc/html DESTINATION doc)
	endif(EXISTS ${PROJECT_BINARY_DIR}/doc/html)
  endif (WIN32)
endif(BUILD_X3DLOADER_DOCS)

# Examples
option(BUILD_X3DLOADER_EXAMPLES "Enable to build Examples (VTK loader)" OFF)
if(BUILD_X3DLOADER_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_X3DLOADER_EXAMPLES)

#ParaView plugin
option(BUILD_PARAVIEW_PLUGIN "Build ParaView X3D reader plugin" OFF)
if(BUILD_PARAVIEW_PLUGIN)
  add_subdirectory(plugins)
endif(BUILD_PARAVIEW_PLUGIN)