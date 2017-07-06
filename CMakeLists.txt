# CMake 最低版本号要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 项目信息
PROJECT(uav-circle-detection)

# # 生成release版本
# SET(CMAKE_BUILD_TYPE "RELEASE")

# if (CMAKE_BUILD_TYPE STREQUAL debug)
#     ADD_DEFINITIONS(-D_DEBUG)
# endif ()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++11")

# 添加包含目录
INCLUDE_DIRECTORIES(./ ./MST ./source)

IF(WIN32)
# Windows平台执行
INCLUDE($ENV{OPENCV_2413_PREBUILD_DIR}/OpenCVConfig.cmake)
message( STATUS "OpenCV library status:" )              # 输出一下得到的变量
message( STATUS "version: ${OpenCV_VERSION}" )
message( STATUS "libraries: ${OpenCV_LIBS}" )
message( STATUS "include path: ${OpenCV_INCLUDE_DIRS}" )

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

ELSEIF(UNIX)
# Linux平台执行

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
ENDIF(WIN32)


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

# 包含子目录
ADD_SUBDIRECTORY(source)

# 指定生成目标
ADD_EXECUTABLE(uav-circle-detection ${DIR_SRCS})

# 链接文件
TARGET_LINK_LIBRARIES(uav-circle-detection ${OpenCV_LIBS} source_lib)

# 设置可执行文件生成路径
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)