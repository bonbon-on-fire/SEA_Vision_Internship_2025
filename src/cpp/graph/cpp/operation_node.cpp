#include "operation_node.hpp"
#include "bindings/hpp/operation_factory.hpp"
#include <stdexcept>

// constructor
OperationNode::OperationNode(const NodeId& id, const std::string& operation_type)
    : GraphNode(id, operation_type) {
    
    // create the wrapped operation using the factory
    operation_ = OperationFactory::createOperation(operation_type);
    if (!operation_) {
        throw std::runtime_error("could not create operation of type: " + operation_type);
    }
}

// execute method - applies the wrapped operation
cv::Mat OperationNode::execute(const std::vector<cv::Mat>& inputs, 
                              const ROI& roi, 
                              const std::map<std::string, double>& parameters) {
    // check that we have exactly one input (for now, single-input operations)
    if (inputs.empty()) {
        throw std::runtime_error("operation node requires exactly one input image");
    }
    
    if (inputs.size() > 1) {
        throw std::runtime_error("operation node can only handle one input image (for now)");
    }
    
    // apply the operation using the existing operation system
    return operation_->execute(inputs[0], roi, parameters);
} 