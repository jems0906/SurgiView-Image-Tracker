#include "tracking/InstrumentTracker.h"

#include <QDateTime>
#include <QtMath>

#include "measurement/DepthMeasurement.h"

namespace surgiview {

InstrumentTracker::InstrumentTracker(QObject* parent)
    : QObject(parent)
    , m_simulatedSource(std::make_unique<SimulatedPoseSource>())
    , m_externalSource(std::make_unique<ExternalTelemetryPoseSource>())
{
    m_timer.setInterval(40);
    connect(&m_timer, &QTimer::timeout, this, &InstrumentTracker::onTick);
    m_simulatedSource->setLabTestingMode(m_labTestingMode);
}

bool InstrumentTracker::running() const
{
    return m_running;
}

bool InstrumentTracker::labTestingMode() const
{
    return m_labTestingMode;
}

QString InstrumentTracker::sourceMode() const
{
    return m_sourceMode == SourceMode::Simulated
        ? QStringLiteral("simulated")
        : QStringLiteral("external");
}

void InstrumentTracker::setLabTestingMode(bool enabled)
{
    if (m_labTestingMode == enabled) {
        return;
    }
    m_labTestingMode = enabled;
    m_simulatedSource->setLabTestingMode(enabled);
    emit labTestingModeChanged();
}

void InstrumentTracker::setTarget(const QPointF& p)
{
    m_target = p;
}

void InstrumentTracker::setSourceMode(const QString& mode)
{
    SourceMode nextMode = SourceMode::Simulated;
    if (mode.compare(QStringLiteral("external"), Qt::CaseInsensitive) == 0) {
        nextMode = SourceMode::ExternalTelemetry;
    }

    if (m_sourceMode == nextMode) {
        return;
    }

    m_sourceMode = nextMode;
    emit sourceModeChanged();
}

void InstrumentTracker::ingestExternalTelemetry(double x, double y, double depthMm)
{
    m_externalSource->ingest(x, y, depthMm);
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
    PoseFrame frame;
    const bool hasFrame = (m_sourceMode == SourceMode::Simulated)
        ? m_simulatedSource->poll(&frame)
        : m_externalSource->poll(&frame);

    if (!hasFrame) {
        return;
    }

    TrackerSample sample;
    sample.timestampMs = QDateTime::currentMSecsSinceEpoch();
    sample.toolTip = frame.toolTip;
    sample.target = m_target;
    sample.depthMm = frame.depthMm;
    sample.alignmentErrorPx = DepthMeasurement::distancePx(frame.toolTip, m_target);

    emit sampleReady(sample);
}

} // namespace surgiview
