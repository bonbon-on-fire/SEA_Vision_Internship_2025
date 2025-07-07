#pragma once

#include "base_operation.hpp"

// brightness adjustment operation (parameter: factor)
class BrightnessOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
};

// blur operation (parameters: kernel_size, sigma)
class BlurOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
};

// contrast adjustment operation (parameters: factor, brightness_offset)
class ContrastOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
};

// crop operation (parameters: x, y, width, height)
class CropOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
};

// sharpen operation (parameters: strength, kernel_size)
class SharpenOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
};

// edge count analysis operation (no parameters)
class EdgeCountOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
};

// object count analysis operation (no parameters)
class ObjectCountOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
};

// color distribution analysis operation (no parameters)
class ColorDistributionOperation : public Operation {
private:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
    std::string getNameImpl() const override;
    bool validateParametersImpl(const std::map<std::string, double>& parameters) const override;
}; 