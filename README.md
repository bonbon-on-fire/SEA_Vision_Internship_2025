# SEA Vision Internship 2025

## Project Overview

### What We Built

During this internship, I prototyped a hybrid computer vision system for SEA Vision's pharmaceutical quality control designed to accelerate and automate defect detection on manufacturing lines, ensuring faster and more accurate identification of product defects. This innovative approach uses a Python-driven framework to analyze images and dynamically determine optimal sequences of image processing operations, while leveraging C++ for high-performance execution of these operations.

This project delivers a flexible, scriptable image processing pipeline system designed for pharmaceutical manufacturing quality control. The system supports two execution models for real-time defect detection, each optimized for different complexity levels of quality control workflows.

**Linear Pipeline**: Sequential execution of operations for straightforward quality checks, where each operation processes the image in order. This model is ideal for simple defect detection scenarios where operations must be performed in a specific sequence.

**Graph-Based Pipeline**: Advanced dependency-based execution for complex multi-step defect analysis, where operations can run in parallel when they don't depend on each other. This model enables sophisticated workflows where multiple analysis paths can be executed simultaneously, significantly reducing total processing time for complex quality control scenarios.

The system operates on a two-phase model: Python handles the compile-time configuration (analyzing sample images and determining optimal operation sequences, parameters, and workflows) while C++ executes the runtime processing (performing actual image analysis and defect detection). This separation is crucial because manufacturing line decisions about product rejection must be made in milliseconds as products move continuously on conveyor belts - there's little time for complex algorithm calculations during the critical inspection window since all the work depends on milliseconds.

The new approach allows operation sequences to be created once during compile time using Python, then executed efficiently in C++ to maintain fast runtime performance. During the compile-time phase, the Python framework analyzes input data characteristics (such as image orientation, lighting conditions, product type, or defect patterns) to determine the optimal sequence of operations. For example, a horizontally oriented image might require a different operation sequence than a vertically oriented one, or images with poor lighting might need additional brightness/contrast adjustments before defect detection. This intelligent analysis enables the creation of pre-programmed sequences that can be automatically selected and run based on different input data characteristics, eliminating the need for manual configuration for each new product or scenario.

### Why We Built It

**Business Problem**: While SEA Vision's existing C++ systems successfully handle basic image processing operations, they require manual configuration for each new product type or manufacturing scenario. Quality control engineers must personally configure operation sequences for every specific input, leading to repetitive work and potential inconsistencies across different production lines.

SEA Vision wanted to evaluate whether a hybrid Python/C++ approach could enable more complex and automated processes while maintaining the critical sub-millisecond runtime performance of their existing C++ systems. The goal was to create a system that could:

- Automate the detection of defects in real-time as products move through manufacturing lines (e.g., pill blister pack inspection)
- Automatically count and validate product quantities (e.g., ensuring correct number of pills per blister)
- Provide both technical and non-technical users with easy-to-use interfaces for pipeline configuration
- Support reproducible workflows through configuration files for consistent quality standards
- Scale from simple operations to complex multi-step defect analysis pipelines

## Technical Implementation

### Architecture Design

The system follows a modular, extensible architecture with clear separation of concerns:

ADD MERMAID GRAPHIC

### Core Components

**C++ Backend Architecture**: The core execution engine is built in C++ to maintain the sub-millisecond performance requirements. The backend consists of three main subsystems that work together to provide flexible, high-performance image processing capabilities.

**Operations System**: The operations system provides the fundamental building blocks for image processing. Each operation inherits from a base abstract class that ensures consistent interfaces and behavior. The system includes seven specialized operations designed specifically for pharmaceutical quality control scenarios, each with parameter validation and optimized implementations for manufacturing line environments.

**Graph System**: The graph system enables complex, dependency-based workflows where operations can execute in parallel when they don't depend on each other. This system includes topological sorting algorithms to determine optimal execution order and supports advanced features like conditional execution paths and result caching for improved performance.

**Python CLI Interface**: The Python CLI provides an intuitive interface for quality control engineers to rapidly prototype and configure image processing pipelines. This interface bridges the gap between the flexibility of Python and the performance of C++, allowing users to experiment with different operation sequences during development phases.

**Interactive Workflow Design**: The CLI features a menu-driven interface that guides users through the process of selecting operations, configuring parameters, and defining regions of interest. Each operation includes pharmaceutical industry-specific default values and parameter ranges that have been validated for manufacturing line environments. The system provides real-time feedback and comprehensive error handling to ensure reliable operation in production settings.

