#pragma once

#include "graph.hpp"
#include "graph_node_factory.hpp"
#include "bindings/hpp/pipeline_reader.hpp"
#include <opencv2/opencv.hpp>
#include <map>
#include <string>

// executor class for running graph-based pipelines
class GraphExecutor {
private:
    Graph graph_;
    std::map<NodeId, cv::Mat> node_results_;  // cache for node execution results
    
public:
    // constructor
    GraphExecutor();
    
    // destructor
    ~GraphExecutor() = default;
    
    // load graph from JSON file
    void loadGraph(const std::string& json_file);
    
    // load graph from GraphConfig
    void loadGraph(const GraphConfig& config);
    
    // execute the graph sequentially
    cv::Mat execute();
    
    // execute the graph with progress reporting
    cv::Mat executeWithProgress(std::function<void(const std::string&, int, int)> progress_callback = nullptr);
    
    // get the final result
    cv::Mat getResult() const;
    
    // clear all cached results
    void clearResults();
    
    // get execution statistics
    struct ExecutionStats {
        int total_nodes;
        int executed_nodes;
        std::chrono::milliseconds execution_time;
    };
    
    ExecutionStats getExecutionStats() const;

private:
    // build graph from configuration
    void buildGraph(const GraphConfig& config);
    
    // execute a single node
    cv::Mat executeNode(const NodeId& node_id);
    
    // get input images for a node
    std::vector<cv::Mat> getNodeInputs(const NodeId& node_id);
    
    // validate graph before execution
    void validateGraph() const;
    
    // execution statistics
    mutable ExecutionStats stats_;
}; 