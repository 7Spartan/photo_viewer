#include "ZoomHandler.h"

ZoomHandler::ZoomHandler(double zoomFactor):zoomFactor(zoomFactor){}

void ZoomHandler::zoomIn(cv::Point2f center) {
	zoomCenter = center;
	zoomFactor *= 1.2; //Increase zoom factor by 20%
	std::cout << "\nzoomin-"<<zoomFactor;
	std::cout << "\n" << center;
}

void ZoomHandler::zoomOut(cv::Point2f center) {
	zoomCenter = center;
	zoomFactor /= 1.2; //Decrease zoom factor by 20%
	std::cout << "\nzoomout-"<<zoomFactor;
	std::cout << "\n" << center;
}

cv::Mat ZoomHandler::applyZoom(const cv::Mat& image) const {
	cv::Mat zoomedImage;
	cv::resize(image, zoomedImage, cv::Size(), zoomFactor, zoomFactor);
	return zoomedImage;
}

cv::Point2f ZoomHandler::getZoomCenter() const {
	return zoomCenter;
}