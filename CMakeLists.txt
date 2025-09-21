cmake_minimum_required(VERSION 3.10)
project(camera_calibration)

set(CMAKE_CXX_STANDARD 17)

# --- Manually specify OpenCV include and libraries ---
include_directories(/usr/include/opencv4)

add_executable(camera_calibration main.cpp)

# Link OpenCV libraries manually
target_link_libraries(camera_calibration
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_imgcodecs
    opencv_videoio
    opencv_aruco
    opencv_calib3d
)
