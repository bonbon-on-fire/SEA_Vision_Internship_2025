#include "output_node.hpp"
#include <stdexcept>

// constructor
OutputNode::OutputNode(const NodeId& id, const std::string& image_path)
    : GraphNode(id, "output"), image_path_(image_path) {
}

// execute method - saves image to file
cv::Mat OutputNode::execute(const std::vector<cv::Mat>& inputs, 
                           const ROI& roi, 
                           const std::map<std::string, double>& parameters) {
    // output nodes don't use roi or parameters
    (void)roi;
    (void)parameters;
    
    // check that we have exactly one input
    if (inputs.empty()) {
        throw std::runtime_error("output node requires exactly one input image");
    }
    
    if (inputs.size() > 1) {
        throw std::runtime_error("output node can only handle one input image");
    }
    
    // save image to file
    if (!cv::imwrite(image_path_, inputs[0])) {
        throw std::runtime_error("could not save image to: " + image_path_);
    }
    
    // return the input image unchanged (for potential further processing)
    return inputs[0];
} 