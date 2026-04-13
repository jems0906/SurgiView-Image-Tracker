# SurgiView Image Tracker Release Note v1

## Summary

SurgiView Image Tracker is a C++17 Qt/QML desktop prototype for medical image visualization with real-time instrument overlay simulation. This release packages the project as a portfolio-quality demo with sample data, external telemetry inputs, recording and playback, annotated export, automated tests, and safety-oriented documentation.

## Included in this release

- Desktop application built with Qt 6, QML, and C++17
- Sample imaging dataset under `sample-data/demo_ct_series`
- Sample recorded session under `sample-data/demo_session.json`
- Simulated and external tracking workflows
- TCP and serial JSON telemetry ingestion paths
- Recording, playback, annotated frame export, and ffmpeg-based video export
- Windows CI pipeline and Qt Test coverage for core behavior
- IEC 62304-style starter safety artifacts under `docs/safety/`
- Demo packaging and launch helper scripts under `scripts/`

## Key engineering highlights

- Modular architecture split across imaging, tracking, measurement, recording, and controller layers
- Transport abstraction that isolates live telemetry ingestion from UI and overlay logic
- Optional DCMTK scaffold for future production-grade DICOM decoding work
- Repeatable build, test, run, and packaging workflows for local evaluation

## Known limitations

- Native DICOM handling remains prototype-level unless DCMTK is enabled and provisioned
- External telemetry integrations are simulation-friendly transports, not certified device interfaces
- Safety documentation is intended as process evidence for a prototype, not a regulatory submission

## Recommended reviewer path

1. Launch the app or packaged demo bundle.
2. Load the sample slice folder.
3. Demonstrate simulated tracking and interactive target movement.
4. Show measurement and depth readouts.
5. Play back the bundled sample session.
6. Switch to external mode and drive the overlay using the demo sender.
7. Review the evaluator guide and safety documents.

## Reference documents

- `docs/demo/evaluator-guide.md`
- `docs/demo/portfolio-summary.md`
- `docs/demo/walkthrough.md`
