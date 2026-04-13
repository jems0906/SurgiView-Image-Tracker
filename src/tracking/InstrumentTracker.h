#pragma once

#include <QObject>
#include <QPointF>
#include <QTimer>
#include <memory>

#include "models/Types.h"
#include "tracking/TrackingPoseSource.h"

namespace surgiview {

class InstrumentTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool labTestingMode READ labTestingMode WRITE setLabTestingMode NOTIFY labTestingModeChanged)
    Q_PROPERTY(QString sourceMode READ sourceMode WRITE setSourceMode NOTIFY sourceModeChanged)

public:
    explicit InstrumentTracker(QObject* parent = nullptr);

    bool running() const;
    bool labTestingMode() const;
    QString sourceMode() const;

    void setLabTestingMode(bool enabled);
    void setTarget(const QPointF& p);
    void setSourceMode(const QString& mode);
    void ingestExternalTelemetry(double x, double y, double depthMm);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void runningChanged();
    void labTestingModeChanged();
    void sourceModeChanged();
    void sampleReady(const surgiview::TrackerSample& sample);

private slots:
    void onTick();

private:
    enum class SourceMode {
        Simulated,
        ExternalTelemetry
    };

    bool m_running = false;
    bool m_labTestingMode = true;
    QTimer m_timer;
    QPointF m_target = QPointF(300.0, 220.0);

    SourceMode m_sourceMode = SourceMode::Simulated;
    std::unique_ptr<SimulatedPoseSource> m_simulatedSource;
    std::unique_ptr<ExternalTelemetryPoseSource> m_externalSource;
};

} // namespace surgiview
