# SurgiView Requirement Checklist

## Completion status

This checklist maps the original project brief to the current implementation so the delivered scope is explicit.

## What you build

- Qt app that overlays surgical instruments onto live X-ray or CT style feeds: completed
  Current implementation uses Qt/QML for the desktop UI, image viewing, overlay rendering, interaction, and controller orchestration.

## Core features

- DICOM image viewer: completed at prototype baseline
  The app loads `.dcm` files through the built-in decoder for common uncompressed Explicit VR Little Endian monochrome studies, with optional DCMTK integration scaffold for broader support.

- Instrument tracking with simulated fiducials or AR markers: completed
  Simulated tracking is built in, and external telemetry ingestion is supported through manual injection, TCP, and serial transports.

- Real-time alignment overlay for tool tip versus target anatomy: completed
  The UI renders tool and target positions with a live alignment line and updates the alignment error continuously.

- Depth measurement tools: completed
  Pixel and millimeter measurements are computed from interactive drag input and current slice spacing.

- Recording and playback for surgical review: completed
  Tracking sessions can be recorded, saved, loaded, and replayed.

- Export annotated video: completed
  The app exports annotated PNG frames and can assemble MP4 output through `ffmpeg` when available.

- Lab testing mode with simulated C-arm behavior: completed
  Lab testing mode is exposed in the UI and simulated drift is handled in the tracker pipeline.

## Why it fits

- Medical imaging experience as a major asset: completed
  The codebase includes imaging, DICOM handling, measurement, review workflow, and medical-software framing artifacts.

- Full-stack medical software plus testing rigor: completed at prototype level
  The project includes UI, imaging, telemetry, export, CI, unit tests, demo tooling, and safety-oriented documentation.

## Key features from the brief

- Load X-ray or CT slices: completed
- Track scalpel or drill position: completed
- Measure distances such as bone screw depth: completed
- Export annotated video: completed
- Cadaver or lab simulation mode: completed as lab simulation

## Tech requirements

- C++17 OOP: completed
- Qt and QML desktop app: completed
- Visual Studio and Git workflow: substantially completed
  Git workflow is fully exercised. Visual Studio project generation is supported through CMake, while the validated build path in this environment uses MinGW plus Ninja.

- TDD and simulation-oriented testing: completed at prototype level
  Automated tests cover measurement, tracking, telemetry parsing, TCP ingestion, and baseline DICOM decoding.

- Surgical context understanding: completed
  The naming, workflows, measurement framing, safety artifacts, and demo materials are all aligned to surgical imaging review use cases.

## Remaining limits

- This is not a clinically validated navigation system.
- Baseline DICOM support is intentionally scoped to common uncompressed monochrome files unless DCMTK is enabled.
- Safety artifacts are prototype process evidence, not a regulated submission package.
