#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "graph_node.hpp"

// type definitions for cleaner code
using ExecutionLevel = std::vector<NodeId>;
using ExecutionLevels = std::vector<ExecutionLevel>;

// connection structure
struct Connection {
    NodeId from_node;
    int from_port;
    NodeId to_node;
    int to_port;
    
    Connection(const NodeId& from, int from_p, const NodeId& to, int to_p)
        : from_node(from), from_port(from_p), to_node(to), to_port(to_p) {}
};

// container class for managing a graph of operations
class Graph {
private:
    NodeMap nodes_;                    // all nodes in the graph
    std::vector<Connection> connections_; // connections between nodes
    NodeId input_node_id_;             // id of the input node
    NodeId output_node_id_;            // id of the output node
    ExecutionLevels execution_levels_; // levels for parallel execution

    // helper function for cycle detection using dfs
    bool hasCyclesDFS(const NodeId& node_id, 
                     std::unordered_set<NodeId>& visited,
                     std::unordered_set<NodeId>& recursion_stack) const;

public:
    // constructor
    Graph();
    
    // destructor
    ~Graph() = default;
    
    // add a node to the graph
    void addNode(NodePtr node);
    
    // get a node by id
    GraphNode* getNode(const NodeId& node_id);
    const GraphNode* getNode(const NodeId& node_id) const;
    
    // check if a node exists
    bool hasNode(const NodeId& node_id) const;
    
    // remove a node from the graph
    bool removeNode(const NodeId& node_id);
    
    // connect two nodes
    void connectNodes(const NodeId& from_node_id, const NodeId& to_node_id);
    
    // add connection with ports
    void addConnection(const NodeId& from_node, int from_port, 
                      const NodeId& to_node, int to_port);
    
    // disconnect two nodes
    void disconnectNodes(const NodeId& from_node_id, const NodeId& to_node_id);
    
    // get all node ids
    std::vector<NodeId> getAllNodeIds() const;
    
    // get all nodes (returns raw pointers to avoid unique_ptr copying issues)
    std::vector<const GraphNode*> getAllNodes() const;
    
    // get nodes by type
    std::vector<NodeId> getNodesByType(const std::string& type) const;
    
    // get incoming connections for a node
    std::vector<Connection> getIncomingConnections(const NodeId& node_id) const;
    
    // get outgoing connections for a node
    std::vector<Connection> getOutgoingConnections(const NodeId& node_id) const;
    
    // get topological order for execution
    std::vector<NodeId> getTopologicalOrder() const;
    
    // get input node id
    const NodeId& getInputNodeId() const { return input_node_id_; }
    
    // get output node id
    const NodeId& getOutputNodeId() const { return output_node_id_; }
    
    // set input node id
    void setInputNodeId(const NodeId& node_id) { input_node_id_ = node_id; }
    
    // set output node id
    void setOutputNodeId(const NodeId& node_id) { output_node_id_ = node_id; }
    
    // get number of nodes
    size_t getNodeCount() const { return nodes_.size(); }
    
    // check if graph is empty
    bool isEmpty() const { return nodes_.empty(); }
    
    // validate graph structure
    bool validate() const;
    
    // detect cycles in the graph
    bool hasCycles() const;
    
    // perform topological sort to get execution levels
    ExecutionLevels topologicalSort() const;
    
    // get execution levels (cached result of topological sort)
    const ExecutionLevels& getExecutionLevels() const { return execution_levels_; }
    
    // update execution levels (call after graph changes)
    void updateExecutionLevels();
    
    // clear all nodes
    void clear();
}; 