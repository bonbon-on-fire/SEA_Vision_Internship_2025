#include "bindings/hpp/operation_factory.hpp"
#include "operations/hpp/operations.hpp"

const std::map<std::string, OperationFactory::OperationCreator> OperationFactory::creators = {
    {"brightness", &OperationFactory::createBrightness},
    {"blur", &OperationFactory::createBlur},
    {"crop", &OperationFactory::createCrop},
    {"sharpen", &OperationFactory::createSharpen},
    {"contrast", &OperationFactory::createContrast},
    {"edge_count", &OperationFactory::createEdgeCount},
    {"blur_detection", &OperationFactory::createBlurDetection}
};

std::unique_ptr<Operation> OperationFactory::createOperation(const std::string& type) {
    auto it = creators.find(type);
    if (it != creators.end()) {
        return it->second();
    }
    return nullptr;
}

std::unique_ptr<Operation> OperationFactory::createBrightness() {
    return std::make_unique<BrightnessOperation>();
}

std::unique_ptr<Operation> OperationFactory::createBlur() {
    return std::make_unique<BlurOperation>();
}

std::unique_ptr<Operation> OperationFactory::createCrop() {
    return std::make_unique<CropOperation>();
}

std::unique_ptr<Operation> OperationFactory::createSharpen() {
    return std::make_unique<SharpenOperation>();
}

std::unique_ptr<Operation> OperationFactory::createContrast() {
    return std::make_unique<ContrastOperation>();
}

std::unique_ptr<Operation> OperationFactory::createEdgeCount() {
    return std::make_unique<EdgeCountOperation>();
}

std::unique_ptr<Operation> OperationFactory::createBlurDetection() {
    return std::make_unique<BlurDetectionOperation>();
}