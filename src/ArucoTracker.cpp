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
        fs["cameraMatrix"] >> this->m_camera.camera_matrix;
        fs["dist_coeffs"] >> this->m_camera.distortion_coeffs;
    } else {
        std::cout
            << "Camera callibration could not be loaded. Results may suffer."
            << std::endl;
    }
}

std::vector<std::shared_ptr<AMarker>> ArucoTracker::markers_by_id(int32_t id) {
    std::vector<std::shared_ptr<AMarker>> matches;

    for (auto marker : this->m_markers) {
        if (marker->get_id() == id) {
            matches.push_back(marker);
        }
    }
    return matches;
}

void ArucoTracker::add_marker(std::shared_ptr<AMarker> marker) {
    this->m_markers.push_back(marker);
    this->m_marker_ids.push_back(marker->get_id());
}

void ArucoTracker::remove_marker(std::string name) {
    // look if the marker is currently tracked
    auto to_remove =
        std::find_if(std::begin(this->m_markers), std::end(this->m_markers),
                     [id](auto marker) { return marker->get_name() == name; });

    // if the marker is currently tracked, remove it
    if (to_remove != this->m_markers.end()) {
        // if the marker is tracked
        this->m_marker_ids.erase(std::begin(this->m_marker_ids) +
                                 (to_remove - std::begin(this->m_markers)));
        this->m_markers.erase(to_remove);
    }
}

void ArucoTracker::get_markers(std::vector<cv::Mat>& markers_out, int size) {
    // specify number of border bits around the actual code (default is one)
    // extra specific for later use
    static int border = 1;

    // loop over all marker ids and create the image for this marker id
    // all markers are also dependent on the dictionary used
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

    // let OpenCV find all makers currently visible
    cv::aruco::detectMarkers(image, this->m_dictionary, markerCorners,
                             detected_marker_ids, this->m_parameters,
                             rejectedCandidates);

    // filter markers only to include markers that were specified
    std::vector<int32_t> markers_of_interest;
    std::vector<std::vector<cv::Point2f>> marker_corners_of_interest;
    for (int marker = 0; marker < detected_marker_ids.size(); marker++) {
        int marker_id = detected_marker_ids[marker];
        if (std::find(std::begin(this->m_marker_ids),
                      std::end(this->m_marker_ids),
                      marker_id) != this->m_marker_ids.end()) {
            markers_of_interest.push_back(marker_id);
            marker_corners_of_interest.push_back(markerCorners[marker]);
        }
    }
    // create copy of query image do draw on
    cv::Mat output_image = image.clone();

    // get orientation and translation of each marker, average them out and
    // display result (+draw for debug)
    if (markers_of_interest.size() > 0) {
        std::vector<cv::Vec3d> object_points;
        std::vector<cv::Vec3d> rotations;
        std::vector<cv::Vec3d> translations;

        // get rotations and translations of all markers of interest
        cv::aruco::estimatePoseSingleMarkers(
            marker_corners_of_interest, this->m_marker_size,
            this->m_camera.camera_matrix, this->m_camera.distortion_coeffs,
            rotations, translations, object_points);

        for (uint64_t marker = 0; marker < markers_of_interest.size();
             marker++) {
            int32_t curr_marker_id = markers_of_interest[marker];

            auto curr_markers = this->markers_by_id(curr_marker_id);

            bool was_tracked_before = false;
            // Look if this marker was tracked last frame
            auto tracked_before =
                std::find(std::begin(this->m_last_tracked_ids),
                          std::end(this->m_last_tracked_ids), curr_marker_id);

            // if this marker was not tracked last frame, call the on_appear
            // method if it was tracked, call the on_present method
            if (tracked_before != this->m_last_tracked_ids.end()) {
                for (auto curr_marker : curr_markers) {
                    curr_marker->on_present(rotations[marker],
                                            translations[marker], m_camera,
                                            output_image);
                }
                this->m_last_tracked_ids.erase(tracked_before);
            } else {
                for (auto curr_marker : curr_markers) {
                    curr_marker->on_appear(rotations[marker],
                                           translations[marker], this->m_camera,
                                           output_image);
                }
            }

            // draw x, y, and z axis in 3d space for all markres of interest
            // length = 0.01m
            cv::aruco::drawAxis(output_image, this->m_camera.camera_matrix,
                                this->m_dist_coeffs, rotations[marker],
                                translations[marker], 0.01);
        }
    }

    // iterate over all markers tracked last frame but not this one
    // and call on_disappear method
    for (int marker_id : this->m_last_tracked_ids) {
        auto markers = this->markers_by_id(marker_id);
        for (auto marker : markers) {
            marker->on_disappear(output_image);
        }
    }
    // clear markers from last frame and replace them with the ones detected
    // this frame
    this->m_last_tracked_ids.clear();
    std::copy(markers_of_interest.begin(), markers_of_interest.end(),
              back_inserter(this->m_last_tracked_ids));

    if (marker_corners_of_interest.size() > 0) {
        // draw image
        cv::aruco::drawDetectedMarkers(output_image, marker_corners_of_interest,
                                       markers_of_interest);
    }

    // draw result
    cv::imshow("Aruco Marker Tracker", output_image);
}

}  // namespace amt
