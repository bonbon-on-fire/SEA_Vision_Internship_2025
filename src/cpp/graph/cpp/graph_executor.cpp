#include "graph_executor.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>

GraphExecutor::GraphExecutor() {
    stats_.total_nodes = 0;
    stats_.executed_nodes = 0;
    stats_.execution_time = std::chrono::milliseconds(0);
}

void GraphExecutor::loadGraph(const std::string& json_file) {
    // use pipeline reader to parse JSON
    PipelineReader reader;
    GraphConfig config = reader.readGraphConfig(json_file);
    loadGraph(config);
}

void GraphExecutor::loadGraph(const GraphConfig& config) {
    // clear previous results
    clearResults();
    
    // build graph from configuration
    buildGraph(config);
    
    // validate graph
    validateGraph();
    
    // update stats
    stats_.total_nodes = graph_.getNodeCount();
    stats_.executed_nodes = 0;
}

cv::Mat GraphExecutor::execute() {
    return executeWithProgress();
}

cv::Mat GraphExecutor::executeWithProgress(std::function<void(const std::string&, int, int)> progress_callback) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // clear previous results
    clearResults();
    
    // get topological order for execution
    auto execution_order = graph_.getTopologicalOrder();
    
    if (execution_order.empty()) {
        throw std::runtime_error("Graph has no valid execution order (possibly cyclic)");
    }
    
    // execute nodes in topological order
    for (size_t i = 0; i < execution_order.size(); ++i) {
        const auto& node_id = execution_order[i];
        
        // report progress
        if (progress_callback) {
            auto node = graph_.getNode(node_id);
            std::string node_name = node ? node->getName() : "Unknown";
            progress_callback(node_name, i + 1, execution_order.size());
        }
        
        // execute node
        cv::Mat result = executeNode(node_id);
        
        // cache result
        node_results_[node_id] = result;
        stats_.executed_nodes++;
    }
    
    // calculate execution time
    auto end_time = std::chrono::high_resolution_clock::now();
    stats_.execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // return final result (last node in topological order)
    return getResult();
}

cv::Mat GraphExecutor::getResult() const {
    if (node_results_.empty()) {
        return cv::Mat();
    }
    
    // find output nodes
    auto output_nodes = graph_.getNodesByType("output");
    
    if (output_nodes.empty()) {
        // if no output nodes, return result of last executed node
        return node_results_.rbegin()->second;
    }
    
    // return first output node result
    auto first_output = output_nodes[0];
    auto it = node_results_.find(first_output);
    if (it != node_results_.end()) {
        return it->second;
    }
    
    return cv::Mat();
}

void GraphExecutor::clearResults() {
    node_results_.clear();
    stats_.executed_nodes = 0;
    stats_.execution_time = std::chrono::milliseconds(0);
}

GraphExecutor::ExecutionStats GraphExecutor::getExecutionStats() const {
    return stats_;
}

void GraphExecutor::buildGraph(const GraphConfig& config) {
    // create nodes from configuration
    for (const auto& node_config : config.nodes) {
        // create node using factory (use the 4-parameter overload)
        auto node = GraphNodeFactory::createNode(node_config.id, node_config.type, node_config.parameters, node_config.image_path);
        
        if (!node) {
            throw std::runtime_error("Failed to create node: " + node_config.id);
        }
        
        // set image path for input and output nodes (if not already set by factory)
        if (!node_config.image_path.empty()) {
            if (node_config.type == "input") {
                auto input_node = dynamic_cast<InputNode*>(node.get());
                if (input_node) {
                    input_node->setImagePath(node_config.image_path);
                }
            } else if (node_config.type == "output") {
                auto output_node = dynamic_cast<OutputNode*>(node.get());
                if (output_node) {
                    output_node->setImagePath(node_config.image_path);
                }
            }
        }
        
        // set ROI
        node->setROI(node_config.roi);
        
        // add node to graph (move the unique_ptr)
        graph_.addNode(std::move(node));
    }
    
    // create connections
    for (const auto& connection : config.connections) {
        graph_.addConnection(connection.from_node, connection.from_port, 
                           connection.to_node, connection.to_port);
    }
}

cv::Mat GraphExecutor::executeNode(const NodeId& node_id) {
    auto node = graph_.getNode(node_id);
    if (!node) {
        throw std::runtime_error("Node not found: " + node_id);
    }
    
    // get input images for this node
    std::vector<cv::Mat> inputs = getNodeInputs(node_id);
    
    // execute node
    cv::Mat result = node->execute(inputs, node->getROI(), node->getParameters());
    
    return result;
}

std::vector<cv::Mat> GraphExecutor::getNodeInputs(const NodeId& node_id) {
    std::vector<cv::Mat> inputs;
    
    // get incoming connections
    auto incoming = graph_.getIncomingConnections(node_id);
    
    for (const auto& connection : incoming) {
        // get result from source node
        auto it = node_results_.find(connection.from_node);
        if (it != node_results_.end()) {
            inputs.push_back(it->second);
        } else {
            // if source node hasn't been executed yet, this is an error
            throw std::runtime_error("Source node not executed: " + connection.from_node);
        }
    }
    
    return inputs;
}

void GraphExecutor::validateGraph() const {
    // check for cycles
    if (graph_.hasCycles()) {
        throw std::runtime_error("Graph contains cycles - cannot execute");
    }
    
    // check for disconnected nodes
    auto nodes = graph_.getAllNodes();
    for (const auto* node : nodes) {
        auto incoming = graph_.getIncomingConnections(node->getId());
        auto outgoing = graph_.getOutgoingConnections(node->getId());
        
        // input nodes should have no incoming connections
        if (node->getType() == "input" && !incoming.empty()) {
            throw std::runtime_error("Input node has incoming connections: " + node->getName());
        }
        
        // output nodes should have no outgoing connections (optional check)
        if (node->getType() == "output" && !outgoing.empty()) {
            std::cout << "Warning: Output node has outgoing connections: " << node->getName() << std::endl;
        }
    }
} 