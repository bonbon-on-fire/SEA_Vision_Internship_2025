# SEA Vision Internship 2025

---

## Quick Start

### Prerequisites
- **CMake**: Version 3.15 or higher
- **C++ Compiler**: Supports C++17
- **Python**: 3.7 or higher
- **Conan**: For dependency management ([Install guide](https://conan.io/downloads.html))
- **OpenCV**: Pre-built binaries required (see below)
- **Windows**: (Recommended, as CLI and build paths are Windows-centric)

### 1. Clone the Repository
```powershell
git clone https://github.com/yourusername/sea_vision_project.git
cd sea_vision_project
```

### 2. Install Prerequisites
- [Install CMake](https://cmake.org/download/)
- [Install Python 3.7+](https://www.python.org/downloads/)
- [Install Conan](https://conan.io/downloads.html) (`pip install conan`)
- Install a C++17 compiler (e.g., Visual Studio 2019 or later)
- Download and build OpenCV, or use the provided binaries (see `opencv/README.md.txt` for details)

### 3. Install Dependencies with Conan
```powershell
conan install . --output-folder=build --build=missing
```

### 4. Build the Project
```powershell
cmake -S . -B build
cmake --build build --config Release
```

### 5. Run the Python CLI
```powershell
python src/python/main_cli.py
```
- The CLI will guide you to build a pipeline and generate a JSON config.
- It will then call the C++ executable automatically.

### OpenCV Note
- The project expects OpenCV binaries in `opencv/build`.
- If you use a custom OpenCV build, update the path in `CMakeLists.txt` and `src/python/main_cli.py` as needed.

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

## Code in Action: Linear Workflow Example

### 1. Sample Input

**Input Image:**  
![Input Image](data/input.jpg)

**Output Image:**  
![Output Image](data/output_step4_combined.jpg)

### 2. JSON Pipeline Configuration

```json
{
  "roi": {"x": 0, "y": 0, "width": 0, "height": 0},
  "operations": [
    {"type": "blur", "parameters": {"kernel_size": 5, "sigma": 1.0}},
    {"type": "brightness", "parameters": {"factor": 1.2}},
    {"type": "blur_detection", "parameters": {}}
  ],
  "input_image": "data/input.jpg",
  "output_image": "data/output_blur_bright_blurdetect.jpg"
}
```

### 3. Python CLI Interaction

```
Welcome to the SEA Vision pipeline builder!
Available operations:
  1. brightness
  2. blur
  3. contrast
  4. crop
  5. sharpen
  6. edge_count
  7. blur_detection
  0. done (finish pipeline)

Select operation number: 2
  kernel size (odd, 3-31, default 5): 5
  sigma (0.1-10.0, default 1.0): 1.0
Select operation number: 1
  brightness factor (0.0-5.0, default 1.0): 1.2
Select operation number: 7
Select operation number: 0

enter roi (region of interest) parameters:
(enter 0 for width/height to process full image)
  x coordinate (default 0): 0
  y coordinate (default 0): 0
  width (0 for full image): 0
  height (0 for full image): 0
enter input image path: data/input.jpg
enter output image path: data/output_blur_bright_blurdetect.jpg
pipeline json written to pipeline_cli.json
running: build/Release/sea_vision.exe pipeline_cli.json data/input.jpg data/output_blur_bright_blurdetect.jpg
```

### 4. C++ Execution Output

```
sea_vision.exe started
starting sea vision json-driven pipeline...
pipeline config: pipeline_cli.json
input image: data/input.jpg
output image: data/output_blur_bright_blurdetect.jpg
execution mode: linear

executing linear pipeline...
reading pipeline configuration...
loading input image...
successfully loaded image with size: 800x600
executing pipeline with 3 operations...

  step 1: blur
operation 1 completed successfully!!

  step 2: brightness
operation 2 completed successfully!!

  step 3: blur_detection
blur score: 0.42 (image considered sharp)
operation 3 completed successfully!!

pipeline completed successfully!!
output saved to: data/output_blur_bright_blurdetect.jpg
```

## How the Workflow Runs

**1. Loading the Pipeline Configuration**

The system loads the pipeline configuration from a JSON file, which defines the sequence of operations and their parameters:
```cpp
PipelineConfig config = PipelineReader::readPipeline("pipeline_cli.json");
```

**2. Creating Operations Dynamically (Factory Pattern)**

For each operation in the pipeline, the factory creates the correct operation object based on the type specified in the JSON:
```cpp
for (const auto& op_config : config.operations) {
    auto operation = OperationFactory::createOperation(op_config.type);
    // ...
}
```

**3. Executing Each Operation**

Each operation is executed in sequence, modifying the image as it passes through the pipeline:
```cpp
cv::Mat result = image.clone();
for (const auto& op_config : config.operations) {
    result = operation->execute(result, roi_to_use, op_config.parameters);
}
```

**4. Saving the Output**

After all operations are complete, the final image is saved:
```cpp
cv::imwrite(output_image, result);
```

**5. Example: Implementing a Blur Operation**

Here’s a simplified version of how a blur operation might be implemented:
```cpp
cv::Mat BlurOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) {
    int kernel_size = static_cast<int>(parameters.at("kernel_size"));
    double sigma = parameters.at("sigma");
    cv::Mat output;
    cv::GaussianBlur(input, output, cv::Size(kernel_size, kernel_size), sigma);
    return output;
}
```

## Project Structure & File Guide

This section explains where to find key components, how files are organized, and how to add new features (like operations).

### Directory Overview

```
sea_vision_project/
│
├── src/
│   ├── cpp/
│   │   ├── operations/
│   │   │   ├── cpp/   # C++ source files for each operation (e.g., blur.cpp, brightness.cpp)
│   │   │   └── hpp/   # C++ headers for each operation (e.g., blur.hpp, brightness.hpp)
│   │   ├── graph/
│   │   │   ├── cpp/   # Graph execution logic (nodes, edges, scheduler)
│   │   │   └── hpp/
│   │   └── bindings/
│   │       ├── cpp/   # C++/Python binding code (if any)
│   │       └── hpp/
│   └── python/
│       └── main_cli.py  # Python CLI for pipeline building
│
├── tests/
│   ├── cpp/    # C++ unit tests (test_operations.cpp, test_runner.cpp)
│   ├── python/ # Python CLI/config tests
│   └── json/   # Example/test pipeline configs
│
├── data/       # Example images (input/output)
├── opencv/     # Prebuilt OpenCV binaries, build scripts
├── CMakeLists.txt
├── conanfile.txt
└── README.md
```

### File & Format Details

#### 1. **Operation Implementation (C++)**

- **Location:**
  - **Source:** `src/cpp/operations/cpp/<operation>.cpp`
  - **Header:** `src/cpp/operations/hpp/<operation>.hpp`

- **Format:**  
Each operation is a class inheriting from a base `Operation` class, e.g.:
```cpp
// src/cpp/operations/hpp/blur.hpp
class BlurOperation : public Operation {
public:
    cv::Mat executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) override;
};
```

```cpp
// src/cpp/operations/cpp/blur.cpp
cv::Mat BlurOperation::executeImpl(const cv::Mat& input, const ROI& roi, const std::map<std::string, double>& parameters) {
    int kernel_size = static_cast<int>(parameters.at("kernel_size"));
    double sigma = parameters.at("sigma");
    cv::Mat output;
    cv::GaussianBlur(input, output, cv::Size(kernel_size, kernel_size), sigma);
    return output;
}
```

#### 2. **Operation Registration (Factory Pattern)**

- **Location:**
  - **Source:** `src/cpp/operations/cpp/OperationFactory.cpp`
  - **Header:** `src/cpp/operations/hpp/OperationFactory.hpp`

- **Format:**  
The factory maps operation type strings to classes:
```cpp
// src/cpp/operations/cpp/OperationFactory.cpp
std::unique_ptr<Operation> OperationFactory::createOperation(const std::string& type) {
    if (type == "blur") return std::make_unique<BlurOperation>();
    if (type == "brightness") return std::make_unique<BrightnessOperation>();
    // ... add new operations here
    throw std::invalid_argument("Unknown operation type: " + type);
}
```

#### 3. **Pipeline Configuration (JSON)**

- **Location:**
  - **Generated Config:** `pipeline_cli.json`
  - **Test Configs:** `tests/json/`

- **Format:**  
```json
{
  "roi": {"x": 0, "y": 0, "width": 0, "height": 0},
  "operations": [
    {"type": "blur", "parameters": {"kernel_size": 5, "sigma": 1.0}},
    {"type": "brightness", "parameters": {"factor": 1.2}}
  ],
  "input_image": "data/input.jpg",
  "output_image": "data/output.jpg"
}
```

#### 4. **Python CLI (Pipeline Builder)**

- **Location:**
  - **Script:** `src/python/main_cli.py`

- **Format:**  
- Presents a menu of available operations (edit this to add new ones).
- Prompts for parameters, validates them, and writes the JSON config.
- Calls the C++ backend with the generated config and image paths.

Example menu code:
```python
operations = [
    {"name": "brightness", "params": [...]},
    {"name": "blur", "params": [...]},
    # Add new operation here
]
```

#### 5. **Graph Execution System**

- **Location:**
  - **Source:** `src/cpp/graph/cpp/`
  - **Header:** `src/cpp/graph/hpp/`

- **Format:**  
- Contains classes for nodes, edges, and the scheduler.
- Handles topological sorting and parallel execution.

#### 6. **Testing**

- **Location:**
  - **C++ Unit Tests:** `tests/cpp/test_operations.cpp`, `tests/cpp/test_runner.cpp`
  - **Python Tests:** `tests/python/test_sequence.py`
  - **Test Configs:** `tests/json/`

- **Format:**  
- C++ and Python unit tests for operations and pipelines.
- Example pipeline configs for automated testing.

#### 7. **OpenCV Integration**

- **Location:**
  - **Binaries & Scripts:** `opencv/`
  - **CMake Config:** `CMakeLists.txt`
  - **Python Path Config:** `src/python/main_cli.py`

- **Format:**  
- Binaries/scripts in `opencv/`
- Path configuration in `CMakeLists.txt` and `src/python/main_cli.py`

## How to Add a New Operation (Step-by-Step)

1. **Implement the Operation**
   - Create `<operation>.cpp` and `<operation>.hpp` in `src/cpp/operations/cpp/` and `hpp/`.
   - Inherit from `Operation`, implement `executeImpl`.

2. **Register in Factory**
   - Add a case for your operation in `OperationFactory.cpp`.

3. **Expose in Python CLI**
   - Add your operation to the menu in `main_cli.py`.
   - Define parameter prompts and validation.

4. **Update JSON Schema (if needed)**
   - Ensure your operation’s parameters are supported in the config format.

5. **Test**
   - Add C++ unit tests in `tests/cpp/`.
   - Add a sample config in `tests/json/`.
   - Optionally, add CLI tests in `tests/python/`.

## Quick Reference Table

| Component         | Location                                 | Format/Notes                                 |
|-------------------|------------------------------------------|----------------------------------------------|
| C++ Operations    | `src/cpp/operations/cpp/`, `hpp/`        | Class per op, inherits `Operation`           |
| Operation Factory | `src/cpp/operations/cpp/OperationFactory.cpp` | String-to-class mapping                      |
| Graph Logic       | `src/cpp/graph/cpp/`, `hpp/`             | Node, edge, scheduler classes                |
| Python CLI        | `src/python/main_cli.py`                  | Menu, param prompts, JSON output             |
| Pipeline Config   | `pipeline_cli.json`, `tests/json/`        | JSON, see example above                      |
| C++ Tests         | `tests/cpp/`                             | Per-op and pipeline tests                    |
| Python Tests      | `tests/python/`                          | CLI/config tests                             |
| Example Images    | `data/`                                  | Input/output for demos/tests                 |
| OpenCV Binaries   | `opencv/`                                | Prebuilt DLLs, scripts                       |

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