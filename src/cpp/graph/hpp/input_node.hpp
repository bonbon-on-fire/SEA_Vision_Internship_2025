#pragma once

#include "graph_node.hpp"
#include <opencv2/opencv.hpp>

// input node for loading images from files
class InputNode : public GraphNode {
private:
    std::string image_path_;

public:
    // constructor
    InputNode(const NodeId& id, const std::string& image_path);
    
    // destructor
    ~InputNode() override = default;
    
    // execute method - loads image from file
    cv::Mat execute(const std::vector<cv::Mat>& inputs, 
                   const ROI& roi, 
                   const std::map<std::string, double>& parameters) override;
    
    // get image path
    const std::string& getImagePath() const { return image_path_; }
    
    // set image path
    void setImagePath(const std::string& path) { image_path_ = path; }
}; 