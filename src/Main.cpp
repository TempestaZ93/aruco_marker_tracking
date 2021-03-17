#include <iostream>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>    

#include "ArucoTracker.hpp"
#include "data/CSVBackend.hpp"

void callback(int id) { std::cout << id << " detected" << std::endl; }

int main() {
    int id = 1;
    cv::VideoCapture cam(id);
    // try opening camera
    while (!cam.isOpened()) {
        std::cout << "Failed to make connection to cam" << std::endl;
        cam.open(id++);
    }
    
    // create database backend
    std::shared_ptr<amt::CSVBackend> backend =
        std::make_shared<amt::CSVBackend>("testfile");

    // create tracker
    std::vector<int> marker_ids = {757, 787, 827, 863, 971};
    amt::ArucoTracker tracker(
        marker_ids, {callback, callback, callback, callback, callback},
        0.03f, "../res/cameraParameters.xml", 
        backend);

    // create marker images
    std::vector<cv::Mat> markers;
    tracker.get_markers(markers, 200);

    // save marker images
    for (uint64_t marker = 0; marker < marker_ids.size(); marker++) {
        std::stringstream ss("");

        ss << "./marker_" << marker_ids[marker] << ".png";

        std::cout << ss.str() << std::endl;
        cv::imwrite(ss.str(), markers[marker]);
    }

    // run main loop until <ESC> is pressed
    cv::Mat image;
    while(cv::waitKey(1) != 27) {
        // get camera image
        cam >> image;
        // update aruco tracker
        tracker.update(image);
    }

    return 0;
}
