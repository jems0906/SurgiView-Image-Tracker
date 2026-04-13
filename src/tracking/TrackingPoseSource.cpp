#include "tracking/TrackingPoseSource.h"

namespace surgiview {

void SimulatedPoseSource::setLabTestingMode(bool enabled)
{
    m_labTestingMode = enabled;
}

bool SimulatedPoseSource::poll(PoseFrame* frame)
{
    if (frame == nullptr) {
        return false;
    }

    const double driftFactor = m_labTestingMode ? 1.4 : 1.0;

    m_toolTip.rx() += m_velocity.x() * driftFactor;
    m_toolTip.ry() += m_velocity.y() * driftFactor;

    if (m_toolTip.x() < 10.0 || m_toolTip.x() > 620.0) {
        m_velocity.rx() = -m_velocity.x();
    }
    if (m_toolTip.y() < 10.0 || m_toolTip.y() > 470.0) {
        m_velocity.ry() = -m_velocity.y();
    }

    m_depthMm += 0.12 * driftFactor;
    if (m_depthMm > 85.0) {
        m_depthMm = 0.0;
    }

    frame->toolTip = m_toolTip;
    frame->depthMm = m_depthMm;
    return true;
}

void ExternalTelemetryPoseSource::ingest(double x, double y, double depthMm)
{
    m_latestFrame.toolTip = QPointF(x, y);
    m_latestFrame.depthMm = depthMm;
    m_hasLatestFrame = true;
}

bool ExternalTelemetryPoseSource::poll(PoseFrame* frame)
{
    if (frame == nullptr || !m_hasLatestFrame) {
        return false;
    }

    *frame = m_latestFrame;
    return true;
}

} // namespace surgiview
