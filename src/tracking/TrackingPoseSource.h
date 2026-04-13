#pragma once

#include <QPointF>

namespace surgiview {

struct PoseFrame {
    QPointF toolTip;
    double depthMm = 0.0;
};

class TrackingPoseSource {
public:
    virtual ~TrackingPoseSource() = default;
    virtual bool poll(PoseFrame* frame) = 0;
};

class SimulatedPoseSource : public TrackingPoseSource {
public:
    void setLabTestingMode(bool enabled);
    bool poll(PoseFrame* frame) override;

private:
    bool m_labTestingMode = true;
    QPointF m_toolTip = QPointF(120.0, 120.0);
    QPointF m_velocity = QPointF(2.5, 1.8);
    double m_depthMm = 0.0;
};

class ExternalTelemetryPoseSource : public TrackingPoseSource {
public:
    void ingest(double x, double y, double depthMm);
    bool poll(PoseFrame* frame) override;

private:
    bool m_hasLatestFrame = false;
    PoseFrame m_latestFrame;
};

} // namespace surgiview
