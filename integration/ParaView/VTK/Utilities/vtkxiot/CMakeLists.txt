PROJECT(VTKXIOT)

INCLUDE_DIRECTORIES(${VTKXIOT_SOURCE_DIR}/include)

IF(WIN32)
  IF(BUILD_SHARED_LIBS)
    SET(XIOT_DLL 1)
  ELSE(BUILD_SHARED_LIBS)
    SET(XIOT_STATIC 1)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF(WIN32)

set( VTKXIOT_SRC
	X3DTypes.cpp
	X3DLoader.cpp
	X3DAttributes.cpp
	X3DFIAttributes.cpp
	X3DSwitch.cpp
	X3DDefaultNodeHandler.cpp
	X3DFILoader.cpp
	X3DDataTypeFactory.cpp
	X3DParseException.cpp
	X3DFICompressionTools.cpp
	X3DParserVocabulary.cpp
	X3DFIEncodingAlgorithms.cpp
	X3DWriterFI.cpp
	X3DWriter.cpp
	X3DFIEncoder.cpp
	X3DWriterXML.cpp
	FIEncoder.cpp
	FIDecoder.cpp
	FIContentHandler.cpp
	FISAXParser.cpp
	FIParserVocabulary.cpp
	FIEncodingAlgorithms.cpp
	X3DXMLLoaderExpatImpl.cpp
	X3DXMLAttributesExpatImpl.cpp )

ADD_LIBRARY(vtkxiot ${VTKXIOT_SRC} )
TARGET_LINK_LIBRARIES(vtkxiot ${VTK_ZLIB_LIBRARIES} ${VTK_EXPAT_LIBRARIES})

# Apply user-defined properties to the library target.
IF(VTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(vtkxiot PROPERTIES ${VTK_LIBRARY_PROPERTIES})
ENDIF(VTK_LIBRARY_PROPERTIES)

IF(NOT VTK_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS vtkxiot
    RUNTIME DESTINATION ${VTK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${VTK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${VTK_INSTALL_LIB_DIR_CM24} COMPONENT Development)
ENDIF(NOT VTK_INSTALL_NO_LIBRARIES)

CONFIGURE_FILE(${VTKXIOT_SOURCE_DIR}/include/xiot/XIOTConfig.h.in
               ${VTKXIOT_BINARY_DIR}/include/xiot/XIOTConfig.h)
