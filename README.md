# Camera-Calibration-With-Charuco-Board-Cpp
A C++ implementation of camera calibration using an ArUco ChArUco board with OpenCV. This project detects ArUco markers and interpolates ChArUco corners across multiple calibration images to compute the camera matrix and distortion coefficients. It also includes real-time testing with live camera undistortion.


![Image](https://github.com/user-attachments/assets/cc0620f9-1079-48e7-a3a9-fdb729b1ccee)

# [YOUTUBE LINK](https://www.youtube.com/watch?v=RU9uCu2yIWE)

✨ Features

Detect ArUco markers and interpolate ChArUco corners

Perform camera calibration using captured ChArUco images

Save and load calibration results in YAML format

Real-time undistortion preview from a connected camera

Works with OpenCV ArUco module (≥ 4.5 recommended)

🔧 Requirements

C++17 or later

OpenCV (with aruco, calib3d, highgui, imgproc, videoio modules)

🚀 Usage

Capture multiple ChArUco board images from different angles.

Run the calibration program with your dataset.

Get camera intrinsic parameters (camera_matrix) and distortion coefficients.

Test calibration with real-time undistortion from your webcam.
