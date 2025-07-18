# SEA Vision Internship 2025

## Project Overview

### What I Built

During my internship at SEA Vision, I designed and prototyped a hybrid computer vision system to improve quality control on pharmaceutical manufacturing lines. The goal was to speed up and automate defect detection, ensuring faster and more accurate identification of faulty products. I created a flexible, Python-driven interface for defining image processing workflows and a high-performance C++ backend that executes them in real-time.

The result was a proof-of-concept image processing pipeline that adapts to different types of products and imaging conditions. By separating the configuration (in Python) from the execution (in C++), the system can make real-time decisions in milliseconds—crucial when products are moving rapidly on conveyor belts.

Two execution models were built:

**Linear Pipeline**: A simple, step-by-step operation chain for straightforward defect checks.

**Graph-Based Pipeline**: A parallelizable, dependency-aware model that supports more complex quality control workflows with optimized runtime performance.

The system can intelligently adjust the image processing strategy depending on input characteristics like orientation, lighting, or product type—all determined ahead of time using Python. Once a pipeline is configured, it can be executed over and over in C++ with minimal latency.

### Why It Matters

Before this system, SEA Vision's engineers had to manually configure image operations for each product and use case, which was repetitive and inefficient. My project explored whether a hybrid Python/C++ setup could provide the same high-speed processing while enabling more intelligent, automated configuration. It succeeded in doing both.

## Technical Overview

### System Architecture

The architecture follows a modular design that separates configuration-time logic (Python) from execution-time logic (C++), improving both flexibility and speed. Here's a high-level view of the components:

- **Python CLI**: For building and testing workflows.
- **JSON Config Files**: Define the operation graph, parameters, and regions of interest.
- **C++ Engine**: Executes workflows at sub-millisecond speeds.

### Key Components

**Operations System**: Defines core image processing steps (brightness, blur, contrast, crop, sharpen, edge detection, and blur detection). Each operation has built-in validation and is optimized for real-world production use.

**Graph Execution System**: Allows complex workflows where some operations can run in parallel. It uses topological sorting to execute steps efficiently and supports conditional paths and result caching.

**Python CLI**: Engineers can build and tweak image processing pipelines interactively. The CLI helps users define operations, tune parameters, and set regions of interest with guidance from pharmaceutical defaults and validation rules.

**Configuration Files**: All pipelines are defined using JSON files, which makes them easy to version, share, and reuse across production lines. These files also support both linear and graph-based workflows.

### Smart Compile-Time Configuration

The Python interface does more than just set parameters. It can analyze input images to figure out the best sequence of operations based on image features. For example, darker images might automatically get a brightness adjustment, or rotated images might trigger a different crop. This smart configuration happens once before deployment, so the actual runtime logic in C++ stays fast.

### Graph-Based Parallel Execution

For more advanced workflows, the graph-based model allows operations that don't depend on each other to run in parallel. This significantly reduces total processing time, especially when multiple image features need to be checked independently.

## Current Status

By the end of the internship, the system supported:

- Hybrid architecture with clear separation of compile-time and runtime
- 7 core operations optimized for pharmaceutical scenarios
- Fully working Python CLI for interactive pipeline design
- JSON-based config system
- Graph-based execution model with dependency resolution
- Testing framework with 15+ real-world scenarios

## Challenges and How I Solved Them

**OpenCV Integration**: Managed DLL complexity with automation in the Python CLI and used prebuilt binaries for stability.

**Graph Execution Logic**: Implemented topological sorting with cycle detection to enable safe, parallel execution.

**Parameter Validation**: Built a robust, template-based validation system with type checking to prevent misconfigurations.

**Runtime Optimization**: Used ROI-based processing and memory-efficient techniques to meet tight time constraints on real manufacturing lines.

## Future Possibilities

The modular nature of the system opens the door to exciting extensions:

- ML-based sequence generation based on historical defect data
- Real-time video stream processing
- Automatic ROI detection

In the long term, this architecture could support adaptive pipelines that learn from results, combine data from multiple sensors, and even incorporate edge computing or quantum acceleration.

## TL;DR

I built a Python-C++ hybrid system that automates image-based defect detection for pharmaceutical products. It analyzes inputs during downtime, pre-generates smart pipelines, and runs them at lightning speed during production—giving SEA Vision both flexibility and performance in one tool.

---

**Internship Duration**: Summer 2025  
**Technologies**: Python, C++, OpenCV, CMake, JSON, Git  
**Lines of Code**: ~2,500 (C++), ~500 (Python), ~1,000 (Tests & Config)