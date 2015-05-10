LIST(REMOVE_ITEM BII_LIB_SRC tests/Argument_helper.cpp)
LIST(APPEND BII_tests_createEventLog_SRC tests/X3DLogNodeHandler.cpp)

ADD_BII_TARGETS()
