## Prerequisites

- OpenCV with Contrib Modules: https://github.com/opencv/opencv_contrib

- Intel RealSense SDK : https://github.com/IntelRealSense/librealsense/blob/master/doc/installation.md

- ONNX Runtime (C++)

        wget https://github.com/microsoft/onnxruntime/releases/download/v1.17.0/onnxruntime-linux-x64-dev-1.17.0.tgz
        tar -xvzf onnxruntime-linux-x64-dev-1.17.0.tgz
        sudo mv onnxruntime-linux-x64-dev-1.17.0 /opt/onnxruntime

- YOLO ONNX Model. Download a YOLOv5, YOLOv7, or YOLOv8 model in ONNX format and place it in the models/ directory.

### Build and Run

        # 1. Clone the repository
        git clone https://github.com/ichsanyudika/RealSense_ORT.git
        
        # 2. Build the project
        cd RealSense_ORT
        mkdir build && cd build
        cmake ..
        make
        
        # 3. Run the application
        ./cam

### Results

![](output/output.png)

