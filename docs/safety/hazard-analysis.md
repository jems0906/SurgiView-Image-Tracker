# Preliminary Hazard Analysis

## Scope

Software-only preliminary analysis for the SurgiView Image Tracker prototype, focused on image display, tracking overlay, measurements, recording/playback, and simulated lab workflows.

## Risk scale

Severity:
- `S1`: negligible
- `S2`: minor injury or workflow delay
- `S3`: serious injury possible
- `S4`: critical injury possible

Probability:
- `P1`: remote
- `P2`: occasional
- `P3`: probable

## Hazard table

| ID | Hazard | Sequence of events | Potential harm | Initial risk | Software risk controls |
| --- | --- | --- | --- | --- | --- |
| H-001 | Misregistered overlay | Tracking source drift, stale target, or incorrect transform causes overlay offset | Wrong anatomy targeting or misplaced tool guidance | S4/P2 | Display alignment error numerically, require explicit target selection, lab-mode simulation labels, future registration tolerance alarm |
| H-002 | Incorrect depth measurement | Pixel spacing wrong, measurement endpoints misplaced, or DICOM spacing invalid | Incorrect screw depth decision | S3/P2 | Show both px and mm values, default spacing fallback, future DICOM metadata validation and operator confirmation |
| H-003 | Stale external telemetry | Telemetry source stops updating but overlay remains visible | Operator acts on outdated tool position | S4/P2 | Future heartbeat timeout, age-of-data indicator, current architecture supports source-mode specific validation |
| H-004 | Wrong slice displayed | Navigation error or dataset ordering issue | Inaccurate anatomical interpretation | S3/P2 | Slice index display, ordered folder loading, future metadata-based series sorting |
| H-005 | Missing recorded evidence | Recording/export failure not noticed | Loss of review evidence and inability to reconstruct procedure | S2/P2 | Status messages for recording/export outcomes, session save/load verification, future audit log and storage integrity check |
| H-006 | Simulated mode mistaken for live mode | Lab/testing mode used inappropriately during demonstration or integration | Misleading guidance or false confidence | S3/P1 | Explicit lab testing mode labeling, separate source mode selector, future startup interlock for non-clinical mode |
| H-007 | Corrupt DICOM decode | Unsupported pixel format or malformed file renders invalid image | Misinterpretation of anatomy | S3/P1 | Fallback simulation path today, future decoder validation, metadata sanity checks, and reject-on-error policy |

## Current software safety posture

Implemented controls in repository today:
- Visible alignment error readout
- Visible depth readout
- Explicit source mode selection
- Explicit lab testing mode toggle
- Unit tests covering measurement and tracking sample generation
- Recording/export status feedback

## Highest-priority future controls

1. Telemetry freshness timeout and stale-data banner.
2. DICOM metadata validation including pixel spacing and modality checks.
3. Registration quality threshold alarm before overlay display.
4. Persistent audit logging for session, source mode, and exported evidence.
5. Formal review/approval signatures for verification evidence.
