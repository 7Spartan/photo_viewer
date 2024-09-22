#pragma once
#ifndef ZOOMHANDLER_H
#define ZOOMHANDLER_H

#include <opencv2/opencv.hpp>

class ZoomHandler {
public:
    ZoomHandler(double zoomFactor = 1.0);

    void zoomIn(cv::Point2f center);
    void zoomOut(cv::Point2f center);
    cv::Mat applyZoom(const cv::Mat& image) const;
    cv::Point2f getZoomCenter() const;

private:
    double zoomFactor;
    cv::Point2f zoomCenter;
};

#endif // ZOOMHANDLER_H