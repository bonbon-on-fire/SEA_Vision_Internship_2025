#pragma once

#include "graph_node.hpp"
#include <opencv2/opencv.hpp>

// output node for saving images to files
class OutputNode : public GraphNode {
private:
    std::string image_path_;

public:
    // constructor
    OutputNode(const NodeId& id, const std::string& image_path);
    
    // destructor
    ~OutputNode() override = default;
    
    // execute method - saves image to file
    cv::Mat execute(const std::vector<cv::Mat>& inputs, 
                   const ROI& roi, 
                   const std::map<std::string, double>& parameters) override;
    
    // get image path
    const std::string& getImagePath() const { return image_path_; }
    
    // set image path
    void setImagePath(const std::string& path) { image_path_ = path; }
}; 