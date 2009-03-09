
set(X3DLOADER_HEADER
	X3DTypes.h
	X3DLoader.h
	X3DXMLLoader.h
	X3DAttributes.h
	X3DXMLAttributes.h
	X3DFIAttributes.h
	X3DSwitch.h
	X3DNodeHandler.h
	X3DDefaultNodeHandler.h
	X3DFILoader.h
	X3DDataTypeFactory.h
	X3DParseException.h
	X3DFICompressionTools.h
	X3DParserVocabulary.h
	X3DFIEncodingAlgorithms.h
	X3DLoaderConfigure.h
)


set(X3DLOADER_SRC
	X3DTypes.cpp
	X3DLoader.cpp
	X3DXMLLoader.cpp
	X3DAttributes.cpp
	X3DXMLAttributes.cpp
	X3DFIAttributes.cpp
	X3DSwitch.cpp
	X3DDefaultNodeHandler.cpp
	X3DFILoader.cpp
	X3DDataTypeFactory.cpp
	X3DParseException.cpp
	X3DFICompressionTools.cpp
	X3DParserVocabulary.cpp
	X3DFIEncodingAlgorithms.cpp
)

set(OPENFI_SRC
	FIDecoder.cpp
	FIContentHandler.cpp
	FISAXParser.cpp
	FIParserVocabulary.cpp
	FIEncodingAlgorithms.cpp
)

set(OPENFI_HEADER
	FIConstants.h
	FITypes.h
	FIDecoder.h
	FIContentHandler.h
	FISAXParser.h
	FIParserVocabulary.h
	FIEncodingAlgorithms.h
)

IF (WIN32)
  # Hint to included zlib resources
  SET(ZLIB_ROOT ${PROJECT_SOURCE_DIR}/contrib/zlib)
  SET(ZLIB_INCLUDE_DIR ${ZLIB_ROOT}/include)
  SET(ZLIB_LIBRARY ${ZLIB_ROOT}/${WIN_ARCH}/lib/zlib.lib)

  # Hint to included XercesC resources
  SET(XERCESC_ROOT ${PROJECT_SOURCE_DIR}/contrib/xercesc)
  SET(XERCESC_INCLUDE_DIR ${XERCESC_ROOT}/include)
  SET(XERCESC_LIBRARY ${XERCESC_ROOT}/${WIN_ARCH}/lib/xerces-c_3.lib)
  SET(XERCESC_DLL ${XERCESC_ROOT}/${WIN_ARCH}/bin/xerces-c_3_0.dll)
ENDIF (WIN32)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})

find_package(XercesC REQUIRED)
include_directories( ${XERCESC_INCLUDE_DIR})


add_library(openFI SHARED ${OPENFI_SRC} ${OPENFI_HEADER})


add_library(x3dLoader SHARED ${X3DLOADER_SRC} ${X3DLOADER_HEADER})
target_link_libraries (x3dLoader ${XERCESC_LIBRARY}) 
target_link_libraries (x3dLoader ${ZLIB_LIBRARIES}) 
target_link_libraries (x3dLoader openFI) 

include_directories(.)
add_executable(sourceGenerator gen/sourceGenerator.cpp)
target_link_libraries (sourceGenerator x3dLoader) 

# install stuff
install(TARGETS x3dLoader openFI
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
install(FILES ${X3DLOADER_HEADER} ${OPENFI_HEADER} DESTINATION include)

if (WIN32)
	install(FILES ${XERCESC_DLL} DESTINATION bin)
endif (WIN32)
