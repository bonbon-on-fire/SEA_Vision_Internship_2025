#pragma once

#include "graph_node.hpp"
#include "input_node.hpp"
#include "output_node.hpp"
#include "operation_node.hpp"
#include <memory>

// factory class for creating graph nodes
class GraphNodeFactory {
public:
    // create a node based on type and configuration
    static NodePtr createNode(const std::string& node_id, 
                             const std::string& node_type,
                             const std::map<std::string, double>& parameters = {},
                             const std::string& image_path = "");
    
    // create a node based on type and name (simplified interface)
    static NodePtr createNode(const std::string& node_type, const std::string& node_name);
    
    // create input node
    static NodePtr createInputNode(const std::string& node_id, const std::string& image_path);
    
    // create output node
    static NodePtr createOutputNode(const std::string& node_id, const std::string& image_path);
    
    // create operation node
    static NodePtr createOperationNode(const std::string& node_id, const std::string& operation_type);
}; 