**Seamless Integration**: The Python interface automatically generates JSON configuration files and seamlessly executes the C++ backend, providing a smooth development experience. This integration enables rapid prototyping of new quality control workflows while maintaining the performance benefits of the C++ execution engine.

**Configuration System**: The system uses JSON-based configuration files to define image processing pipelines, enabling version control, reproducibility, and easy sharing of quality control workflows across different manufacturing lines and facilities.

**Pipeline Configuration Format**: The JSON format provides a human-readable way to define operation sequences, parameters, and regions of interest. This format supports both simple linear pipelines and complex graph-based workflows, making it suitable for a wide range of quality control scenarios. The configuration includes comprehensive parameter validation to ensure operations receive valid inputs.

**Graph-Based Workflows**: For complex quality control scenarios, the system supports graph-based configurations where operations are represented as nodes with defined input and output connections. This enables parallel execution of independent operations and supports conditional execution paths based on intermediate results, significantly improving performance for complex defect detection workflows.

## Current Status and Future Development

The project successfully established the foundational hybrid architecture that enables input-based sequence selection, with a production-ready system featuring 7 core image processing operations, an interactive Python CLI for rapid prototyping, and a comprehensive testing framework. We have since implemented advanced graph-based pipeline capabilities that support complex dependency-based workflows and parallel processing. The current implementation includes a base hybrid Python/C++ architecture with compile-time sequence generation, seven core image processing operations (brightness, blur, contrast, crop, sharpen, edge detection, and blur detection), an interactive Python CLI for rapid prototyping and parameter optimization, a JSON-based configuration system for reproducible workflows, a comprehensive testing framework with 15+ test scenarios, and graph-based pipeline execution with dependency management.

Looking forward, we plan to implement machine learning integration for automatic sequence optimization based on historical defect data, real-time video stream processing capabilities for continuous manufacturing line monitoring, advanced ROI detection algorithms for automatic region identification, and a performance monitoring and analytics dashboard for manufacturing line optimization. Beyond these planned features, the architecture opens possibilities for future innovations including adaptive learning pipelines that self-optimize sequences based on defect detection accuracy, multi-sensor fusion integrating temperature, humidity, and vibration sensors for comprehensive quality assessment, predictive defect detection using AI models that identify potential defects before they become visible, distributed edge computing with local processing nodes communicating with central quality control systems, augmented reality interfaces providing real-time defect analysis overlays for quality control technicians, blockchain quality tracking for immutable records of quality control decisions ensuring regulatory compliance, and quantum computing integration for ultra-fast pattern recognition in complex defect scenarios.

This hybrid architecture enables quality control engineers to develop sophisticated defect detection algorithms during idle time, then deploy them for sub-millisecond execution during production runs, significantly reducing runtime latency and increasing throughput while providing a foundation for future innovations.

## Technical Challenges and Solutions

**OpenCV Integration Complexity**: The integration of OpenCV libraries presented significant challenges due to complex DLL management requirements on Windows systems. This was solved through automated DLL path management in the Python CLI and the use of pre-built OpenCV binaries, ensuring consistent deployment across different manufacturing environments.

**Graph-Based Execution Architecture**: Implementing complex dependency management and execution ordering required sophisticated algorithms to handle parallel processing scenarios. The solution involved developing a topological sorting algorithm with cycle detection and parallel execution planning, enabling efficient processing of complex quality control workflows.

**Parameter Validation and Type Safety**: Ensuring type safety and parameter range validation across all operations was critical for system reliability. This challenge was addressed through a template-based validation system with compile-time type checking, providing robust error detection while maintaining performance.

**Performance Optimization Requirements**: Meeting sub-millisecond execution requirements for real-time product rejection decisions on high-speed manufacturing lines required careful optimization. The solution combined ROI-based processing, efficient memory management, and OpenCV optimization techniques to ensure timely defect detection and product removal without compromising accuracy.

## Conclusion

The SEA Vision project successfully delivered a production-ready image processing pipeline system for pharmaceutical manufacturing quality control that addresses both immediate business needs and long-term strategic goals. The combination of high-performance C++ backend, user-friendly Python interface, and advanced graph-based execution provides a solid foundation for real-time defect detection and quality control in pharmaceutical manufacturing.

---

**Internship Duration**: [Duration]
**Skills**: Python, C++, OpenCV, CMake, JSON, Git
**Lines of Code**: ~2,500 (C++), ~500 (Python), ~1,000 (Tests & Config)