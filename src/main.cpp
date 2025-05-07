#include <iostream>
#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>
#include <filesystem>
#include "utils.h"
#include "yolov8Predictor.h"

using namespace rs2;

float confThreshold = 0.5f;
float iouThreshold = 0.5f;
float maskThreshold = 0.6f;

std::string modelPath = "/home/ichan/Documents/realSense_OnnxRT/models/zed_obs.onnx";
std::string classNamesPath = "/home/ichan/Documents/realSense_OnnxRT/models/coconames.txt";
bool isGPU = true;

int main() {
    // Load class names
    const std::vector<std::string> classNames = utils::loadNames(classNamesPath);
    if (classNames.empty()) {
        std::cerr << "Error: Class name file is empty." << std::endl;
        return -1;
    }

    // Check if the model file exists
    if (!std::filesystem::exists(modelPath)) {
        std::cerr << "Error: Model file does not exist." << std::endl;
        return -1;
    }

    // Initialize YOLO
    YOLOPredictor predictor{nullptr};
    try {
        predictor = YOLOPredictor(modelPath, isGPU, confThreshold, iouThreshold, maskThreshold);
        std::cout << "YOLO model initialized successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    assert(classNames.size() == predictor.classNums);

    // Initialize RealSense
    rs2::pipeline pipe;
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    pipe.start(cfg);
    std::cout << "RealSense started. Press ESC to quit.\n";

    while (true) {
        rs2::frameset frames = pipe.wait_for_frames();
        rs2::video_frame colorFrame = frames.get_color_frame();
        rs2::depth_frame depthFrame = frames.get_depth_frame();

        cv::Mat colorImage(cv::Size(colorFrame.get_width(), colorFrame.get_height()),
                           CV_8UC3,
                           (void*)colorFrame.get_data(),
                           cv::Mat::AUTO_STEP);

        std::vector<Yolov8Result> result = predictor.predict(colorImage);
        utils::visualizeDetection(colorImage, result, classNames);

        for (const auto& res : result) {
            int classId = res.classId;
            std::string detectedClass = classNames[classId];
        
            // Calculate the center point of the bounding box
            int centerX = res.box.x + res.box.width / 2;
            int centerY = res.box.y + res.box.height / 2;
            cv::Point objectCenter(centerX, centerY);
        
            // Draw a vertical line from the center of the object to the bottom of the frame
            cv::line(colorImage, objectCenter, cv::Point(objectCenter.x, colorImage.rows - 1),
                     cv::Scalar(0, 0, 255), 2);  // Red color
        
            // Add depth text if the detected object is a Ball
            if (detectedClass == "Ball") {
                float ballDepth = depthFrame.get_distance(centerX, centerY);
                if (ballDepth > 0) {
                    std::ostringstream ss;
                    ss << std::fixed << std::setprecision(2) << ballDepth << "m";
                    cv::putText(colorImage, ss.str(), cv::Point(centerX + 10, centerY),
                                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);
                    std::cout << "Ball detected at depth: " << ballDepth << " meters" << std::endl;
                }
            }
        }
        
        cv::imshow("YOLOv8 + RealSense", colorImage);
        if (cv::waitKey(1) == 27) {  // ESC
            break;
        }
    }

    pipe.stop();
    cv::destroyAllWindows();
    std::cout << "Done." << std::endl;
    return 0;
}
