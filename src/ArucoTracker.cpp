#include "ArucoTracker.hpp"
#include <iostream>
#include <algorithm>
#include <opencv2/core.hpp>

namespace amt {

ArucoTracker::ArucoTracker(const std::vector<int32_t>& ids,
                           const std::vector<Callback_t>& callbacks,
                           float real_size,
                           std::string camera_calib_file,
                           std::shared_ptr<ADataBackend> backend)
    : m_marker_ids(ids),
      m_callbacks(callbacks),
      m_real_size(real_size),
      m_dictionary(
          cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL)) {
    this->m_data_access.reset(new DataAccess(backend));
    
    // set detection parameters like theshold, mind marker size and so on
    this->m_parameters = cv::aruco::DetectorParameters::create();
    
    this->m_moving_median_length = 10;

    auto fs = cv::FileStorage(camera_calib_file, cv::FileStorage::READ);
    std::cout << "real size: " << this->m_real_size << std::endl;
    if(fs.isOpened()) {
        fs["cameraMatrix"] >> this->m_camera_matrix;
        fs["dist_coeffs"] >> this->m_dist_coeffs;
    } else {
        std::cout << "Camera callibration could not be loaded. Results may suffer." << std::endl;
    }

}

void ArucoTracker::set_callback(int32_t id, Callback_t callback) {
    auto iter =
        std::find(this->m_marker_ids.begin(), this->m_marker_ids.end(), id);

    if (iter == this->m_marker_ids.end()) {
        this->m_marker_ids.push_back(id);
        this->m_callbacks.push_back(callback);
    } else {
        auto callback_iter =
            this->m_callbacks.begin() + (iter - this->m_marker_ids.begin());
        this->m_callbacks.erase(callback_iter);
        this->m_callbacks.insert(callback_iter, callback);
    }
}

void ArucoTracker::remove_callback(int32_t id) {
    auto iter =
        std::find(this->m_marker_ids.begin(), this->m_marker_ids.end(), id);

    if (iter == this->m_marker_ids.end()) {
        // probably put some kind of error here
    } else {
        auto callback_iter =
            this->m_callbacks.begin() + (iter - this->m_marker_ids.begin());
        this->m_marker_ids.erase(iter);
        this->m_callbacks.erase(callback_iter);
    }
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
    
    // call callbacks for detected markers
    for (uint64_t marker = 0; marker < markerCorners.size(); marker++) {
        auto iter = std::find(this->m_marker_ids.begin(), this->m_marker_ids.end(), detected_marker_ids[marker]);
        if (iter == this->m_marker_ids.end()) {
            continue;
        }
        int index = iter - this->m_marker_ids.begin();
        this->m_callbacks[index](detected_marker_ids[marker]);
    }

    cv::Mat outputImage = image.clone();
    
    // get orientation and translation of each marker, average them out and display result (+draw for debug)
    if(markerCorners.size() > 0) {
        std::vector<cv::Vec3d> object_points;
        std::vector<cv::Vec3d> rotations;
        std::vector<cv::Vec3d> translations;

        // get rotations and translations of all markers
        cv::aruco::estimatePoseSingleMarkers(markerCorners, this->m_real_size, this->m_camera_matrix, this->m_dist_coeffs, rotations, translations, object_points);
        
        for (uint64_t marker = 0; marker < markerCorners.size(); marker++) {  
            cv::aruco::drawAxis(outputImage, this->m_camera_matrix, this->m_dist_coeffs, rotations[marker], translations[marker], 0.01);
        }
    }

    // draw image
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners,
                                   detected_marker_ids);

    // draw result
    cv::imshow("Aruco Marker Tracker", outputImage);
}

}  // namespace amt
