cmake_minimum_required(VERSION 2.6)

set (VTKIMPORTER_SRC
	vtkX3DImporter.cpp
	vtkX3DNodeHandler.cpp
	vtkX3DNodeHandler.h
	vtkX3DIndexedFaceSetSource.h
	vtkX3DIndexedFaceSetSource.cpp
	vtkX3DIndexedLineSetSource.h
	vtkX3DIndexedLineSetSource.cpp
)

find_package(VTK 5.0 REQUIRED)

include_directories(. ${PROJECT_SOURCE_DIR}/include ${VTK_INCLUDE_DIRS})

# vtkX3DLoader
link_directories(${VTK_LIBRARY_DIRS} )
add_executable (vtkX3DLoader ${VTKIMPORTER_SRC} vtkX3DLoader.cpp Argument_helper.cpp)
target_link_libraries(vtkX3DLoader xiot vtkCommon vtkHybrid vtkRendering vtkIO vtkFiltering vtkCommon vtkGraphics)

# glyphTest
ADD_EXECUTABLE(x3dglyphtest Argument_helper.cc X3DTest.cxx main.cxx vtkX3DExporterGeneric.cxx)
TARGET_LINK_LIBRARIES(x3dglyphtest xiot vtkCommon vtkHybrid vtkRendering vtkIO vtkFiltering vtkCommon vtkGraphics)


install(TARGETS vtkX3DLoader RUNTIME DESTINATION bin)
install(TARGETS x3dglyphtest RUNTIME DESTINATION bin)
