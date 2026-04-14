# SurgiView Image Tracker GitHub Release Draft

## SurgiView Image Tracker v1

SurgiView Image Tracker is a C++17 Qt/QML desktop prototype for surgical imaging workflows. It combines slice viewing, instrument overlay, depth measurement, recording and playback, annotated export, and external telemetry ingestion in a single medical-software-focused demo application.

## Highlights

- Qt 6 and QML desktop application architecture in C++17
- Baseline DICOM support for common uncompressed monochrome Explicit VR Little Endian studies
- Simulated tracking plus external telemetry ingestion over TCP and serial JSON streams
- Real-time overlay alignment between tool tip and target anatomy
- Interactive pixel and millimeter measurement tools
- Recording, playback, annotated frame export, and ffmpeg-based video export
- Windows CI, Qt Test coverage, packaging scripts, and demo walkthrough assets
- IEC 62304-style starter safety artifacts for prototype traceability and review

## Included reviewer assets

- Sample slice data in `sample-data/demo_ct_series`
- Sample replay session in `sample-data/demo_session.json`
- Demo walkthrough in `docs/demo/walkthrough.md`
- Evaluator guide in `docs/demo/evaluator-guide.md`
- Requirement traceability checklist in `docs/demo/requirements-checklist.md`

## Validation status

- Project builds successfully with the validated local MinGW plus Ninja workflow
- Automated test suite is passing, including measurement, tracking, telemetry, and DICOM decoder coverage
- Application launch has been verified locally
- Demo packaging flow has been verified, including zip bundle creation

## Known limits

- This is a prototype, not a clinically validated navigation product
- Full production-grade DICOM coverage still benefits from enabling DCMTK integration
- External telemetry inputs are demo-friendly transports, not certified device interfaces

## Suggested release notes footer

Recommended first reads for reviewers:

- `docs/demo/portfolio-summary.md`
- `docs/demo/evaluator-guide.md`
- `docs/demo/requirements-checklist.md`
