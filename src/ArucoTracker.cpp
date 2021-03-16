#include "ArucoTracker.hpp"

#include <algorithm>

namespace amt {

ArucoTracker::ArucoTracker(const std::vector<int32_t>& ids,
                           const std::vector<Callback_t>& callbacks,
                           std::shared_ptr<ADataBackend> backend)
    : m_marker_ids(ids),
      m_callbacks(callbacks),
      m_dictionary(
          cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL)) {
    this->m_data_access.reset(new DataAccess(backend));
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

    // set detection parameters like theshold, mind marker size and so on
    cv::Ptr<cv::aruco::DetectorParameters> parameters =
        cv::aruco::DetectorParameters::create();

    // actually detect markers
    cv::aruco::detectMarkers(image, this->m_dictionary, markerCorners,
                             this->m_marker_ids, parameters,
                             rejectedCandidates);

    // draw image
    cv::Mat outputImage = image.clone();
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners,
                                   this->m_marker_ids);
    cv::imshow("Aruco Marker Tracker", outputImage);
}

}  // namespace amt
