# SurgiView Image Tracker

SurgiView Image Tracker is a Qt/QML desktop application for simulated medical imaging and surgical instrument overlay workflows.

## Implemented capabilities

- Load X-ray/CT slices from a folder (`.png`, `.jpg`, `.bmp`, baseline `.dcm` support)
- Simulated instrument tracking (fiducial/AR-style marker stream)
- Real-time alignment overlay (tool tip vs. target anatomy)
- Depth measurement tools (pixel and mm scale)
- Recording and playback of tracking sessions
- Annotated frame and video export (PNG sequence + ffmpeg MP4)
- Lab testing mode with simulated C-arm drift
- Unit tests (Qt Test) for measurement, tracking, telemetry, and baseline DICOM decoding

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
- `SurgiView: Demo Walkthrough`
- `SurgiView: Demo Sender TCP`
- `SurgiView: Demo Sender Serial`
- `SurgiView: Package Demo Bundle`
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

This project includes a built-in baseline DICOM decoder for common uncompressed grayscale studies using Explicit VR Little Endian transfer syntax. It supports:

- Monochrome 8-bit and 16-bit pixel data
- Pixel spacing extraction
- MONOCHROME1 and MONOCHROME2 presentation
- Basic window center and window width handling
- Rescale slope and intercept application

Optional DCMTK integration scaffold is available now.

- CMake option: `SURGIVIEW_ENABLE_DCMTK=ON`
- Build example:

```powershell
cmake -S . -B build -G Ninja -DSURGIVIEW_ENABLE_DCMTK=ON
cmake --build build -j 2
```

When DCMTK is disabled, the built-in decoder is used first. If a DICOM file is unsupported or malformed, loading falls back to the existing simulated grayscale frame so the demo remains operable.

## Notes on video export

`Export Video` renders annotated PNG frames and then invokes `ffmpeg` from PATH to produce MP4. If ffmpeg is unavailable, frames are still exported and the status message explains what is missing.

## Tracking hardware adapter scaffold

Tracking now supports two source modes:

- `simulated`: internal lab-mode trajectory simulation.
- `external`: expects telemetry from a hardware adapter feed.

External mode is available through `SurgiViewController::setTrackingSourceMode("external")` and `SurgiViewController::ingestExternalTelemetry(x, y, depthMm)`.

The app now also includes TCP and serial telemetry listeners for live adapter integration.

- Start the TCP listener from the UI with `source mode = external`.
- Default port example: `45454`
- Start the serial listener from the UI with `source mode = external`.
- Example serial settings: `COM3` at `115200` baud.
- Send newline-delimited JSON payloads such as:

```json
{"x":210.0,"y":160.0,"depthMm":12.5}
```

The same newline-delimited JSON payload format is accepted over both TCP and serial transports.

## Demo sender utility

For live demos, use `scripts/send_demo_telemetry.py` to stream realistic motion data into SurgiView.

TCP example:

```powershell
.\.venv\Scripts\python.exe .\scripts\send_demo_telemetry.py --mode tcp --host 127.0.0.1 --port 45454 --count 300
```

Serial example:

```powershell
.\.venv\Scripts\python.exe .\scripts\send_demo_telemetry.py --mode serial --serial-port COM3 --baud-rate 115200 --count 300
```

If serial mode is needed in a fresh environment, install the demo dependency first:

```powershell
.\.venv\Scripts\python.exe -m pip install -r .\scripts\requirements-demo.txt
```

## Demo package

Portfolio/demo assets are included in the repository:

- Sample slice folder: `sample-data/demo_ct_series`
- Sample replay session: `sample-data/demo_session.json`
- Demo walkthrough: `docs/demo/walkthrough.md`
- Evaluator guide: `docs/demo/evaluator-guide.md`
- Release note: `docs/demo/release-note-v1.md`
- GitHub release draft: `docs/demo/github-release-description.md`
- Portfolio summary: `docs/demo/portfolio-summary.md`
- Requirement checklist: `docs/demo/requirements-checklist.md`
- Demo helper script: `scripts/start_demo.ps1`
- Demo packaging script: `scripts/package_demo_bundle.ps1`

To print the recommended walkthrough steps and launch the app:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\start_demo.ps1
```

To create a distributable demo bundle in `dist/`:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package_demo_bundle.ps1
```

For reviewer handoff or interview prep, start with:

- `docs/demo/release-note-v1.md`
- `docs/demo/portfolio-summary.md`
- `docs/demo/evaluator-guide.md`
- `docs/demo/requirements-checklist.md`

Current branch implementation includes an in-process external telemetry adapter stub so robotics/camera drivers can be integrated without changing overlay, measurement, or recording pipelines.

## Suggested next steps for cadaver/lab readiness

1. Integrate true DICOM decoding (window/level, metadata validation).
2. Replace simulated tracker with camera/robotics telemetry adapter.
3. Execute and sign off the safety artifacts in `docs/safety/` with real evidence.
4. Add persistent case/session model with audit logging.
