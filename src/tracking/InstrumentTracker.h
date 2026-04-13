#pragma once

#include <QObject>
#include <QPointF>
#include <QTimer>

#include "models/Types.h"

namespace surgiview {

class InstrumentTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool labTestingMode READ labTestingMode WRITE setLabTestingMode NOTIFY labTestingModeChanged)

public:
    explicit InstrumentTracker(QObject* parent = nullptr);

    bool running() const;
    bool labTestingMode() const;

    void setLabTestingMode(bool enabled);
    void setTarget(const QPointF& p);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void runningChanged();
    void labTestingModeChanged();
    void sampleReady(const surgiview::TrackerSample& sample);

private slots:
    void onTick();

private:
    bool m_running = false;
    bool m_labTestingMode = true;
    QTimer m_timer;
    QPointF m_toolTip = QPointF(120.0, 120.0);
    QPointF m_target = QPointF(300.0, 220.0);
    QPointF m_velocity = QPointF(2.5, 1.8);
    double m_depthMm = 0.0;
};

} // namespace surgiview
