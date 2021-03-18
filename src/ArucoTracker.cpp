#include "ArucoTracker.hpp"

#include <algorithm>
#include <iostream>
#include <opencv2/core.hpp>

namespace amt {

ArucoTracker::ArucoTracker(std::vector<std::shared_ptr<AMarker>> markers,
                           float marker_size, std::string camera_calib_file)
    : m_markers(markers),
      m_marker_size(marker_size),
      m_dictionary(
          cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL)) {
    for (auto marker : markers) {
        this->m_marker_ids.push_back(marker->get_id());
    }
    // set detection parameters like theshold, mind marker size and so on
    this->m_parameters = cv::aruco::DetectorParameters::create();

    // read camera calibration file
    auto fs = cv::FileStorage(camera_calib_file, cv::FileStorage::READ);
    if (fs.isOpened()) {
        fs["cameraMatrix"] >> this->m_camera_matrix;
        fs["dist_coeffs"] >> this->m_dist_coeffs;
    } else {
        std::cout
            << "Camera callibration could not be loaded. Results may suffer."
            << std::endl;
    }
}

auto ArucoTracker::marker_by_id(int32_t id) {
    return std::find_if(std::begin(this->m_markers), std::end(this->m_markers),
                        [id](auto marker) { return marker->get_id() == id; });
}

void ArucoTracker::add_marker(std::shared_ptr<AMarker> marker) {
    this->m_markers.push_back(marker);
    this->m_marker_ids.push_back(marker->get_id());
}

void ArucoTracker::remove_marker(int32_t id) {
    auto to_remove =
        std::find_if(std::begin(this->m_markers), std::end(this->m_markers),
                     [id](auto marker) { return marker->get_id() == id; });

    this->m_markers.erase(to_remove);
}

void ArucoTracker::get_markers(std::vector<cv::Mat>& markers_out, int size) {
    static int border = 1;

    for (auto marker : this->m_marker_ids) {
        cv::Mat marker_image;
        cv::aruco::drawMarker(this->m_dictionary, marker, size, marker_image,
                              border);
        markers_out.push_back(marker_image);
    }
}

void ArucoTracker::update(cv::Mat image) {
    // init variables for results
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    std::vector<int32_t> detected_marker_ids;

    // detect markers
    cv::aruco::detectMarkers(image, this->m_dictionary, markerCorners,
                             detected_marker_ids, this->m_parameters,
                             rejectedCandidates);

    std::vector<int32_t> markers_of_interest;

    for (auto marker_id : detected_marker_ids) {
        if (std::find(std::begin(this->m_marker_ids),
                      std::end(this->m_marker_ids),
                      marker_id) != this->m_marker_ids.end()) {
            markers_of_interest.push_back(marker_id);
        }
    }

    cv::Mat outputImage = image.clone();

    // get orientation and translation of each marker, average them out and
    // display result (+draw for debug)
    if (markers_of_interest.size() > 0) {
        std::vector<cv::Vec3d> object_points;
        std::vector<cv::Vec3d> rotations;
        std::vector<cv::Vec3d> translations;

        // get rotations and translations of all markers
        // TODO: filter corners, too, to improve performance
        cv::aruco::estimatePoseSingleMarkers(
            markerCorners, this->m_marker_size, this->m_camera_matrix,
            this->m_dist_coeffs, rotations, translations, object_points);

        for (uint64_t marker = 0; marker < markers_of_interest.size();
             marker++) {
            int32_t curr_marker_id = markers_of_interest[marker];

            auto curr_marker = this->marker_by_id(curr_marker_id);

            // sanity check. should never happen, but just in case
            if (curr_marker != this->m_markers.end()) {
                auto tracked_before = std::find(
                    std::begin(this->m_last_tracked_ids),
                    std::end(this->m_last_tracked_ids), curr_marker_id);

                // if this marker was not tracked last frame, call the on_appear
                // method if it was tracked, call the on_present method
                if (tracked_before == this->m_last_tracked_ids.end()) {
                    (*curr_marker)
                        ->on_appear(rotations[marker], translations[marker]);
                } else {
                    // remove the checked this marker from the ones tracked last
                    // frame. all markers remaining were tracked last frame but
                    // not this one.
                    this->m_last_tracked_ids.erase(tracked_before);
                    (*curr_marker)
                        ->on_present(rotations[marker], translations[marker]);
                }
            }

            cv::aruco::drawAxis(outputImage, this->m_camera_matrix,
                                this->m_dist_coeffs, rotations[marker],
                                translations[marker], 0.01);

            // iterate over all markers tracked last frame but not this one
            // and call on_disappear method
            for (int marker_id : this->m_last_tracked_ids) {
                auto marker = this->marker_by_id(marker_id);
                if (marker != this->m_markers.end()) {
                    (*marker)->on_disappear();
                }
            }

            this->m_last_tracked_ids.clear();
            std::copy(markers_of_interest.begin(), markers_of_interest.end(),
                      back_inserter(this->m_last_tracked_ids));
        }
    }

    // draw image
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners,
                                   detected_marker_ids);

    // draw result
    cv::imshow("Aruco Marker Tracker", outputImage);
}

}  // namespace amt
