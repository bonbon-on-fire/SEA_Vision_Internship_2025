import argparse
import sys
import os
from pathlib import Path
import json
import subprocess

# define available operations and their parameters
OPERATIONS = [
    {
        "name": "brightness",
        "params": [
            {"name": "factor", "type": float, "prompt": "brightness factor (0.0-5.0, default 1.0)", "default": 1.0}
        ]
    },
    {
        "name": "blur",
        "params": [
            {"name": "kernel_size", "type": int, "prompt": "kernel size (odd, 3-31, default 5)", "default": 5},
            {"name": "sigma", "type": float, "prompt": "sigma (0.1-10.0, default 1.0)", "default": 1.0}
        ]
    },
    {
        "name": "contrast",
        "params": [
            {"name": "factor", "type": float, "prompt": "contrast factor (0.0-3.0, default 1.0)", "default": 1.0},
            {"name": "brightness_offset", "type": float, "prompt": "brightness offset (-100 to 100, default 0)", "default": 0.0}
        ]
    },
    {
        "name": "crop",
        "params": [
            {"name": "x", "type": int, "prompt": "x (default 0)", "default": 0},
            {"name": "y", "type": int, "prompt": "y (default 0)", "default": 0},
            {"name": "width", "type": int, "prompt": "width (default: image width - x)", "default": None},
            {"name": "height", "type": int, "prompt": "height (default: image height - y)", "default": None}
        ]
    },
    {
        "name": "sharpen",
        "params": [
            {"name": "strength", "type": float, "prompt": "strength (0.0-2.0, default 1.0)", "default": 1.0},
            {"name": "kernel_size", "type": int, "prompt": "kernel size (odd, 3-15, default 5)", "default": 5}
        ]
    },
    {
        "name": "edge_count",
        "params": []
    },
    {
        "name": "blur_detection",
        "params": []
    }
]

def prompt_for_operation():
    print("\navailable operations:")
    for idx, op in enumerate(OPERATIONS, 1):
        print(f"  {idx}. {op['name']}")
    print("  0. done (finish pipeline)")
    while True:
        try:
            choice = int(input("select operation number: "))
            if 0 <= choice <= len(OPERATIONS):
                return choice
            else:
                print(f"please enter a number between 0 and {len(OPERATIONS)}.")
        except ValueError:
            print("invalid input. please enter a number.")

def prompt_for_params(op):
    params = {}
    for param in op["params"]:
        while True:
            prompt = f"  {param['prompt']}: "
            val = input(prompt)
            if val == '' and param["default"] is not None:
                params[param["name"]] = param["default"]
                break
            try:
                if val == '' and param["default"] is None:
                    break  # skip optional param
                params[param["name"]] = param["type"](val)
                break
            except ValueError:
                print(f"invalid value for {param['name']}. please enter a {param['type'].__name__}.")
    return params

def prompt_for_roi():
    print("\nenter roi (region of interest) parameters:")
    print("(enter 0 for width/height to process full image)")
    
    x = int(input("  x coordinate (default 0): ") or "0")
    y = int(input("  y coordinate (default 0): ") or "0")
    width = int(input("  width (0 for full image): ") or "0")
    height = int(input("  height (0 for full image): ") or "0")
    
    return {
        "x": x,
        "y": y,
        "width": width,
        "height": height
    }

def main():
    print("welcome to the sea vision pipeline builder!")
    operations = []

    while True:
        op_num = prompt_for_operation()
        if op_num == 0:
            break
        op = OPERATIONS[op_num - 1]
        params = prompt_for_params(op)
        operations.append({
            "type": op["name"],
            "parameters": params
        })
    if not operations:
        print("no operations selected. exiting.")
        return
    
    # get roi input
    roi = prompt_for_roi()
    
    input_image = input("enter input image path: ")
    output_image = input("enter output image path: ")
    pipeline ={
        "roi": roi,
        "operations": operations,
        "input_image": input_image,
        "output_image": output_image
    }
    json_path = "pipeline_cli.json"
    with open(json_path, "w") as f:
        json.dump(pipeline, f, indent=2)
    print(f"pipeline json written to {json_path}")
    # run the c++ executable
    cmd = [os.path.join("build", "Release", "sea_vision.exe"), json_path, input_image, output_image]
    print(f"running: {' '.join(cmd)}")
    # set OpenCV DLL path in environment
    opencv_bin = r"C:\Users\nishi\sea_vision_project\opencv\build\x64\vc16\bin"
    env = os.environ.copy()
    env["PATH"] = opencv_bin + ";" + env["PATH"]

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True, env=env)
        print(result.stdout)
        print("pipeline executed successfully.")
    except subprocess.CalledProcessError as e:
        print("error running pipeline:")
        if e.stdout:
            print("stdout:")
            print(e.stdout)
        if e.stderr:
            print("stderr:")
            print(e.stderr)

if __name__ == "__main__":
    main()