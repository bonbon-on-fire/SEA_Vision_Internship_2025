#include "graph.hpp"
#include <algorithm>
#include <queue>
#include <unordered_set>

// constructor
Graph::Graph() {}

// add a node to the graph
void Graph::addNode(NodePtr node) {
    if (node) {
        nodes_[node->getId()] = std::move(node);
        // update execution levels after adding a node
        updateExecutionLevels();
    }
}

// get a node by id
GraphNode* Graph::getNode(const NodeId& node_id) {
    auto it = nodes_.find(node_id);
    return (it != nodes_.end()) ? it->second.get() : nullptr;
}

// get a node by id (const version)
const GraphNode* Graph::getNode(const NodeId& node_id) const {
    auto it = nodes_.find(node_id);
    return (it != nodes_.end()) ? it->second.get() : nullptr;
}

// check if a node exists
bool Graph::hasNode(const NodeId& node_id) const {
    return nodes_.find(node_id) != nodes_.end();
}

// remove a node from the graph
bool Graph::removeNode(const NodeId& node_id) {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        // remove all connections to/from this node
        GraphNode* node = it->second.get();
        
        // remove this node from all input lists
        for (const auto& input_id : node->getInputNodeIds()) {
            GraphNode* input_node = getNode(input_id);
            if (input_node) {
                input_node->removeOutput(node_id);
            }
        }
        
        // remove this node from all output lists
        for (const auto& output_id : node->getOutputNodeIds()) {
            GraphNode* output_node = getNode(output_id);
            if (output_node) {
                output_node->removeInput(node_id);
            }
        }
        
        // remove the node
        nodes_.erase(it);
        
        // update execution levels after removing a node
        updateExecutionLevels();
        return true;
    }
    return false;
}

// connect two nodes
void Graph::connectNodes(const NodeId& from_node_id, const NodeId& to_node_id) {
    GraphNode* from_node = getNode(from_node_id);
    GraphNode* to_node = getNode(to_node_id);
    
    if (from_node && to_node) {
        from_node->addOutput(to_node_id);
        to_node->addInput(from_node_id);
        // update execution levels after connecting nodes
        updateExecutionLevels();
    }
}

// disconnect two nodes
void Graph::disconnectNodes(const NodeId& from_node_id, const NodeId& to_node_id) {
    GraphNode* from_node = getNode(from_node_id);
    GraphNode* to_node = getNode(to_node_id);
    
    if (from_node && to_node) {
        from_node->removeOutput(to_node_id);
        to_node->removeInput(from_node_id);
        // update execution levels after disconnecting nodes
        updateExecutionLevels();
    }
}

// get all node ids
std::vector<NodeId> Graph::getAllNodeIds() const {
    std::vector<NodeId> ids;
    ids.reserve(nodes_.size());
    for (const auto& pair : nodes_) {
        ids.push_back(pair.first);
    }
    return ids;
}

// validate graph structure
bool Graph::validate() const {
    // check if input and output nodes exist
    if (!input_node_id_.empty() && !hasNode(input_node_id_)) {
        return false;
    }
    if (!output_node_id_.empty() && !hasNode(output_node_id_)) {
        return false;
    }
    
    // check for cycles
    if (hasCycles()) {
        return false;
    }
    
    // check that all connections reference existing nodes
    for (const auto& pair : nodes_) {
        const GraphNode* node = pair.second.get();
        
        // check input connections
        for (const auto& input_id : node->getInputNodeIds()) {
            if (!hasNode(input_id)) {
                return false;
            }
        }
        
        // check output connections
        for (const auto& output_id : node->getOutputNodeIds()) {
            if (!hasNode(output_id)) {
                return false;
            }
        }
    }
    
    return true;
}

// detect cycles in the graph using depth-first search
bool Graph::hasCycles() const {
    std::unordered_set<NodeId> visited;
    std::unordered_set<NodeId> recursion_stack;
    
    for (const auto& pair : nodes_) {
        const NodeId& node_id = pair.first;
        if (visited.find(node_id) == visited.end()) {
            if (hasCyclesDFS(node_id, visited, recursion_stack)) {
                return true;
            }
        }
    }
    
    return false;
}

// helper function for cycle detection using dfs
bool Graph::hasCyclesDFS(const NodeId& node_id, 
                        std::unordered_set<NodeId>& visited,
                        std::unordered_set<NodeId>& recursion_stack) const {
    visited.insert(node_id);
    recursion_stack.insert(node_id);
    
    const GraphNode* node = getNode(node_id);
    if (node) {
        for (const auto& output_id : node->getOutputNodeIds()) {
            if (visited.find(output_id) == visited.end()) {
                if (hasCyclesDFS(output_id, visited, recursion_stack)) {
                    return true;
                }
            } else if (recursion_stack.find(output_id) != recursion_stack.end()) {
                return true; // cycle detected
            }
        }
    }
    
    recursion_stack.erase(node_id);
    return false;
}

// perform topological sort to get execution levels
ExecutionLevels Graph::topologicalSort() const {
    ExecutionLevels levels;
    
    // calculate in-degrees for each node
    std::map<NodeId, int> in_degree;
    for (const auto& pair : nodes_) {
        in_degree[pair.first] = 0;
    }
    
    for (const auto& pair : nodes_) {
        const GraphNode* node = pair.second.get();
        for (const auto& output_id : node->getOutputNodeIds()) {
            in_degree[output_id]++;
        }
    }
    
    // kahn's algorithm for topological sorting
    std::queue<NodeId> queue;
    
    // add nodes with in-degree 0 to queue
    for (const auto& pair : in_degree) {
        if (pair.second == 0) {
            queue.push(pair.first);
        }
    }
    
    while (!queue.empty()) {
        ExecutionLevel current_level;
        
        // process all nodes at current level
        size_t level_size = queue.size();
        for (size_t i = 0; i < level_size; ++i) {
            NodeId current_node_id = queue.front();
            queue.pop();
            
            current_level.push_back(current_node_id);
            
            // reduce in-degree of all neighbors
            const GraphNode* current_node = getNode(current_node_id);
            if (current_node) {
                for (const auto& output_id : current_node->getOutputNodeIds()) {
                    in_degree[output_id]--;
                    if (in_degree[output_id] == 0) {
                        queue.push(output_id);
                    }
                }
            }
        }
        
        levels.push_back(current_level);
    }
    
    // check if all nodes were processed (no cycles)
    size_t processed_nodes = 0;
    for (const auto& level : levels) {
        processed_nodes += level.size();
    }
    
    if (processed_nodes != nodes_.size()) {
        // cycle detected, return empty levels
        return ExecutionLevels();
    }
    
    return levels;
}

// update execution levels (call after graph changes)
void Graph::updateExecutionLevels() {
    execution_levels_ = topologicalSort();
}

// clear all nodes
void Graph::clear() {
    nodes_.clear();
    input_node_id_.clear();
    output_node_id_.clear();
    execution_levels_.clear();
} 