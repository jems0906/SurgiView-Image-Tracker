# SurgiView Image Tracker Portfolio Summary

## Project snapshot

SurgiView Image Tracker is a desktop surgical imaging prototype that combines slice viewing, instrument overlay, depth measurement, telemetry ingestion, and session replay in a single Qt/QML application. It is designed to read as a serious engineering project rather than a narrow UI mockup.

## What this demonstrates

- C++17 and Qt/QML application architecture for a stateful desktop workflow
- Separation of concerns across imaging, tracking, measurement, recording, and presentation layers
- Real-time UI behavior driven by simulated or live external telemetry sources
- Practical engineering workflow with CMake, Git, CI, tests, documentation, and packaging
- Awareness of medical-device software expectations through traceability and hazard-analysis artifacts

## Strong portfolio signals

- Built from an empty workspace into a runnable, tested, packaged application
- Supports multiple operating modes: simulated tracking, TCP telemetry, and serial telemetry
- Includes operator-facing demo tooling, sample datasets, and evaluator documentation
- Adds export and replay flows instead of stopping at a static visualization demo
- Uses a modular transport design that can be extended to camera or robotics adapters

## Scope and framing

This is a prototype for medical imaging and instrument-tracking workflows, not a clinically validated navigation system. The project is intentionally framed to show system design, desktop UX, integration readiness, and engineering discipline.

## Suggested interview framing

Use this project to discuss:

- how UI state and streaming telemetry are coordinated in Qt
- how prototype subsystems were isolated so hardware adapters can be swapped in later
- where prototype shortcuts exist and how they would be hardened for production
- how testing and safety artifacts were introduced early instead of after implementation

## Best supporting materials

- Demo walkthrough: `docs/demo/walkthrough.md`
- Evaluator guide: `docs/demo/evaluator-guide.md`
- Release note: `docs/demo/release-note-v1.md`
- Safety artifacts: `docs/safety/`
