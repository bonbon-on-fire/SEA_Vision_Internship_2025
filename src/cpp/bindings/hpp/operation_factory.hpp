#pragma once

#include <string>
#include <memory>
#include <map>
#include "operations/hpp/base_operation.hpp"
#include "operations/hpp/operations.hpp"

// operation factory class
class OperationFactory {
public:
    // create an operation based on type string
    static std::unique_ptr<Operation> createOperation(const std::string& type);

private:
    using OperationCreator = std::unique_ptr<Operation>(*)();
    static const std::map<std::string, OperationCreator> creators;
    
    static std::unique_ptr<Operation> createBrightness();
    static std::unique_ptr<Operation> createBlur();
    static std::unique_ptr<Operation> createCrop();
    static std::unique_ptr<Operation> createSharpen();
    static std::unique_ptr<Operation> createContrast();
    static std::unique_ptr<Operation> createEdgeCount();
    static std::unique_ptr<Operation> createBlurDetection();
}; 