#include "tracking/InstrumentTracker.h"

#include <QDateTime>
#include <QtMath>

#include "measurement/DepthMeasurement.h"

namespace surgiview {

InstrumentTracker::InstrumentTracker(QObject* parent)
    : QObject(parent)
{
    m_timer.setInterval(40);
    connect(&m_timer, &QTimer::timeout, this, &InstrumentTracker::onTick);
}

bool InstrumentTracker::running() const
{
    return m_running;
}

bool InstrumentTracker::labTestingMode() const
{
    return m_labTestingMode;
}

void InstrumentTracker::setLabTestingMode(bool enabled)
{
    if (m_labTestingMode == enabled) {
        return;
    }
    m_labTestingMode = enabled;
    emit labTestingModeChanged();
}

void InstrumentTracker::setTarget(const QPointF& p)
{
    m_target = p;
}

void InstrumentTracker::start()
{
    if (m_running) {
        return;
    }
    m_running = true;
    m_timer.start();
    emit runningChanged();
}

void InstrumentTracker::stop()
{
    if (!m_running) {
        return;
    }
    m_running = false;
    m_timer.stop();
    emit runningChanged();
}

void InstrumentTracker::onTick()
{
    // Simulate marker drift and instrument movement for lab/C-arm testing mode.
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

    TrackerSample sample;
    sample.timestampMs = QDateTime::currentMSecsSinceEpoch();
    sample.toolTip = m_toolTip;
    sample.target = m_target;
    sample.depthMm = m_depthMm;
    sample.alignmentErrorPx = DepthMeasurement::distancePx(m_toolTip, m_target);

    emit sampleReady(sample);
}

} // namespace surgiview
