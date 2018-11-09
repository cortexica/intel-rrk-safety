#pragma once

#include <opencv/cv.hpp>

namespace Json
{
class Value;
}

/**
 * @brief The PPE class.
 */
class PPE
{
public:
    /**
     * @brief PPE
     */
    PPE();

    /**
     * @brief Main loop
     * @return - EXIT_SUCCESS or EXIT_FAILURE
     */
    int run() const;

private:
    /**
     * @brief Sends a multipart form data request to the REST API of PPE service.
     * @param frame - image to be used for the inference
     * @param continuous - specifies whether this request is for continuous mode
     * @return - JSON containing query results
     */
    Json::Value sendRequest(const cv::Mat& frame, const bool continuous) const;

    /**
     * @brief Draws results.
     * @param results - JSON containing results
     * @param frame - image on which to draw
     */
    void drawResults(const Json::Value& results, cv::Mat& frame) const;

    /**
     * @brief Callback for trackbars.
     */
    static void onTrackbar(int, void*);

    const std::string m_windowName;
    static int        m_ppeThreshold;
    static int        m_faceThreshold;
    static int        m_personThreshold;
    static int        m_bodyPartsThreshold;
};
