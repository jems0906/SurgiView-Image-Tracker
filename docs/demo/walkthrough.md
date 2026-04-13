# Demo Walkthrough

## Goal

Show SurgiView as a desktop medical imaging prototype with image loading, tracking overlay, depth measurement, recording/playback, and live telemetry integration.

## Demo assets

- CT slice folder: `sample-data/demo_ct_series`
- Session replay file: `sample-data/demo_session.json`
- Live sender: `scripts/send_demo_telemetry.py`

## Recommended demo flow

1. Run the app.
2. Load `sample-data/demo_ct_series`.
3. Click `Start Tracking` in simulated mode.
4. Click the viewer to move the target and show alignment error updates.
5. Drag in the viewer to show depth/measurement values.
6. Start recording, let motion run briefly, then stop recording.
7. Save or load `sample-data/demo_session.json` and show playback.
8. Switch source mode to `external`.
9. Start the TCP listener on port `45454`.
10. Run the TCP demo sender task.
11. Show the overlay following live telemetry.
12. Optionally demonstrate serial mode by switching to a COM port and baud rate.

## Talking points

- Qt/QML desktop app in C++17
- Imaging + tracking + measurement in one workflow
- Safety-oriented artifacts in `docs/safety/`
- Automated build/test and demo sender utilities
- External transport abstraction for future robotics/camera integration
