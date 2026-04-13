# Release and Demo Checklist

## Build and regression

- [ ] `run.ps1 -All` completes successfully
- [ ] GitHub Actions CI passes on target branch
- [ ] Working tree is clean before tagging/release packaging

## Imaging and measurement

- [ ] Slice loading verified with representative folder
- [ ] Measurement px/mm outputs manually spot-checked
- [ ] DICOM fallback or real decoder path verified for intended demo dataset

## Tracking and overlay

- [ ] Simulated mode verified
- [ ] External telemetry mode verified or disabled explicitly
- [ ] Alignment error readout visible during demo flow
- [ ] Lab/testing mode clearly indicated when enabled

## Recording and evidence

- [ ] Recording start/stop verified
- [ ] Session save/load verified
- [ ] Annotated frame export verified
- [ ] Video export verified or ffmpeg absence documented

## Safety review

- [ ] Hazard analysis reviewed for current feature scope
- [ ] Traceability matrix updated for changed requirements
- [ ] Verification protocol execution evidence attached
- [ ] Open known issues reviewed and dispositioned

## Demo/operator readiness

- [ ] Target workflow script prepared
- [ ] Sample data available locally
- [ ] Failure-mode explanation prepared for simulated or prototype-only functions
- [ ] Non-clinical prototype disclaimer included in demo materials
