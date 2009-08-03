

set(XIOT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(XML_PARSER_SELECTION "XERCES" CACHE STRING "External XML parser to use. One of (XERCES | EXPAT)")

string(TOLOWER ${XML_PARSER_SELECTION} ${XML_PARSER_SELECTION})

# Find the Z library
if (WIN32)
  # Hint to included zlib resources
  set(ZLIB_ROOT ${PROJECT_SOURCE_DIR}/contrib/zlib)
  set(ZLIB_INCLUDE_DIR ${ZLIB_ROOT}/include)
  set(ZLIB_LIBRARY ${ZLIB_ROOT}/${WIN_ARCH}/lib/zlib.lib)
endif (WIN32)
find_package(ZLIB REQUIRED)

# Set up the chosen XML parser library
if(${XML_PARSER_SELECTION} STREQUAL "xerces")

	if (WIN32)
	  # Hint to included XercesC resources
	  SET(XERCESC_ROOT ${PROJECT_SOURCE_DIR}/contrib/xercesc)
	  SET(XML_PARSER_INSTALL_FILES ${XERCESC_ROOT}/${WIN_ARCH}/bin/xerces-c_3_0.dll)
	  SET(XERCESC_INCLUDE_DIR ${XERCESC_ROOT}/include CACHE PATH "Include path of XercesC library")
	  SET(XERCESC_LIBRARY ${XERCESC_ROOT}/${WIN_ARCH}/lib/xerces-c_3.lib CACHE FILEPATH  "XercesC library" )
	else (WIN32)
	  SET(XERCESC_INCLUDE_DIR "" CACHE PATH "Include path of XercesC library")
	  SET(XERCESC_LIBRARY "" CACHE FILEPATH  "XercesC library" )
	endif (WIN32)
	
	find_package(XercesC)
	
	if (XERCESC_FOUND)
		set (XML_PARSER_INCLUDE_DIR ${XERCESC_INCLUDE_DIR})
		set (XML_PARSER_LIBRARY ${XERCESC_LIBRARY})
	else (XERCESC_FOUND)
		message(SEND_ERROR "Xerces is chosen as external XML parser, but library is not found.")
	endif(XERCESC_FOUND)
	
	set(XML_PARSER_IMPL_SRC	X3DXMLLoaderXercesImpl.cpp X3DXMLAttributesXercesImpl.cpp)

elseif(${XML_PARSER_SELECTION} STREQUAL "expat")
	if (WIN32)
	  # Hint to included EXPAT resources
	  SET(EXPAT_ROOT ${PROJECT_SOURCE_DIR}/contrib/expat)
	  SET(XML_PARSER_INSTALL_FILES ${EXPAT_ROOT}/${WIN_ARCH}/bin/libexpat.dll)
	  SET(EXPAT_INCLUDE_DIR ${EXPAT_ROOT}/include CACHE PATH "Include path of Expat library")
	  SET(EXPAT_LIBRARY ${EXPAT_ROOT}/${WIN_ARCH}/lib/libexpat.lib CACHE FILEPATH  "Expat library" )
	else (WIN32)
	  SET(EXPAT_INCLUDE_DIR "" CACHE PATH "Include path of Expat library")
	  SET(EXPAT_LIBRARY "" CACHE FILEPATH "Expat library" )
	endif (WIN32)
	
	find_package(EXPAT)
	
	if (EXPAT_FOUND)
		set (XML_PARSER_INCLUDE_DIR ${EXPAT_INCLUDE_DIRS})
		set (XML_PARSER_LIBRARY ${EXPAT_LIBRARIES})
	else (EXPAT_FOUND)
		message(SEND_ERROR "Expat is chosen as external XML parser, but library is not found.")
	endif(EXPAT_FOUND)
	
	set(XML_PARSER_IMPL_SRC	X3DXMLLoaderExpatImpl.cpp X3DXMLAttributesExpatImpl.cpp)
	add_definitions( -DXML_STATIC)
	
endif(${XML_PARSER_SELECTION} STREQUAL "xerces")

set(XIOT_HEADER
	${XIOT_INCLUDE_DIR}/xiot/X3DTypes.h
	${XIOT_INCLUDE_DIR}/xiot/X3DLoader.h
	${XIOT_INCLUDE_DIR}/xiot/X3DXMLLoader.h
	${XIOT_INCLUDE_DIR}/xiot/X3DAttributes.h
	${XIOT_INCLUDE_DIR}/xiot/X3DXMLAttributes.h
	${XIOT_INCLUDE_DIR}/xiot/X3DFIAttributes.h
	${XIOT_INCLUDE_DIR}/xiot/X3DSwitch.h
	${XIOT_INCLUDE_DIR}/xiot/X3DNodeHandler.h
	${XIOT_INCLUDE_DIR}/xiot/X3DDefaultNodeHandler.h
	${XIOT_INCLUDE_DIR}/xiot/X3DFILoader.h
	${XIOT_INCLUDE_DIR}/xiot/X3DDataTypeFactory.h
	${XIOT_INCLUDE_DIR}/xiot/X3DParseException.h
	${XIOT_INCLUDE_DIR}/xiot/X3DFICompressionTools.h
	${XIOT_INCLUDE_DIR}/xiot/X3DParserVocabulary.h
	${XIOT_INCLUDE_DIR}/xiot/X3DFIEncodingAlgorithms.h
	${XIOT_INCLUDE_DIR}/xiot/XIOTConfigure.h
	${XIOT_INCLUDE_DIR}/xiot/X3DWriter.h
	${XIOT_INCLUDE_DIR}/xiot/X3DWriterFI.h
	${XIOT_INCLUDE_DIR}/xiot/X3DWriterFIHelper.h
	${XIOT_INCLUDE_DIR}/xiot/X3DWriterXML.h
	${XIOT_INCLUDE_DIR}/xiot/X3DZLibDataCompressor.h

)


set(XIOT_SRC
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
	X3DWriterXML.cpp
	X3DZLibDataCompressor.cpp
	${XML_PARSER_IMPL_SRC}
)

set(OPENFI_SRC
	FIDecoder.cpp
	FIContentHandler.cpp
	FISAXParser.cpp
	FIParserVocabulary.cpp
	FIEncodingAlgorithms.cpp
)

set(OPENFI_HEADER
	${XIOT_INCLUDE_DIR}/xiot/FIConstants.h
	${XIOT_INCLUDE_DIR}/xiot/FITypes.h
	${XIOT_INCLUDE_DIR}/xiot/FIDecoder.h
	${XIOT_INCLUDE_DIR}/xiot/FIContentHandler.h
	${XIOT_INCLUDE_DIR}/xiot/FISAXParser.h
	${XIOT_INCLUDE_DIR}/xiot/FIParserVocabulary.h
	${XIOT_INCLUDE_DIR}/xiot/FIEncodingAlgorithms.h
)

# set up the include directories
include_directories( ${XIOT_INCLUDE_DIR} )
include_directories( ${ZLIB_INCLUDE_DIR} )
include_directories( ${XML_PARSER_INCLUDE_DIR} )

# set up openFI
add_library(openFI SHARED ${OPENFI_SRC} ${OPENFI_HEADER})

# set up XIOT
add_library(xiot SHARED ${XIOT_SRC} ${XIOT_HEADER})
target_link_libraries (xiot ${XML_PARSER_LIBRARY}) 
target_link_libraries (xiot ${ZLIB_LIBRARIES}) 
target_link_libraries (xiot openFI) 


add_executable(sourceGenerator gen/sourceGenerator.cpp)
target_link_libraries (sourceGenerator xiot) 

# install stuff
install(TARGETS xiot openFI
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
install(FILES ${XIOT_HEADER} ${OPENFI_HEADER} DESTINATION include)

if (WIN32)
	install(FILES ${XML_PARSER_INSTALL_FILES} DESTINATION bin)
endif (WIN32)
