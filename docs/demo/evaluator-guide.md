# Evaluator Guide

## Purpose

This guide is intended for hiring managers, reviewers, or technical evaluators who want a fast, structured walkthrough of the SurgiView Image Tracker demo package.

## What to look for

- Desktop application built in C++17 with Qt/QML
- Medical imaging workflow concepts: slice loading, overlay alignment, depth measurement
- Simulated and external telemetry paths
- Recording/playback and export workflow
- Test automation and safety-process artifacts

## Recommended evaluation flow

1. Launch the packaged app.
2. Load the bundled sample slice folder.
3. Start simulated tracking and move the target.
4. Drag a measurement across the viewer.
5. Load the bundled sample session and play it back.
6. Switch to external source mode.
7. Start the TCP listener and send demo telemetry.
8. Review the safety documents in `docs/safety/`.

## Expected reviewer takeaways

- The codebase is organized into imaging, tracking, measurement, recording, and app/controller layers.
- The transport abstraction supports future robotics or tracker integration.
- The repository includes CI, tests, demo tooling, and safety-oriented documentation.

## Limitations to understand

- DICOM decoding is scaffolded and falls back to simulated grayscale without full DCMTK integration.
- External telemetry integrations are prototype-level transports, not validated clinical interfaces.
- Safety artifacts are starter documents, not a complete regulated submission package.
