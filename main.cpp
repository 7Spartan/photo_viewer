#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include "ZoomHandler.h"

namespace fs = std::filesystem;

// Function to list all JPG files in the given folder
std::vector<std::string> listImages(const std::string& folderPath) {
    std::vector<std::string> images;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        std::cout << entry.path();
        if (entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg" || entry.path().extension() == ".JPEG" || entry.path().extension() == ".JPG") {
            images.push_back(entry.path().string());
        }
    }
    return images;
}

cv::Mat addPadding(const cv::Mat& image, int targetWidth, int targetHeight) {
    int originalWidth = image.cols;
    int originalHeight = image.rows;
    std::cout << "\noriginal: " << originalWidth << "x" << originalHeight;
    double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
    int newWidth, newHeight;
    std::cout << "\naspect: " << aspectRatio;
    // Determine new dimensions based on aspect ratio
    if (originalWidth > originalHeight) {
        newWidth = std::min(targetWidth,originalWidth);
        newHeight = static_cast<int>(round(newWidth / aspectRatio));
    }
    else {
        newHeight = std::min(targetHeight, originalHeight);
        newWidth = static_cast<int>(round(newHeight * aspectRatio));
    }
    std::cout << "\nnewsize: " << newWidth << "x" << newHeight;
    // Create a black canvas with the target dimensions
    cv::Mat paddedImage = cv::Mat::zeros(targetHeight, targetWidth, image.type());

    // Calculate top-left corner position for the original image
    int xOffset = (targetWidth - newWidth) / 2;
    int yOffset = (targetHeight - newHeight) / 2;
    std::cout << "\noffset: " << xOffset << ", " << yOffset;
    // Debugging information
    std::cout << "\npaddedimage: " << paddedImage.cols << "x" << paddedImage.rows;
    
    // Ensure the rectangle does not go out of bounds
    if (newWidth > 0 && newHeight > 0) {
        image.copyTo(paddedImage(cv::Rect(xOffset, yOffset, newWidth, newHeight)));
    }
    else {
        std::cerr << "Error: Invalid newWidth or newHeight.\n";
    }

    return paddedImage;
}

cv::Mat resizeImageToFit(const cv::Mat& image, int targetWidth, int targetHeight) {
    int originalWidth = image.cols;
    int originalHeight = image.rows;

    double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
    int newWidth, newHeight;

    // Calculate the new dimensions while preserving the aspect ratio
    if (originalWidth > originalHeight) {
        newWidth = targetWidth;
        newHeight = static_cast<int>(targetWidth / aspectRatio);
    }
    else {
        newHeight = targetHeight;
        newWidth = static_cast<int>(targetHeight * aspectRatio);
    }

    // Resize the image
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(newWidth, newHeight));

    return resizedImage;
}

ZoomHandler zoomHandler; // Instantiate ZoomHandler

void onMouse(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        cv::Point2f center(static_cast<float>(x), static_cast<float>(y));
        zoomHandler.zoomIn(center);
    }
    else if (event == cv::EVENT_RBUTTONDOWN) {
        cv::Point2f center(static_cast<float>(x), static_cast<float>(y));
        zoomHandler.zoomOut(center);
    }
}


int main() {
    std::string folderPath;
    std::cout << "Enter the folder path: ";
    std::cin >> folderPath;

    std::vector<std::string> imageFiles = listImages(folderPath);
    if (imageFiles.empty()) {
        std::cout << "No images found in the folder.\n";
        return -1;
    }

    int currentIndex = 0;
    const int targetWidth = 8000;
    const int targetHeight = 8000;
    cv::namedWindow("Photo viewer", cv::WINDOW_NORMAL);
    cv::resizeWindow("Photo viewer", targetWidth, targetHeight);

    cv::setMouseCallback("Photo viewer", onMouse, nullptr);
    while (true) {
        cv::Mat image = cv::imread(imageFiles[currentIndex]);
        if (image.empty()) {
            std::cout << "Failed to load image.\n";
            return -1;
        }

        // Apply zoom to the image
        cv::Mat zoomedImage = zoomHandler.applyZoom(image);
        std::cout << "\n zoomedimage: " << zoomedImage.cols << "-" << zoomedImage.rows;

        // Create a black canvas to display the zoomed image centered
        cv::Mat canvas(targetHeight, targetWidth, CV_8UC3, cv::Scalar(0, 0, 0));

        // Get the current zoom center and adjust the display position
        cv::Point2f center = zoomHandler.getZoomCenter();

        // Calculate offsets and ensure the image fits within the canvas
        int xOffset = std::max(0, (int)std::round((targetWidth - zoomedImage.cols) / 2));
        int yOffset = std::max(0, (int)std::round((targetHeight - zoomedImage.rows) / 2));
        
        int new_width = std::min(zoomedImage.cols, targetWidth - xOffset);
        int new_height = std::min(zoomedImage.rows, targetHeight - yOffset);

        // Ensure we don't copy outside the canvas
        cv::Rect roi(xOffset, yOffset,new_width,new_height);
        cv::Rect roi1(0, 0, new_width, new_height);
        std::cout << "\n roi: " << roi << "\n";
        if (zoomedImage.rows <= targetHeight && zoomedImage.cols <= targetWidth) {
            zoomedImage.copyTo(canvas(roi));
        }
        else {
            zoomedImage(roi1).copyTo(canvas(roi)); // Copy zoomed image to canvas
        }

        cv::imshow("Photo viewer", canvas);
        char key = static_cast<char>(cv::waitKey(1));

        // Navigation controls
        if (key == 'n') { // Next image
            currentIndex = (currentIndex + 1) % imageFiles.size();
            //zoomHandler.zoomFactor = 1.0;
            zoomHandler = ZoomHandler(); // Reset zoom
        }
        else if (key == 'p') { // Previous image
            currentIndex = (currentIndex - 1 + imageFiles.size()) % imageFiles.size();
            //zoomHandler.zoomFactor = 1.0;
            zoomHandler = ZoomHandler(); // Reset zoom
        }
        /*else if (key == 'z') {
            zoomHandler.zoomFactor *= 1.2;
        }
        else if (key == 'x') {
            zoomHandler.zoomFactor /= 1.2;
        }*/
        else if (key == 27) { // Escape key to exit
            break;
        }
    }

    cv::destroyAllWindows(); // Clean up windows

    return 0;
}
