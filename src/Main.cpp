#include <string.h>

#include <cstdlib>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <sstream>

#include "ArucoTracker.hpp"
#include "CubeMarker.hpp"
#include "PyramidMarker.hpp"
#include "data/CSVBackend.hpp"

int handle_arguments(int argc, char* argv[], std::string& calibration_file,
                     float& marker_size, bool& save_marker_images);

int main(int argc, char* argv[]) {
    std::string calibration_file;
    float marker_size = 0.0f;
    bool save_marker_images = false;

    int success = handle_arguments(argc, argv, calibration_file, marker_size,
                                   save_marker_images);
    if (!success) {
        return 0;
    }

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

    std::vector<std::shared_ptr<amt::AMarker>> markers = {
        std::make_shared<amt::CubeMarker>(757, "Cube Marker", marker_size),
        std::make_shared<amt::PyramidMarker>(971, "Pyramid Marker", marker_size,
                                             1)};

    // create tracker
    amt::ArucoTracker tracker(markers, marker_size, calibration_file);

    if (save_marker_images) {
        // create marker images
        std::vector<cv::Mat> marker_images;
        tracker.get_markers(marker_images, 200);

        // save marker images
        for (uint64_t marker = 0; marker < markers.size(); marker++) {
            std::stringstream ss("");

            ss << "./marker_" << (markers[marker])->get_name() << ".png";

            std::cout << ss.str() << std::endl;
            cv::imwrite(ss.str(), marker_images[marker]);
        }
    }

    // run main loop until <ESC> is pressed
    cv::Mat image;
    while (cv::waitKey(1) != 27) {
        // get camera image
        cam >> image;
        // update aruco tracker
        tracker.update(image);
    }

    return 0;
}

bool is_argument_valid(int argc, int arg, char* argv[],
                       const char* short_command, const char* long_command,
                       bool has_value) {
    return (strcmp(argv[arg], short_command) == 0 ||
            strcmp(argv[arg], long_command) == 0) &&
           (!has_value || arg < argc - 1);
}

int handle_arguments(int argc, char* argv[], std::string& calibration_file,
                     float& marker_size, bool& save_marker_images) {
    std::string help = R"(ArUco marker tracker

key | verbose key          <value>                                   (default)                 : description
------------------------------------------------------------------------------------------------------------
-c  | --calibration_file   <path/to/opencv/calibration-file>         (./cameraCalibration.xml) : if present, you can link a opencv camera calibration file to improve performance

-s  | --marker_size        <decimal number in meters>                (0.03)                    : specify size of markers (all must be the same size)

-i  | --save_marker_images <off|on>                                  (off)                     : should the program save images of selected marker ids to drive (same directory as executable)

-h  | --help               <>                                        ()                        : display this help menu
    )";

    // init default values
    calibration_file = "./cameraCalibration.xml";
    marker_size = 0.05f;
    save_marker_images = false;

    for (int arg = 1; arg < argc; arg++) {
        if (is_argument_valid(argc, arg, argv, "-c", "--calibration-file",
                              true)) {
            calibration_file = argv[++arg];
        } else if (is_argument_valid(argc, arg, argv, "-s", "--marker-size",
                                     true)) {
            marker_size = std::atof(argv[++arg]);
        } else if (is_argument_valid(argc, arg, argv, "-i",
                                     "--save-marker-images", false)) {
            save_marker_images = true;
        } else if (is_argument_valid(argc, arg, argv, "-h", "--help", false)) {
            std::cout << help << "\n\n";
            return 0;
        } else {
            std::cout << "Command '" << argv[arg]
                      << "' not recognized. Please consult help with -h.\n";
            return 0;
        }
    }
    return 1;
}
