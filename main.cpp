#include <opencv2/opencv.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <iostream>
#include <vector>
#include <sstream>
void testCameraCalibration(const std::string& yamlFile);

int main() {
    // --- ChArUco board parameters ---
    int squaresX = 7;
    int squaresY = 5;
    float squareLength = 0.02f; // meters
    float markerLength = 0.015f; // meters
    int dictionaryId = cv::aruco::DICT_6X6_250;

    cv::Ptr<cv::aruco::Dictionary> dictionary =
        cv::aruco::getPredefinedDictionary(dictionaryId);

    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(
        squaresX, squaresY, squareLength, markerLength, dictionary);

    cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();

    std::vector<cv::Mat> allCharucoCorners, allCharucoIds;
    cv::Size imageSize;

    // --- Loop through images ---
    for (int i = 1; i <= 43; ++i) {
        std::ostringstream filename;
        filename << "/home/cvlab/projects/CharucoCalibration/imgs/" << i << ".jpg";

        cv::Mat image = cv::imread(filename.str());
        if (image.empty()) {
            std::cerr << "Could not read " << filename.str() << std::endl;
            continue;
        }

        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        // Detect markers
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners;
        cv::aruco::detectMarkers(gray, dictionary, markerCorners, markerIds, detectorParams);

        // Draw detected markers
        cv::aruco::drawDetectedMarkers(image, markerCorners, markerIds);

        // Interpolate ChArUco corners
        cv::Mat charucoCorners, charucoIds;
        if (!markerIds.empty()) {
            cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, gray,
                                                 board, charucoCorners, charucoIds);

            // Draw ChArUco corners
            if (charucoIds.total() > 0) {
                cv::aruco::drawDetectedCornersCharuco(image, charucoCorners, charucoIds);
                allCharucoCorners.push_back(charucoCorners);
                allCharucoIds.push_back(charucoIds);
                imageSize = image.size();
                std::cout << "Processed " << filename.str() << std::endl;
            }
        }

        // Show image
        cv::imshow("ChArUco Detection", image);
        char key = (char) cv::waitKey(0); // Wait for key press
        if (key == 27) { // ESC key to quit early
            break;
        }
    }

    cv::destroyAllWindows();

    if (allCharucoCorners.size() < 3) {
        std::cerr << "Not enough valid images for calibration!" << std::endl;
        return -1;
    }

    // --- Calibrate camera using ChArUco ---
    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    double repError = cv::aruco::calibrateCameraCharuco(
        allCharucoCorners, allCharucoIds, board, imageSize,
        cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "Reprojection error: " << repError << "\n";
    std::cout << "Camera Matrix:\n" << cameraMatrix << "\n";
    std::cout << "Distortion Coefficients:\n" << distCoeffs << "\n";

    // --- Save calibration ---
    cv::FileStorage fs("/home/cvlab/projects/CharucoCalibration/calibration.yaml", cv::FileStorage::WRITE);
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs.release();

    std::cout << "Calibration saved to calibration.yaml" << std::endl;

    testCameraCalibration("/home/cvlab/projects/CharucoCalibration/calibration.yaml");

    return 0;
}

void testCameraCalibration(const std::string& yamlFile) {
    // --- Load calibration ---
    cv::Mat cameraMatrix, distCoeffs;
    cv::FileStorage fs(yamlFile, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Failed to open " << yamlFile << std::endl;
        return;
    }
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    fs.release();

    // --- Open camera ---
    cv::VideoCapture cap(0); // default camera
    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera!" << std::endl;
        return;
    }

    cv::Mat frame, undistorted;
    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Undistorted", cv::WINDOW_NORMAL);

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Undistort frame
        cv::undistort(frame, undistorted, cameraMatrix, distCoeffs);

        cv::imshow("Original", frame);
        cv::imshow("Undistorted", undistorted);

        char key = (char) cv::waitKey(1);
        if (key == 27) break; // ESC to quit
    }

    cap.release();
    cv::destroyAllWindows();
}