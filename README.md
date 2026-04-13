# SurgiView Image Tracker

SurgiView Image Tracker is a Qt/QML desktop application for simulated medical imaging and surgical instrument overlay workflows.

## Implemented capabilities

- Load X-ray/CT slices from a folder (`.png`, `.jpg`, `.bmp`, `.dcm` placeholder support)
- Simulated instrument tracking (fiducial/AR-style marker stream)
- Real-time alignment overlay (tool tip vs. target anatomy)
- Depth measurement tools (pixel and mm scale)
- Recording and playback of tracking sessions
- Annotated frame and video export (PNG sequence + ffmpeg MP4)
- Lab testing mode with simulated C-arm drift
- Unit tests (Qt Test) for measurement and tracking behavior

## Build requirements

- CMake 3.21+
- Qt 6.5+ with modules: Core, Gui, Quick, QuickControls2, Multimedia, Test
- C++17 compiler (Visual Studio 2022 recommended on Windows)

## Build (Windows, Visual Studio)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Run

```powershell
.\build\Release\SurgiViewImageTracker.exe
```

## One-command workflow (current environment)

This repository includes `run.ps1`, which configures, builds, tests, and launches the app with the validated MinGW + Ninja setup.

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1 -All
```

Useful subsets:

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1 -Configure
powershell -ExecutionPolicy Bypass -File .\run.ps1 -Build
powershell -ExecutionPolicy Bypass -File .\run.ps1 -Test
powershell -ExecutionPolicy Bypass -File .\run.ps1 -Run
```

## VS Code tasks

Preconfigured tasks are available in `.vscode/tasks.json`:

- `SurgiView: Configure`
- `SurgiView: Build`
- `SurgiView: Test`
- `SurgiView: Run`
- `SurgiView: Full Pipeline`

## CI pipeline

GitHub Actions workflow is defined in `.github/workflows/ci.yml`.

The pipeline runs on Windows and performs:

- Qt + MinGW toolchain provisioning via `aqtinstall`
- CMake configure with Ninja
- Build
- Unit tests (`ctest`)

## Suggested feature branches

- `feature/dicom-dcmtk-integration`
- `feature/tracker-hardware-adapter`
- `feature/iec62304-safety-artifacts`

## Safety artifacts

IEC 62304-style starter artifacts are available under `docs/safety/`:

- `docs/safety/README.md`
- `docs/safety/hazard-analysis.md`
- `docs/safety/traceability-matrix.csv`
- `docs/safety/verification-protocol.md`
- `docs/safety/release-checklist.md`

## Notes on DICOM

This project includes a lightweight `.dcm` placeholder loader for rapid prototyping and simulation. For production-grade DICOM parsing/rendering, integrate DCMTK or GDCM and route decoded pixel data into `DicomSeriesLoader`.

## Notes on video export

`Export Video` renders annotated PNG frames and then invokes `ffmpeg` from PATH to produce MP4. If ffmpeg is unavailable, frames are still exported and the status message explains what is missing.

## Suggested next steps for cadaver/lab readiness

1. Integrate true DICOM decoding (window/level, metadata validation).
2. Replace simulated tracker with camera/robotics telemetry adapter.
3. Execute and sign off the safety artifacts in `docs/safety/` with real evidence.
4. Add persistent case/session model with audit logging.
