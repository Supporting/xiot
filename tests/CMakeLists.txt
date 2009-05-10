cmake_minimum_required(VERSION 2.6)

include_directories(. ${PROJECT_SOURCE_DIR}/src)

#simpleTest
add_executable (simpleTest simpleTest.cpp Argument_helper.cpp)
target_link_libraries(simpleTest x3dLoader)

#shapeCounter
add_executable (shapeCounter shapeCounter.cpp Argument_helper.cpp)
target_link_libraries(shapeCounter x3dLoader)

#FITest
add_executable (fitest fitest.cpp Argument_helper.cpp)
target_link_libraries(fitest x3dLoader)

#IOTest
add_executable (ioTest IOTest.cpp Argument_helper.cpp)
target_link_libraries(ioTest x3dLoader)


#createEventLog
add_executable (createEventLog createEventLog.cpp Argument_helper.cpp X3DLogNodeHandler.cpp X3DLogNodeHandler.h)
target_link_libraries(createEventLog x3dLoader)


install(TARGETS simpleTest shapeCounter fitest createEventLog RUNTIME DESTINATION bin)