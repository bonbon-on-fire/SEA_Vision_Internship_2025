#pragma once

#include "graph_node.hpp"
#include "operations/hpp/base_operation.hpp"
#include <memory>

// operation node that wraps existing operations
class OperationNode : public GraphNode {
private:
    std::unique_ptr<Operation> operation_;

public:
    // constructor
    OperationNode(const NodeId& id, const std::string& operation_type);
    
    // destructor
    ~OperationNode() override = default;
    
    // execute method - applies the wrapped operation
    cv::Mat execute(const std::vector<cv::Mat>& inputs, 
                   const ROI& roi, 
                   const std::map<std::string, double>& parameters) override;
    
    // get the wrapped operation
    const Operation* getOperation() const { return operation_.get(); }
    
    // set the wrapped operation
    void setOperation(std::unique_ptr<Operation> operation) { operation_ = std::move(operation); }
}; 