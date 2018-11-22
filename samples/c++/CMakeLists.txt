cmake_minimum_required(VERSION 3.2)

project(sample)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenCV REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Net NetSSL Util)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

FILE(GLOB INCLUDES include/*.h)
FILE(GLOB SOURCES src/*.cpp)

source_group("Header Files" FILES ${INCLUDES})
source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${INCLUDES} ${SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} ${Poco_LIBRARIES} ${OpenCV_LIBS})
install(TARGETS ${PROJECT_NAME} DESTINATION /home/upsquared/cortexica/sample/bin/)
