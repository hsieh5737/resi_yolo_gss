# Resi-YOLO GSS 

Glare-Aware Tiny Vessel Detection on Embedded UAV Platforms via Resilient YOLO and Glare Severity Scoring

This repository provides a reference C-based implementation scaffold for the Resi-YOLO maritime detection framework with Glare Severity Score (GSS) analysis.
The project is designed to support reproducible deployment and evaluation of glare-aware tiny-object detection on embedded edge platforms, targeting NVIDIA Jetson Orin Nano.

The system integrates a lightweight glare quantification module with a real-time object detection pipeline,
focusing on challenging maritime scenarios such as strong sea-surface reflections, specular highlights, white-wave interference, and low-contrast distant targets.

The implementation emphasizes reproducibility, deterministic system behavior, and clear separation between
core control logic (C) and auxiliary computation utilities (Python).

---

## Key Features

**Glare-Aware Analysis:**  
Implements Glare Severity Score (GSS) based on HSV color space statistics,
allowing images to be categorized into Low / Medium / High glare levels.
This enables glare-stratified evaluation and analysis of detection robustness.

**Tiny Vessel Focus:**  
Explicitly supports identification of tiny maritime targets
(objects smaller than 32×32 pixels),
which represent a major challenge in long-range UAV-based maritime search and rescue.

**Edge-Oriented Deployment:**  
Designed for embedded edge platforms with strict latency constraints.
The deployment pipeline supports TensorRT FP16 inference on Jetson Orin Nano,
matching real-time inference latency reported in the paper (~15 ms).

**Reproducible Pipeline:**  
The project structure and scripts are aligned with Appendix B3 (GSS computation)
and Appendix A3 (deployment and evaluation automation),
making the repository suitable as supplementary material for academic publication.

**Deterministic Control Flow:**  
The main pipeline is implemented in C, with Python used only for well-isolated computation utilities.
This design choice ensures predictable execution behavior and simplifies system-level integration.

---

## Implementation Notes

The project is implemented with C as the primary orchestration language.
Python scripts are invoked only for auxiliary tasks such as glare computation,
model export, and evaluation automation.

The design follows a modular structure:
- C code controls execution order and system integration.
- Python scripts implement clearly defined algorithmic components.
- No training logic is included; the repository focuses on deployment and evaluation.

The implementation targets embedded Linux environments and assumes:
- No interactive GUI dependency
- No runtime training
- Fixed input resolution and bounded memory usage

---

## Directory Structure

resi_yolo_gss/
├── CMakeLists.txt
├── README.md
│
├── src/
│ ├── main.c # Pipeline entry point
│ ├── gss_runner.c/h # GSS execution interface
│ ├── yolo_label.c/h # YOLO parsing & tiny vessel logic
│ └── utils.c/h
│
├── scripts/
│ ├── compute_gss.py # Appendix B3: GSS computation
│ ├── export_model.sh # Appendix A3-1: model export
│ ├── inference.py # Appendix A3-2: inference (placeholder)
│ └── run_tracking.py # Appendix A3-3: tracking replay (placeholder)
│
├── data/
│ ├── images/
│ ├── labels/
│ └── metadata.csv
│
├── weights/
│ ├── best.pt
│ └── resi_yolo_640x640_fp16.engine
│
└── output/
├── dets.txt
└── logs/


---

## Glare Severity Score (Appendix B3)

The Glare Severity Score (GSS) is computed as follows:

1. Convert the input image to HSV color space
2. Extract the top 65% vertical region of the image as the region of interest (ROI)
3. Identify glare pixels satisfying:
   - Value (V) > 217
   - Saturation (S) < 38
4. Compute: GSS = (# glare pixels) / (total ROI pixels)
5. Assign glare level:
- Low:    GSS < 0.3
- Medium: 0.3 ≤ GSS < 0.6
- High:   GSS ≥ 0.6

The results are stored in `data/metadata.csv` with the following fields: filename, gss_value, glare_level


---

## Object Classes and Tiny Vessel Definition

### Supported Object Classes

The system supports five maritime object categories:

| Class ID | Category                 |
|--------:|--------------------------|
| 0       | Boat                     |
| 1       | Swimmer                  |
| 2       | Jetski                   |
| 3       | Buoy                     |
| 4       | Life-saving appliance    |

### Tiny Vessel Criterion

An object is classified as a **Tiny Vessel** if:
- bounding box width < 32 pixels AND bounding box height < 32 pixels


This criterion is evaluated using YOLO normalized annotations and the original image resolution.

---

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

## Run (GSS Computation)
./resi_yolo

This command performs:

Glare Severity Score computation

Generation of data/metadata.csv

## Model Export (Appendix A3-1)

To convert PyTorch weights to a TensorRT FP16 engine:

```
bash scripts/export_model.sh
```

The exported engine will be saved as: `weights/resi_yolo_640x640_fp16.engine`

## Inference and Tracking (Appendix A3)

### Inference

`scripts/inference.py` is intended for:
- Running inference on video streams
- Recording per-frame inference latency

Note: The C code in `src/` currently does not link against the TensorRT (nvinfer) C++ API.
The README and `weights/` include a TensorRT engine file as an example artifact,
but integrating TensorRT requires adding a C/C++ inference wrapper that links
to the TensorRT libraries (nvinfer, nvonnxparser, etc.) and exposes a small
runtime API callable from the C orchestrator. This repository provides a
Python `scripts/inference.py` helper to run inference via a CLI; to use
TensorRT from the C pipeline you can either:

- implement a small C++ wrapper that creates an `IExecutionContext` and exposes
   a C API (e.g., `trt_infer_run(const char *engine_path, const char *input, ... )`) and link it in the build;
- or spawn an external inference process (for example `python` or a dedicated
   binary) from `main.c` (the current pipeline already uses `system()` to call Python scripts).

Marking this for follow-up: integrate `nvinfer` requires adding linking flags
and a lightweight C++ wrapper — TODO in this repo.

### Tracking Replay
`scripts/run_tracking.py` is intended for:
- Injecting artificial delay and jitter
- Simulating asynchronous detection updates
- Evaluating tracking robustness with TSMR (Time-Stamped Measurement Replay)

Both scripts are provided as placeholders and can be extended to match experimental configurations described in the paper.

## Citation

If you use this codebase or refer to this implementation in your research, please cite the corresponding paper.