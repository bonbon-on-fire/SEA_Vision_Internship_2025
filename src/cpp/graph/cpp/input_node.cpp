#include "input_node.hpp"
#include <stdexcept>

// constructor
InputNode::InputNode(const NodeId& id, const std::string& image_path)
    : GraphNode(id, "input"), image_path_(image_path) {
}

// execute method - loads image from file
cv::Mat InputNode::execute(const std::vector<cv::Mat>& inputs, 
                          const ROI& roi, 
                          const std::map<std::string, double>& parameters) {
    // input nodes don't use inputs from other nodes
    (void)inputs;
    (void)roi;
    (void)parameters;
    
    // load image from file
    cv::Mat image = cv::imread(image_path_);
    if (image.empty()) {
        throw std::runtime_error("could not load image from: " + image_path_);
    }
    
    return image;
} 