#include "../hpp/operations.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath> // For std::abs
#include <numeric> // For std::accumulate
#include <limits> // For std::numeric_limits

cv::Mat BrightnessOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& params) {
    // get brightness factor from parameters (default to 1.0 if not specified)
    double factor = 1.0;
    auto it = params.find("factor");
    if (it != params.end()) {
        factor = it->second;
    }
    
    // extract ROI from input image
    cv::Mat roi_image = ROITools::extractROI(input, roi);
    cv::Mat output;
    
    // convert to float for processing
    cv::Mat float_img;
    roi_image.convertTo(float_img, CV_32F);
    
    // apply brightness adjustment
    float_img *= factor;
    
    // convert back to original type
    float_img.convertTo(output, roi_image.type());
    
    // ensure values are clamped to valid range
    cv::threshold(output, output, 255, 255, cv::THRESH_TRUNC);
    cv::threshold(output, output, 0, 0, cv::THRESH_TOZERO);
    
    // apply the processed ROI back to the original image
    return ROITools::applyROI(input, output, roi);
}

std::string BrightnessOperation::getNameImpl() const {
    return "brightness";
}

bool BrightnessOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // check brightness factor
    if (parameters.count("factor")) {
        double factor = parameters.at("factor");
        if (factor < 0.0 || factor > 5.0) {
            std::cerr << "error: brightness factor must be between 0.0 and 5.0" << std::endl;
            return false;
        }
    }
    
    return true;
}

cv::Mat BlurOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& params) {
    // get parameters with defaults
    int kernel_size = 5;
    double sigma = 1.0;
    
    auto kernel_it = params.find("kernel_size");
    if (kernel_it != params.end()) {
        kernel_size = static_cast<int>(kernel_it->second);
    }
    
    auto sigma_it = params.find("sigma");
    if (sigma_it != params.end()) {
        sigma = sigma_it->second;
    }
    
    // ensure kernel size is odd
    if (kernel_size % 2 == 0) {
        kernel_size += 1;
    }
    
    // extract ROI from input image
    cv::Mat roi_image = ROITools::extractROI(input, roi);
    cv::Mat output;
    
    // apply Gaussian blur
    cv::GaussianBlur(roi_image, output, cv::Size(kernel_size, kernel_size), sigma);
    
    // apply the processed ROI back to the original image
    return ROITools::applyROI(input, output, roi);
}

std::string BlurOperation::getNameImpl() const {
    return "blur";
}

bool BlurOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // check kernel size
    if (parameters.count("kernel_size")) {
        double kernel_size = parameters.at("kernel_size");
        if (kernel_size < 3 || kernel_size > 31) {
            std::cerr << "error: blur kernel size must be between 3 and 31" << std::endl;
            return false;
        }
    }
    
    // check sigma
    if (parameters.count("sigma")) {
        double sigma = parameters.at("sigma");
        if (sigma < 0.1 || sigma > 10.0) {
            std::cerr << "error: blur sigma must be between 0.1 and 10.0" << std::endl;
            return false;
        }
    }
    
    return true;
}

cv::Mat ContrastOperation::executeImpl(const cv::Mat& image, const ROI& roi, const std::map<std::string, double>& parameters) {
    // get parameters with defaults
    double factor = 1.0;
    double brightness_offset = 0.0;
    
    auto factor_it = parameters.find("factor");
    if (factor_it != parameters.end()) {
        factor = factor_it->second;
    }
    
    auto offset_it = parameters.find("brightness_offset");
    if (offset_it != parameters.end()) {
        brightness_offset = offset_it->second;
    }
    
    // extract ROI from input image
    cv::Mat roi_image = ROITools::extractROI(image, roi);
    cv::Mat output;
    
    // apply contrast and brightness adjustment
    roi_image.convertTo(output, -1, factor, brightness_offset);
    
    // apply the processed ROI back to the original image
    return ROITools::applyROI(image, output, roi);
}

std::string ContrastOperation::getNameImpl() const {
    return "contrast";
}

