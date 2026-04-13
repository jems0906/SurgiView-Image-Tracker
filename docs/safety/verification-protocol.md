# Verification Protocol

## Purpose

Define repeatable verification activities for SurgiView prototype functions relevant to imaging, tracking, measurement, and safety-oriented operator feedback.

## Test environment

- Windows workstation with validated `run.ps1` pipeline
- Qt/MinGW environment configured per repository instructions
- Sample folder containing PNG/JPG slices and optional DICOM files
- Simulated source mode and external telemetry mode exercised separately

## Verification activities

### VP-001 Slice navigation integrity

Steps:
1. Launch the application.
2. Load a folder with at least three slices.
3. Press `Next` and `Prev` repeatedly.
4. Confirm displayed index matches visible image changes.

Expected result:
- Slice label updates correctly.
- No crash or blank frame during navigation.

### VP-002 Overlay alignment visibility

Steps:
1. Start tracking in simulated mode.
2. Click a new target location on the viewer.
3. Observe tool-tip marker, target marker, and connecting line.

Expected result:
- Overlay redraws immediately.
- Alignment error value changes consistently with geometry.

### VP-003 Measurement correctness

Steps:
1. Press and drag on the viewer to create a measurement.
2. Compare displayed pixel distance to expected geometric distance for a simple drag.
3. Verify millimeter value changes with pixel spacing assumptions.

Expected result:
- Pixel and mm values are both shown.
- Values remain non-negative and stable.

### VP-004 Recording and playback

Steps:
1. Start tracking.
2. Start recording for at least five seconds.
3. Stop recording and save session.
4. Reload the session and start playback.

Expected result:
- Save and load complete without error.
- Playback emits motion consistent with recorded trajectory.

### VP-005 Annotated export

Steps:
1. Record a short session.
2. Export annotated frames.
3. If ffmpeg is installed, export video.

Expected result:
- PNG sequence is generated.
- MP4 generation succeeds when ffmpeg is available.
- UI status message reports outcome.

### VP-006 External telemetry mode

Steps:
1. Change source mode to `external`.
2. Start tracking.
3. Inject telemetry samples through UI or controller API.

Expected result:
- No samples are emitted before telemetry ingest.
- Overlay updates immediately after telemetry input.
- Tool position matches injected coordinates.

### VP-007 DICOM fallback behavior

Steps:
1. Load a folder containing `.dcm` files.
2. Run with DCMTK disabled.
3. Observe frame rendering.

Expected result:
- DICOM files do not crash the loader.
- Simulated grayscale fallback is displayed.
- Status and navigation remain usable.

## Automated evidence currently available

- TV-001: `DepthMeasurement` unit tests
- TV-002: `InstrumentTracker` unit tests including external telemetry mode
- CI workflow on Windows for build and test regression

## Execution record template

| Protocol ID | Date | Operator | Build/Commit | Result | Notes |
| --- | --- | --- | --- | --- | --- |
| VP-001 |  |  |  |  |  |
| VP-002 |  |  |  |  |  |
| VP-003 |  |  |  |  |  |
| VP-004 |  |  |  |  |  |
| VP-005 |  |  |  |  |  |
| VP-006 |  |  |  |  |  |
| VP-007 |  |  |  |  |  |
