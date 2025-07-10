#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <opencv2/opencv.hpp>
#include "base_operation.hpp"

// forward declarations
class GraphNode;

// type definitions for cleaner code
using NodeId = std::string;
using NodePtr = std::unique_ptr<GraphNode>;
using NodeMap = std::map<NodeId, NodePtr>;
using InputList = std::vector<NodeId>;
using OutputList = std::vector<NodeId>;

// base class for all nodes in the graph-based pipeline system
class GraphNode {
protected:
    NodeId id_;
    std::string type_;
    std::map<std::string, double> parameters_;
    InputList input_node_ids_;
    OutputList output_node_ids_;
    ROI roi_;
    bool executed_;
    cv::Mat result_;
    
public:
    GraphNode(const NodeId& id, const std::string& type);
    virtual ~GraphNode() = default;
    
    virtual cv::Mat execute(const std::vector<cv::Mat>& inputs, 
                           const ROI& roi, 
                           const std::map<std::string, double>& parameters) = 0;
    
    const NodeId& getId() const { return id_; }
    const std::string& getType() const { return type_; }
    const InputList& getInputNodeIds() const { return input_node_ids_; }
    const OutputList& getOutputNodeIds() const { return output_node_ids_; }
    const ROI& getROI() const { return roi_; }
    bool isExecuted() const { return executed_; }
    const cv::Mat& getResult() const { return result_; }
    
    void setInputNodeIds(const InputList& inputs) { input_node_ids_ = inputs; }
    void setOutputNodeIds(const OutputList& outputs) { output_node_ids_ = outputs; }
    void setROI(const ROI& roi) { roi_ = roi; }
    void setParameters(const std::map<std::string, double>& params) { parameters_ = params; }
    void setExecuted(bool executed) { executed_ = executed; }
    void setResult(const cv::Mat& result) { result_ = result; }
    
    void addInput(const NodeId& input_node_id);
    
    void addOutput(const NodeId& output_node_id);
    
    bool removeInput(const NodeId& input_node_id);
    
    bool removeOutput(const NodeId& output_node_id);
    
    bool hasInput(const NodeId& input_node_id) const;
    
    bool hasOutput(const NodeId& output_node_id) const;
    
    size_t getInputCount() const { return input_node_ids_.size(); }
    
    size_t getOutputCount() const { return output_node_ids_.size(); }
    
    bool isReadyForExecution() const { return !executed_; }
    
    void resetExecution();
}; 