#include "graph_node.hpp"
#include <algorithm>

// constructor for creating a new graph node
GraphNode::GraphNode(const NodeId& id, const std::string& type)
    : id_(id), type_(type), executed_(false) {
    // initialize member variables
    // id_ and type_ are set by the initializer list above
    // parameters_ is default-constructed (empty map)
    // input_node_ids_ and output_node_ids_ are default-constructed (empty vectors)
    // roi_ is default-constructed (all zeros, full_image = false)
    // executed_ is set to false
    // result_ is default-constructed (empty cv::Mat)
}

// add an input connection to this node
void GraphNode::addInput(const NodeId& input_node_id) {
    // check if this input is already connected
    if (!hasInput(input_node_id)) {
        input_node_ids_.push_back(input_node_id);
    }
}

// add an output connection from this node
void GraphNode::addOutput(const NodeId& output_node_id) {
    // check if this output is already connected
    if (!hasOutput(output_node_id)) {
        output_node_ids_.push_back(output_node_id);
    }
}

// remove an input connection from this node
bool GraphNode::removeInput(const NodeId& input_node_id) {
    auto it = std::find(input_node_ids_.begin(), input_node_ids_.end(), input_node_id);
    if (it != input_node_ids_.end()) {
        input_node_ids_.erase(it);
        return true;
    }
    return false;
}

// remove an output connection from this node
bool GraphNode::removeOutput(const NodeId& output_node_id) {
    auto it = std::find(output_node_ids_.begin(), output_node_ids_.end(), output_node_id);
    if (it != output_node_ids_.end()) {
        output_node_ids_.erase(it);
        return true;
    }
    return false;
}

// check if this node has a specific input connection
bool GraphNode::hasInput(const NodeId& input_node_id) const {
    return std::find(input_node_ids_.begin(), input_node_ids_.end(), input_node_id) != input_node_ids_.end();
}

// check if this node has a specific output connection
bool GraphNode::hasOutput(const NodeId& output_node_id) const {
    return std::find(output_node_ids_.begin(), output_node_ids_.end(), output_node_id) != output_node_ids_.end();
}

// reset the execution state of this node
void GraphNode::resetExecution() {
    executed_ = false;
    result_ = cv::Mat(); // clear the result
} 