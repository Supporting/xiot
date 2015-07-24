include(GenerateExportHeader)

LIST(REMOVE_ITEM BII_LIB_SRC tests/Argument_helper.cpp)
LIST(APPEND BII_tests_createEventLog_SRC tests/X3DLogNodeHandler.cpp)

ADD_BII_TARGETS()

GENERATE_EXPORT_HEADER( ${BII_LIB_TARGET} BASE_NAME xiot)
GENERATE_EXPORT_HEADER( ${BII_LIB_TARGET} BASE_NAME openfi)
target_include_directories( ${BII_LIB_TARGET} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