bool ContrastOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // check contrast factor
    if (parameters.count("factor")) {
        double factor = parameters.at("factor");
        if (factor < 0.0 || factor > 3.0) {
            std::cerr << "error: contrast factor must be between 0.0 and 3.0" << std::endl;
            return false;
        }
    }
    
    // check brightness offset
    if (parameters.count("brightness_offset")) {
        double offset = parameters.at("brightness_offset");
        if (offset < -100.0 || offset > 100.0) {
            std::cerr << "error: brightness offset must be between -100 and 100" << std::endl;
            return false;
        }
    }
    
    return true;
}

cv::Mat CropOperation::executeImpl(const cv::Mat& image, const ROI& roi, const std::map<std::string, double>& parameters) {
    // get crop parameters with defaults
    int x = 0;
    int y = 0;
    int width = image.cols;
    int height = image.rows;
    
    auto x_it = parameters.find("x");
    if (x_it != parameters.end()) {
        x = static_cast<int>(x_it->second);
    }
    
    auto y_it = parameters.find("y");
    if (y_it != parameters.end()) {
        y = static_cast<int>(y_it->second);
    }
    
    auto width_it = parameters.find("width");
    if (width_it != parameters.end()) {
        width = static_cast<int>(width_it->second);
    } else {
        width = image.cols - x;
    }
    
    auto height_it = parameters.find("height");
    if (height_it != parameters.end()) {
        height = static_cast<int>(height_it->second);
    } else {
        height = image.rows - y;
    }
    
    // validate crop region
    if (x < 0 || y < 0 || x >= image.cols || y >= image.rows) {
        std::cerr << "error: crop coordinates out of bounds" << std::endl;
        return image.clone();
    }
    
    if (width <= 0 || height <= 0 || x + width > image.cols || y + height > image.rows) {
        std::cerr << "error: crop dimensions invalid" << std::endl;
        return image.clone();
    }
    
    // create crop region
    cv::Rect crop_region(x, y, width, height);
    
    // perform crop
    cv::Mat cropped = image(crop_region).clone();
    
    return cropped;
}

std::string CropOperation::getNameImpl() const {
    return "crop";
}

bool CropOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // check for required parameters
    if (parameters.count("x") && parameters.at("x") < 0) {
        std::cerr << "error: crop x coordinate must be non-negative" << std::endl;
        return false;
    }
    
    if (parameters.count("y") && parameters.at("y") < 0) {
        std::cerr << "error: crop y coordinate must be non-negative" << std::endl;
        return false;
    }
    
    if (parameters.count("width") && parameters.at("width") <= 0) {
        std::cerr << "error: crop width must be positive" << std::endl;
        return false;
    }
    
    if (parameters.count("height") && parameters.at("height") <= 0) {
        std::cerr << "error: crop height must be positive" << std::endl;
        return false;
    }
    
    return true;
}

cv::Mat SharpenOperation::executeImpl(const cv::Mat& image, const ROI& roi, const std::map<std::string, double>& parameters) {
    // get parameters with defaults
    double strength = 1.0;
    int kernel_size = 5;
    
    auto strength_it = parameters.find("strength");
    if (strength_it != parameters.end()) {
        strength = strength_it->second;
    }
    
    auto kernel_it = parameters.find("kernel_size");
    if (kernel_it != parameters.end()) {
        kernel_size = static_cast<int>(kernel_it->second);
    }
    
    // ensure kernel size is odd
    if (kernel_size % 2 == 0) {
        kernel_size++;
    }
    
    // extract roi from input image
    cv::Mat roi_image = ROITools::extractROI(image, roi);
    cv::Mat output;
    
    // create unsharp mask
    cv::Mat blurred;
    cv::GaussianBlur(roi_image, blurred, cv::Size(kernel_size, kernel_size), 0);
    
    // apply unsharp mask
    cv::addWeighted(roi_image, 1.0 + strength, blurred, -strength, 0, output);
    
    // apply the processed roi back to the original image
    return ROITools::applyROI(image, output, roi);
}

