#include <iostream>
#include <sstream>

#include "ArucoTracker.hpp"
#include "data/CSVBackend.hpp"

void callback(int id) { std::cout << id << std::endl; }

int main(int argc, char* argv[]) {
    std::cout << "Hello World!" << std::endl;

    std::shared_ptr<amt::CSVBackend> backend =
        std::make_shared<amt::CSVBackend>("testfile");

    std::vector<int> marker_ids = {1, 2, 3, 4, 5};
    amt::ArucoTracker tracker(
        marker_ids, {callback, callback, callback, callback, callback},
        backend);

    std::vector<cv::Mat> markers;
    tracker.get_markers(markers, 200);

    for (int marker = 0; marker < marker_ids.size(); marker++) {
        std::stringstream ss("");

        ss << "./marker_" << marker_ids[marker] << ".png";

        std::cout << ss.str() << std::endl;
        cv::imwrite(ss.str(), markers[marker]);
    }

    return 0;
}
