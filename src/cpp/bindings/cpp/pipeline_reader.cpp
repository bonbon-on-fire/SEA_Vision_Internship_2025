#include "bindings/hpp/pipeline_reader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

PipelineConfig PipelineReader::readPipeline(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("could not open pipeline file: " + filename);
    }
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("invalid JSON in pipeline file: " + std::string(e.what()));
    }
    
    // auto-detect format and handle accordingly
    if (isGraphFormat(j)) {
        // convert graph format to linear format for backward compatibility
        GraphConfig graph_config = readGraphFromJson(j);
        return convertGraphToPipeline(graph_config);
    } else {
        // parse as linear format
        return readPipelineFromJson(j);
    }
}

GraphConfig PipelineReader::readGraph(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("could not open graph file: " + filename);
    }
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("invalid JSON in graph file: " + std::string(e.what()));
    }
    
    return readGraphFromJson(j);
}

GraphConfig PipelineReader::readGraphConfig(const std::string& filename) {
    return readGraph(filename);
}

GraphConfig PipelineReader::readGraphFromJson(const json& j) {
    GraphConfig config;
    
    // parse nodes array
    if (!j.contains("nodes") || !j["nodes"].is_array()) {
        throw std::runtime_error("graph must contain 'nodes' array");
    }
    
    for (const auto& node_json : j["nodes"]) {
        config.nodes.push_back(parseNode(node_json));
    }
    
    // parse input/output node ids
    if (j.contains("input_node_id")) {
        config.input_node_id = j["input_node_id"];
    }
    
    if (j.contains("output_node_id")) {
        config.output_node_id = j["output_node_id"];
    }
    
    // parse connections array
    if (j.contains("connections") && j["connections"].is_array()) {
        for (const auto& conn_json : j["connections"]) {
            Connection conn;
            
            if (conn_json.contains("from_node") && conn_json["from_node"].is_string()) {
                conn.from_node = conn_json["from_node"];
            }
            
            if (conn_json.contains("from_port") && conn_json["from_port"].is_number()) {
                conn.from_port = conn_json["from_port"].get<int>();
            }
            
            if (conn_json.contains("to_node") && conn_json["to_node"].is_string()) {
                conn.to_node = conn_json["to_node"];
            }
            
            if (conn_json.contains("to_port") && conn_json["to_port"].is_number()) {
                conn.to_port = conn_json["to_port"].get<int>();
            }
            
            config.connections.push_back(conn);
        }
    }
    
    // parse input/output image paths
    if (j.contains("input_image")) {
        config.input_image = j["input_image"];
    }
    
    if (j.contains("output_image")) {
        config.output_image = j["output_image"];
    }
    
    return config;
}

PipelineConfig PipelineReader::readPipelineFromJson(const json& j) {
    PipelineConfig config;
    
    // parse global roi
    if (j.contains("roi")) {
        config.global_roi = parseROI(j["roi"]);
    } else {
        config.global_roi = ROI(0, 0, 0, 0, true);
    }
    
    // parse operations array
    if (!j.contains("operations") || !j["operations"].is_array()) {
        throw std::runtime_error("pipeline must contain 'operations' array");
    }
    
    for (const auto& op_json : j["operations"]) {
        config.operations.push_back(parseOperation(op_json));
    }
    
    // parse input/output image paths
    if (j.contains("input_image")) {
        config.input_image = j["input_image"];
    }
    
    if (j.contains("output_image")) {
        config.output_image = j["output_image"];
    }
    
    return config;
}

GraphConfig PipelineReader::convertPipelineToGraph(const PipelineConfig& pipeline) {
    GraphConfig graph;
    
    // set input/output image paths
    graph.input_image = pipeline.input_image;
    graph.output_image = pipeline.output_image;
    
    // create input node
    NodeConfig input_node;
    input_node.id = "input";
    input_node.type = "input";
    input_node.image_path = pipeline.input_image;
    graph.nodes.push_back(input_node);
    graph.input_node_id = "input";
    
    // create operation nodes
    std::string prev_node_id = "input";
    for (size_t i = 0; i < pipeline.operations.size(); ++i) {
        const auto& op = pipeline.operations[i];
        
        NodeConfig node;
        node.id = op.type + "_" + std::to_string(i + 1);
        node.type = op.type;
        node.parameters = op.parameters;
        node.roi = op.roi.full_image ? pipeline.global_roi : op.roi;
        node.inputs = {prev_node_id};
        
        graph.nodes.push_back(node);
        prev_node_id = node.id;
    }
    
    // create output node
    NodeConfig output_node;
    output_node.id = "output";
    output_node.type = "output";
    output_node.image_path = pipeline.output_image;
    output_node.inputs = {prev_node_id};
    graph.nodes.push_back(output_node);
    graph.output_node_id = "output";
    
    return graph;
}