std::string SharpenOperation::getNameImpl() const {
    return "sharpen";
}

bool SharpenOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // check strength parameter
    if (parameters.count("strength")) {
        double strength = parameters.at("strength");
        if (strength < 0.0 || strength > 2.0) {
            std::cerr << "error: sharpen strength must be between 0.0 and 2.0" << std::endl;
            return false;
        }
    }
    
    // check kernel size parameter
    if (parameters.count("kernel_size")) {
        double kernel_size = parameters.at("kernel_size");
        if (kernel_size < 3 || kernel_size > 15) {
            std::cerr << "error: sharpen kernel size must be between 3 and 15" << std::endl;
            return false;
        }
    }
    
    return true;
} 

cv::Mat EdgeCountOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) {
    // extract ROI from input image
    cv::Mat roi_image = ROITools::extractROI(input, roi);
    
    // convert to grayscale for edge detection
    cv::Mat gray;
    if (roi_image.channels() == 3) {
        cv::cvtColor(roi_image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = roi_image.clone();
    }
    
    // detect edges using Canny
    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150);
    
    // count edge pixels
    int edge_pixels = cv::countNonZero(edges);
    int total_pixels = edges.rows * edges.cols;
    double edge_density = static_cast<double>(edge_pixels) / total_pixels;
    
    // calculate edge strength using Sobel
    cv::Mat grad_x, grad_y, grad_magnitude;
    cv::Sobel(gray, grad_x, CV_64F, 1, 0, 3);
    cv::Sobel(gray, grad_y, CV_64F, 0, 1, 3);
    cv::magnitude(grad_x, grad_y, grad_magnitude);
    
    double avg_edge_strength = cv::mean(grad_magnitude)[0];
    
    // print analysis results
    std::cout << "=== EDGE COUNT ANALYSIS ===" << std::endl;
    std::cout << "Edge pixels: " << edge_pixels << std::endl;
    std::cout << "Total pixels: " << total_pixels << std::endl;
    std::cout << "Edge density: " << std::fixed << std::setprecision(4) << edge_density << " (" << (edge_density * 100) << "%)" << std::endl;
    std::cout << "Average edge strength: " << std::fixed << std::setprecision(2) << avg_edge_strength << std::endl;
    std::cout << "==========================" << std::endl;
    
    // return the original image unchanged
    return input.clone();
}

std::string EdgeCountOperation::getNameImpl() const {
    return "edge_count";
}

bool EdgeCountOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // no parameters to validate for edge count operation
    return true;
}

cv::Mat BlurDetectionOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) {
    // extract ROI from input image
    cv::Mat roi_image = ROITools::extractROI(input, roi);

    // convert to grayscale if needed
    cv::Mat gray;
    if (roi_image.channels() == 3) {
        cv::cvtColor(roi_image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = roi_image.clone();
    }

    // compute Laplacian
    cv::Mat laplacian;
    cv::Laplacian(gray, laplacian, CV_64F);

    // calculate variance of Laplacian
    cv::Scalar mean, stddev;
    cv::meanStdDev(laplacian, mean, stddev);
    double variance = stddev[0] * stddev[0];

    // qualitative label
    std::string blur_label;
    if (variance < 20.0) {
        blur_label = "Blurry";
    } else if (variance < 100.0) {
        blur_label = "Slightly blurry";
    } else {
        blur_label = "Sharp";
    }

    // print analysis results
    std::cout << "=== BLUR DETECTION ANALYSIS ===" << std::endl;
    std::cout << "Variance of Laplacian: " << std::fixed << std::setprecision(2) << variance << std::endl;
    std::cout << "Assessment: " << blur_label << std::endl;
    std::cout << "===============================" << std::endl;

    // return the original image unchanged
    return input.clone();
}

std::string BlurDetectionOperation::getNameImpl() const {
    return "blur_detection";
}

bool BlurDetectionOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // no parameters to validate for blur detection operation
    return true;
}

 