cmake_minimum_required(VERSION 3.0.0)
project(realSense_OnnxRT)

if(NOT DEFINED ONNXRUNTIME_DIR)
    set(ONNXRUNTIME_DIR $ENV{ONNXRUNTIME_DIR})  
endif()

if(NOT DEFINED ONNXRUNTIME_DIR)
    message(FATAL_ERROR "ONNX Runtime directory not specified! Set ONNXRUNTIME_DIR.")
else()
    message(STATUS "ONNXRUNTIME_DIR: ${ONNXRUNTIME_DIR}")
endif()

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV found: ${OpenCV_LIBS}")

include_directories(
    "${ONNXRUNTIME_DIR}/include"
    "/usr/include/librealsense2"
    "${PROJECT_SOURCE_DIR}/include"
)

add_executable(cam_onnxRT
    src/utils.cpp
    src/yolov8Predictor.cpp
    src/main.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
    set(ONNXRUNTIME_LIB "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so")
elseif(WIN32)
    set(ONNXRUNTIME_LIB "${ONNXRUNTIME_DIR}/lib/onnxruntime.lib")
endif()

target_link_libraries(cam_onnxRT 
    ${OpenCV_LIBS}
    ${ONNXRUNTIME_LIB}
    realsense2
)