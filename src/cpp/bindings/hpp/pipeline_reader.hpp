#pragma once

#include <string>
#include <vector>
#include <map>
#include "operations/hpp/base_operation.hpp"
#include "graph/hpp/graph.hpp"
#include <nlohmann/json.hpp>

/**
 * structure to hold operation configuration from JSON
 */
struct OperationConfig {
    std::string type;
    std::map<std::string, double> parameters;
    ROI roi;
};

/**
 * structure to hold complete pipeline configuration (linear format)
 */
struct PipelineConfig {
    ROI global_roi;
    std::vector<OperationConfig> operations;
    std::string input_image;
    std::string output_image;
};

/**
 * structure to hold node configuration from JSON (graph format)
 */
struct NodeConfig {
    std::string id;
    std::string type;
    std::map<std::string, double> parameters;
    std::vector<std::string> inputs;
    ROI roi;
    std::string image_path;  // for input/output nodes
};

/**
 * structure to hold complete graph configuration (graph format)
 */
struct GraphConfig {
    std::vector<NodeConfig> nodes;
    std::string input_node_id;
    std::string output_node_id;
    std::string input_image;
    std::string output_image;
};

// json pipeline reader class
class PipelineReader {
public:
    // read pipeline configuration from json file (auto-detect format)
    static PipelineConfig readPipeline(const std::string& filename);
    
    // read graph configuration from json file
    static GraphConfig readGraph(const std::string& filename);
    
    // convert linear pipeline to graph format
    static GraphConfig convertPipelineToGraph(const PipelineConfig& pipeline);
    
    // convert graph to linear pipeline format
    static PipelineConfig convertGraphToPipeline(const GraphConfig& graph);

private:
    // parse roi from json object
    static ROI parseROI(const nlohmann::json& roi_json);
    
    // parse operation configuration from json object
    static OperationConfig parseOperation(const nlohmann::json& op_json);
    
    // parse node configuration from json object
    static NodeConfig parseNode(const nlohmann::json& node_json);
    
    // detect if json is in graph format or linear format
    static bool isGraphFormat(const nlohmann::json& json);
    
    // parse graph configuration from json object
    static GraphConfig readGraphFromJson(const nlohmann::json& j);
    
    // parse pipeline configuration from json object
    static PipelineConfig readPipelineFromJson(const nlohmann::json& j);
}; 