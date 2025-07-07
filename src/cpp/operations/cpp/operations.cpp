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

cv::Mat ObjectCountOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) {
    // extract ROI from input image
    cv::Mat roi_image = ROITools::extractROI(input, roi);
    
    // convert to grayscale
    cv::Mat gray;
    if (roi_image.channels() == 3) {
        cv::cvtColor(roi_image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = roi_image.clone();
    }
    
    // apply threshold to create binary image
    cv::Mat binary;
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    
    // find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binary, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // filter contours by area (remove very small noise)
    int min_area = 50; // minimum area in pixels
    std::vector<std::vector<cv::Point>> filtered_contours;
    std::vector<double> areas;
    
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area >= min_area) {
            filtered_contours.push_back(contour);
            areas.push_back(area);
        }
    }
    
    // calculate statistics
    int object_count = static_cast<int>(filtered_contours.size());
    double total_area = 0;
    double avg_area = 0;
    double max_area = 0;
    double min_area_found = std::numeric_limits<double>::max();
    
    if (!areas.empty()) {
        total_area = std::accumulate(areas.begin(), areas.end(), 0.0);
        avg_area = total_area / areas.size();
        max_area = *std::max_element(areas.begin(), areas.end());
        min_area_found = *std::min_element(areas.begin(), areas.end());
    }
    
    // print analysis results
    std::cout << "=== OBJECT COUNT ANALYSIS ===" << std::endl;
    std::cout << "Total objects detected: " << object_count << std::endl;
    std::cout << "Total object area: " << std::fixed << std::setprecision(1) << total_area << " pixels" << std::endl;
    std::cout << "Average object area: " << std::fixed << std::setprecision(1) << avg_area << " pixels" << std::endl;
    std::cout << "Largest object area: " << std::fixed << std::setprecision(1) << max_area << " pixels" << std::endl;
    std::cout << "Smallest object area: " << std::fixed << std::setprecision(1) << min_area_found << " pixels" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // return the original image unchanged
    return input.clone();
}

std::string ObjectCountOperation::getNameImpl() const {
    return "object_count";
}

bool ObjectCountOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // no parameters to validate for object count operation
    return true;
}

cv::Mat ColorDistributionOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) {
    // extract ROI from input image
    cv::Mat roi_image = ROITools::extractROI(input, roi);
    
    // convert to different color spaces
    cv::Mat hsv, lab;
    cv::cvtColor(roi_image, hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(roi_image, lab, cv::COLOR_BGR2Lab);
    
    // calculate color statistics
    cv::Scalar mean_bgr = cv::mean(roi_image);
    cv::Scalar mean_hsv = cv::mean(hsv);
    cv::Scalar mean_lab = cv::mean(lab);
    
    // calculate standard deviation
    cv::Mat mean, stddev;
    cv::meanStdDev(roi_image, mean, stddev);
    
    // find dominant colors using k-means clustering
    cv::Mat data = roi_image.reshape(1, roi_image.total());
    data.convertTo(data, CV_32F);
    
    cv::Mat labels, centers;
    int k = 5; // number of dominant colors
    cv::kmeans(data, k, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);
    
    // count pixels for each dominant color
    std::vector<int> color_counts(k, 0);
    for (int i = 0; i < labels.rows; i++) {
        int cluster_id = labels.at<int>(i);
        color_counts[cluster_id]++;
    }
    
    // print analysis results
    std::cout << "=== COLOR DISTRIBUTION ANALYSIS ===" << std::endl;
    std::cout << "BGR Color Statistics:" << std::endl;
    std::cout << "  Blue  - Mean: " << std::fixed << std::setprecision(1) << mean_bgr[0] 
              << ", StdDev: " << std::fixed << std::setprecision(1) << stddev.at<double>(0) << std::endl;
    std::cout << "  Green - Mean: " << std::fixed << std::setprecision(1) << mean_bgr[1] 
              << ", StdDev: " << std::fixed << std::setprecision(1) << stddev.at<double>(1) << std::endl;
    std::cout << "  Red   - Mean: " << std::fixed << std::setprecision(1) << mean_bgr[2] 
              << ", StdDev: " << std::fixed << std::setprecision(1) << stddev.at<double>(2) << std::endl;
    
    std::cout << "HSV Color Statistics:" << std::endl;
    std::cout << "  Hue: " << std::fixed << std::setprecision(1) << mean_hsv[0] 
              << ", Saturation: " << std::fixed << std::setprecision(3) << (mean_hsv[1] / 255.0) 
              << ", Value: " << std::fixed << std::setprecision(3) << (mean_hsv[2] / 255.0) << std::endl;
    
    std::cout << "Dominant Colors (BGR):" << std::endl;
    int total_pixels = roi_image.total();
    for (int i = 0; i < k; i++) {
        cv::Vec3f color = centers.at<cv::Vec3f>(i);
        double percentage = (static_cast<double>(color_counts[i]) / total_pixels) * 100.0;
        std::cout << "  Color " << (i + 1) << ": B=" << std::fixed << std::setprecision(0) << color[0] 
                  << " G=" << std::fixed << std::setprecision(0) << color[1] 
                  << " R=" << std::fixed << std::setprecision(0) << color[2] 
                  << " (" << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
    }
    std::cout << "===================================" << std::endl;
    
    // return the original image unchanged
    return input.clone();
}

std::string ColorDistributionOperation::getNameImpl() const {
    return "color_distribution";
}

bool ColorDistributionOperation::validateParametersImpl(const std::map<std::string, double>& parameters) const {
    // no parameters to validate for color distribution operation
    return true;
} 