#pragma once

#include <QObject>
#include <QPointF>

#include "imaging/DicomSeriesLoader.h"
#include "models/Types.h"
#include "recording/PlaybackRecorder.h"
#include "tracking/InstrumentTracker.h"

namespace surgiview {

class SurgiViewController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(int frameRevision READ frameRevision NOTIFY frameUpdated)
    Q_PROPERTY(QPointF toolTip READ toolTip NOTIFY overlayChanged)
    Q_PROPERTY(QPointF targetPoint READ targetPoint NOTIFY overlayChanged)
    Q_PROPERTY(double alignmentErrorPx READ alignmentErrorPx NOTIFY overlayChanged)
    Q_PROPERTY(double depthMm READ depthMm NOTIFY overlayChanged)
    Q_PROPERTY(double measurementPx READ measurementPx NOTIFY measurementChanged)
    Q_PROPERTY(double measurementMm READ measurementMm NOTIFY measurementChanged)
    Q_PROPERTY(int currentSliceIndex READ currentSliceIndex NOTIFY sliceChanged)
    Q_PROPERTY(int totalSlices READ totalSlices NOTIFY sliceChanged)
    Q_PROPERTY(bool trackerRunning READ trackerRunning NOTIFY trackerRunningChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)

public:
    explicit SurgiViewController(QObject* parent = nullptr);

    QString statusMessage() const;
    int frameRevision() const;

    QPointF toolTip() const;
    QPointF targetPoint() const;
    double alignmentErrorPx() const;
    double depthMm() const;

    double measurementPx() const;
    double measurementMm() const;

    int currentSliceIndex() const;
    int totalSlices() const;

    bool trackerRunning() const;
    bool recording() const;

    QImage currentImage() const;

    Q_INVOKABLE void loadSliceFolder(const QString& folderPath);
    Q_INVOKABLE void nextSlice();
    Q_INVOKABLE void previousSlice();

    Q_INVOKABLE void startTracking();
    Q_INVOKABLE void stopTracking();
    Q_INVOKABLE void setLabTestingMode(bool enabled);

    Q_INVOKABLE void setTargetPoint(double x, double y);
    Q_INVOKABLE void setMeasurementStart(double x, double y);
    Q_INVOKABLE void setMeasurementEnd(double x, double y);

    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
    Q_INVOKABLE void saveRecording(const QString& jsonPath);
    Q_INVOKABLE void loadRecording(const QString& jsonPath);
    Q_INVOKABLE void exportAnnotatedFrames(const QString& outputFolder);
    Q_INVOKABLE void exportAnnotatedVideo(const QString& outputFolder, const QString& videoPath);
    Q_INVOKABLE void startPlayback();
    Q_INVOKABLE void pausePlayback();

signals:
    void statusMessageChanged();
    void frameUpdated();
    void overlayChanged();
    void measurementChanged();
    void sliceChanged();
    void trackerRunningChanged();
    void recordingChanged();

private slots:
    void onTrackerSample(const surgiview::TrackerSample& sample);
    void onPlaybackSample(const surgiview::TrackerSample& sample);

private:
    void applySample(const TrackerSample& sample);
    void refreshFrame();

    DicomSeriesLoader m_loader;
    InstrumentTracker m_tracker;
    PlaybackRecorder m_recorder;

    QString m_statusMessage;
    int m_frameRevision = 0;

    QPointF m_toolTip = QPointF(120.0, 120.0);
    QPointF m_targetPoint = QPointF(300.0, 220.0);
    double m_alignmentErrorPx = 0.0;
    double m_depthMm = 0.0;

    QPointF m_measurementStart = QPointF(120.0, 120.0);
    QPointF m_measurementEnd = QPointF(220.0, 120.0);
    double m_measurementPx = 0.0;
    double m_measurementMm = 0.0;
};

} // namespace surgiview
