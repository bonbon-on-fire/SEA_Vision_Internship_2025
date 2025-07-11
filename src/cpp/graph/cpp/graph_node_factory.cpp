#include "graph_node_factory.hpp"
#include <stdexcept>

// create a node based on type and configuration
NodePtr GraphNodeFactory::createNode(const std::string& node_id, 
                                    const std::string& node_type,
                                    const std::map<std::string, double>& parameters,
                                    const std::string& image_path) {
    
    if (node_type == "input") {
        return createInputNode(node_id, image_path);
    } else if (node_type == "output") {
        return createOutputNode(node_id, image_path);
    } else {
        // assume it's an operation type
        return createOperationNode(node_id, node_type);
    }
}

// create a node based on type and name (simplified interface)
NodePtr GraphNodeFactory::createNode(const std::string& node_type, const std::string& node_name) {
    if (node_type == "input") {
        return createInputNode(node_name, ""); // image path will be set later
    } else if (node_type == "output") {
        return createOutputNode(node_name, ""); // image path will be set later
    } else {
        // assume it's an operation type
        return createOperationNode(node_name, node_type);
    }
}

// create input node
NodePtr GraphNodeFactory::createInputNode(const std::string& node_id, const std::string& image_path) {
    // allow empty image path for now - it will be set later
    return std::make_unique<InputNode>(node_id, image_path);
}

// create output node
NodePtr GraphNodeFactory::createOutputNode(const std::string& node_id, const std::string& image_path) {
    // allow empty image path for now - it will be set later
    return std::make_unique<OutputNode>(node_id, image_path);
}

// create operation node
NodePtr GraphNodeFactory::createOperationNode(const std::string& node_id, const std::string& operation_type) {
    return std::make_unique<OperationNode>(node_id, operation_type);
} 