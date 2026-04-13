# SurgiView Safety Artifact Pack

This folder contains baseline software lifecycle artifacts aligned to IEC 62304-style development practices for the SurgiView Image Tracker prototype.

## Included artifacts

- `hazard-analysis.md`: preliminary software hazard and risk control analysis
- `traceability-matrix.csv`: high-level linkage between requirements, hazards, risk controls, and verification evidence
- `verification-protocol.md`: repeatable verification activities for bench/lab simulation
- `release-checklist.md`: pre-demo and pre-release quality gate checklist

## Intended use

These artifacts are intended to demonstrate process maturity and provide a starting point for regulated medical software workflows. They are not a substitute for a full quality management system, clinical validation, cybersecurity review, or formal risk management file.

## Update rules

- Update the traceability matrix when adding or changing requirements.
- Link each new safety-relevant control to a verification activity.
- Capture test evidence, test date, operator, and result for each executed protocol.
- Review hazard severity/probability assumptions when integrating real hardware or real DICOM decoding.