PipelineConfig PipelineReader::convertGraphToPipeline(const GraphConfig& graph) {
    PipelineConfig pipeline;
    
    // set input/output image paths
    pipeline.input_image = graph.input_image;
    pipeline.output_image = graph.output_image;
    
    // find the global roi (use the first operation's roi as default)
    pipeline.global_roi = ROI(0, 0, 0, 0, true);
    
    // convert nodes to operations (skip input/output nodes)
    for (const auto& node : graph.nodes) {
        if (node.type != "input" && node.type != "output") {
            OperationConfig op;
            op.type = node.type;
            op.parameters = node.parameters;
            op.roi = node.roi;
            pipeline.operations.push_back(op);
        }
    }
    
    return pipeline;
}

bool PipelineReader::isGraphFormat(const json& j) {
    // check if json has "nodes" array (graph format)
    if (j.contains("nodes") && j["nodes"].is_array()) {
        return true;
    }
    
    // check if json has "operations" array (linear format)
    if (j.contains("operations") && j["operations"].is_array()) {
        return false;
    }
    
    // default to linear format for backward compatibility
    return false;
}

NodeConfig PipelineReader::parseNode(const json& node_json) {
    NodeConfig node;
    
    // parse node id
    if (!node_json.contains("id") || !node_json["id"].is_string()) {
        throw std::runtime_error("node must have 'id' field");
    }
    node.id = node_json["id"];
    
    // parse node name (optional, defaults to id)
    if (node_json.contains("name") && node_json["name"].is_string()) {
        node.name = node_json["name"];
    } else {
        node.name = node.id; // default name to id if not provided
    }
    
    // parse node type
    if (!node_json.contains("type") || !node_json["type"].is_string()) {
        throw std::runtime_error("node must have 'type' field");
    }
    node.type = node_json["type"];
    
    // parse parameters
    if (node_json.contains("parameters") && node_json["parameters"].is_object()) {
        for (const auto& [key, value] : node_json["parameters"].items()) {
            if (value.is_number()) {
                node.parameters[key] = value.get<double>();
            }
        }
    }
    
    // parse inputs array
    if (node_json.contains("inputs") && node_json["inputs"].is_array()) {
        for (const auto& input : node_json["inputs"]) {
            if (input.is_string()) {
                node.inputs.push_back(input.get<std::string>());
            }
        }
    }
    
    // parse roi
    if (node_json.contains("roi")) {
        node.roi = parseROI(node_json["roi"]);
    } else {
        node.roi = ROI(0, 0, 0, 0, true);
    }
    
    // parse image_path (for input/output nodes)
    if (node_json.contains("image_path") && node_json["image_path"].is_string()) {
        node.image_path = node_json["image_path"];
    }
    
    return node;
}

ROI PipelineReader::parseROI(const json& roi_json) {
    ROI roi;
    
    roi.x = roi_json.value("x", 0);
    roi.y = roi_json.value("y", 0);
    roi.width = roi_json.value("width", 0);
    roi.height = roi_json.value("height", 0);
    
    roi.full_image = (roi.width == 0 && roi.height == 0);
    
    return roi;
}

OperationConfig PipelineReader::parseOperation(const json& op_json) {
    OperationConfig op;
    
    // parse operation type
    if (!op_json.contains("type") || !op_json["type"].is_string()) {
        throw std::runtime_error("operation must have 'type' field");
    }
    op.type = op_json["type"];
    
    // parse parameters
    if (op_json.contains("parameters") && op_json["parameters"].is_object()) {
        for (const auto& [key, value] : op_json["parameters"].items()) {
            if (value.is_number()) {
                op.parameters[key] = value.get<double>();
            }
        }
    }
    
    // parse roi
    if (op_json.contains("roi")) {
        op.roi = parseROI(op_json["roi"]);
    } else {
        op.roi = ROI(0, 0, 0, 0, true);
    }
    
    return op;
} 