#include "app/SurgiViewController.h"

#include "measurement/DepthMeasurement.h"

namespace surgiview {

SurgiViewController::SurgiViewController(QObject* parent)
    : QObject(parent)
{
    connect(&m_tracker, &InstrumentTracker::sampleReady, this, &SurgiViewController::onTrackerSample);
    connect(&m_tracker, &InstrumentTracker::runningChanged, this, &SurgiViewController::trackerRunningChanged);
    connect(&m_tracker, &InstrumentTracker::sourceModeChanged, this, &SurgiViewController::trackingSourceModeChanged);

    connect(&m_telemetryServer, &TcpTelemetryServer::telemetryReceived, this, &SurgiViewController::ingestExternalTelemetry);
    connect(&m_telemetryServer, &TcpTelemetryServer::serverMessage, this, &SurgiViewController::onTelemetryServerMessage);
    connect(&m_telemetryServer, &TcpTelemetryServer::listeningChanged, this, &SurgiViewController::telemetryServerListeningChanged);

    connect(&m_serialTelemetry, &SerialTelemetryDevice::telemetryReceived, this, &SurgiViewController::ingestExternalTelemetry);
    connect(&m_serialTelemetry, &SerialTelemetryDevice::deviceMessage, this, &SurgiViewController::onSerialTelemetryMessage);
    connect(&m_serialTelemetry, &SerialTelemetryDevice::openChanged, this, &SurgiViewController::serialTelemetryOpenChanged);

    connect(&m_recorder, &PlaybackRecorder::recordingChanged, this, &SurgiViewController::recordingChanged);
    connect(&m_recorder, &PlaybackRecorder::playbackSampleReady, this, &SurgiViewController::onPlaybackSample);

    m_statusMessage = QStringLiteral("Ready. Load X-ray/CT slices to begin.");
    setMeasurementStart(m_measurementStart.x(), m_measurementStart.y());
    setMeasurementEnd(m_measurementEnd.x(), m_measurementEnd.y());
}

QString SurgiViewController::statusMessage() const
{
    return m_statusMessage;
}

int SurgiViewController::frameRevision() const
{
    return m_frameRevision;
}

QPointF SurgiViewController::toolTip() const
{
    return m_toolTip;
}

QPointF SurgiViewController::targetPoint() const
{
    return m_targetPoint;
}

double SurgiViewController::alignmentErrorPx() const
{
    return m_alignmentErrorPx;
}

double SurgiViewController::depthMm() const
{
    return m_depthMm;
}

double SurgiViewController::measurementPx() const
{
    return m_measurementPx;
}

double SurgiViewController::measurementMm() const
{
    return m_measurementMm;
}

int SurgiViewController::currentSliceIndex() const
{
    return m_loader.currentIndex();
}

int SurgiViewController::totalSlices() const
{
    return m_loader.frameCount();
}

bool SurgiViewController::trackerRunning() const
{
    return m_tracker.running();
}

QString SurgiViewController::trackingSourceMode() const
{
    return m_tracker.sourceMode();
}

bool SurgiViewController::telemetryServerListening() const
{
    return m_telemetryServer.isListening();
}

int SurgiViewController::telemetryServerPort() const
{
    return static_cast<int>(m_telemetryServer.port());
}

bool SurgiViewController::serialTelemetryOpen() const
{
    return m_serialTelemetry.isOpen();
}

QString SurgiViewController::serialTelemetryPortName() const
{
    return m_serialTelemetry.portName();
}

int SurgiViewController::serialTelemetryBaudRate() const
{
    return static_cast<int>(m_serialTelemetry.baudRate());
}

bool SurgiViewController::recording() const
{
    return m_recorder.recording();
}

QImage SurgiViewController::currentImage() const
{
    if (!m_loader.hasFrames()) {
        QImage placeholder(960, 540, QImage::Format_RGB32);
        placeholder.fill(QColor(26, 28, 34));
        return placeholder;
    }
    return m_loader.currentFrame().image;
}

void SurgiViewController::loadSliceFolder(const QString& folderPath)
{
    QString error;
    if (!m_loader.loadFolder(folderPath, &error)) {
        m_statusMessage = QStringLiteral("Load failed: %1").arg(error);
        emit statusMessageChanged();
        return;
    }

    m_statusMessage = QStringLiteral("Loaded %1 slices").arg(m_loader.frameCount());
    emit statusMessageChanged();
    emit sliceChanged();
    refreshFrame();
}

void SurgiViewController::nextSlice()
{
    if (m_loader.next()) {
        emit sliceChanged();
        refreshFrame();
    }
}

void SurgiViewController::previousSlice()
{
    if (m_loader.previous()) {
        emit sliceChanged();
        refreshFrame();
    }
}

void SurgiViewController::startTracking()
{
    m_tracker.start();
    m_statusMessage = QStringLiteral("Tracking started");
    emit statusMessageChanged();
}

void SurgiViewController::stopTracking()
{
    m_tracker.stop();
    m_statusMessage = QStringLiteral("Tracking stopped");
    emit statusMessageChanged();
}

void SurgiViewController::setLabTestingMode(bool enabled)
{
    m_tracker.setLabTestingMode(enabled);
    m_statusMessage = enabled
        ? QStringLiteral("Lab testing mode: simulated C-arm drift enabled")
        : QStringLiteral("Lab testing mode disabled");
    emit statusMessageChanged();
}

void SurgiViewController::setTrackingSourceMode(const QString& mode)
{
    m_tracker.setSourceMode(mode);
    m_statusMessage = QStringLiteral("Tracking source mode: %1").arg(m_tracker.sourceMode());
    emit statusMessageChanged();
}

void SurgiViewController::ingestExternalTelemetry(double x, double y, double depthMm)
{
    m_tracker.ingestExternalTelemetry(x, y, depthMm);
    if (m_tracker.sourceMode() == QStringLiteral("external")) {
        m_statusMessage = QStringLiteral("External telemetry ingested");
        emit statusMessageChanged();
    }
}

void SurgiViewController::startTelemetryServer(int port)
{
    QString error;
    if (!m_telemetryServer.start(static_cast<quint16>(port), &error)) {
        m_statusMessage = QStringLiteral("Telemetry server start failed: %1").arg(error);
        emit statusMessageChanged();
        return;
    }

    m_statusMessage = QStringLiteral("Telemetry server listening on port %1").arg(m_telemetryServer.port());
    emit statusMessageChanged();
}

void SurgiViewController::stopTelemetryServer()
{
    m_telemetryServer.stop();
    m_statusMessage = QStringLiteral("Telemetry server stopped");
    emit statusMessageChanged();
}

void SurgiViewController::startSerialTelemetry(const QString& portName, int baudRate)
{
    QString error;
    if (!m_serialTelemetry.start(portName, static_cast<qint32>(baudRate), &error)) {
        m_statusMessage = QStringLiteral("Serial telemetry start failed: %1").arg(error);
        emit statusMessageChanged();
        return;
    }

    m_statusMessage = QStringLiteral("Serial telemetry opened on %1 @ %2 baud").arg(portName).arg(baudRate);
    emit statusMessageChanged();
}

void SurgiViewController::stopSerialTelemetry()
{
    m_serialTelemetry.stop();
    m_statusMessage = QStringLiteral("Serial telemetry stopped");
    emit statusMessageChanged();
}

void SurgiViewController::setTargetPoint(double x, double y)
{
    m_targetPoint = QPointF(x, y);
    m_tracker.setTarget(m_targetPoint);
    m_alignmentErrorPx = DepthMeasurement::distancePx(m_toolTip, m_targetPoint);
    emit overlayChanged();
}

void SurgiViewController::setMeasurementStart(double x, double y)
{
    m_measurementStart = QPointF(x, y);
    m_measurementPx = DepthMeasurement::distancePx(m_measurementStart, m_measurementEnd);
    m_measurementMm = DepthMeasurement::distanceMm(
        m_measurementStart,
        m_measurementEnd,
        m_loader.hasFrames() ? m_loader.currentFrame().pixelSpacingMm : 0.2);
    emit measurementChanged();
}

void SurgiViewController::setMeasurementEnd(double x, double y)
{
    m_measurementEnd = QPointF(x, y);
    m_measurementPx = DepthMeasurement::distancePx(m_measurementStart, m_measurementEnd);
    m_measurementMm = DepthMeasurement::distanceMm(
        m_measurementStart,
        m_measurementEnd,
        m_loader.hasFrames() ? m_loader.currentFrame().pixelSpacingMm : 0.2);
    emit measurementChanged();
}

void SurgiViewController::startRecording()
{
    m_recorder.startRecording();
    m_statusMessage = QStringLiteral("Recording started");
    emit statusMessageChanged();
}

void SurgiViewController::stopRecording()
{
    m_recorder.stopRecording();
    m_statusMessage = QStringLiteral("Recording stopped");
    emit statusMessageChanged();
}

void SurgiViewController::saveRecording(const QString& jsonPath)
{
    QString error;
    if (!m_recorder.saveSession(jsonPath, &error)) {
        m_statusMessage = QStringLiteral("Save failed: %1").arg(error);
    } else {
        m_statusMessage = QStringLiteral("Session saved: %1").arg(jsonPath);
    }
    emit statusMessageChanged();
}

void SurgiViewController::loadRecording(const QString& jsonPath)
{
    QString error;
    if (!m_recorder.loadSession(jsonPath, &error)) {
        m_statusMessage = QStringLiteral("Load session failed: %1").arg(error);
    } else {
        m_statusMessage = QStringLiteral("Session loaded: %1").arg(jsonPath);
    }
    emit statusMessageChanged();
}

void SurgiViewController::exportAnnotatedFrames(const QString& outputFolder)
{
    QString error;
    if (!m_recorder.exportAnnotatedFrames(outputFolder, &error)) {
        m_statusMessage = QStringLiteral("Export failed: %1").arg(error);
    } else {
        m_statusMessage = QStringLiteral("Annotated frames exported: %1").arg(outputFolder);
    }
    emit statusMessageChanged();
}

void SurgiViewController::exportAnnotatedVideo(const QString& outputFolder, const QString& videoPath)
{
    QString error;
    if (!m_recorder.exportAnnotatedVideo(outputFolder, videoPath, &error)) {
        m_statusMessage = QStringLiteral("Video export failed: %1").arg(error);
    } else {
        m_statusMessage = QStringLiteral("Annotated video exported: %1").arg(videoPath);
    }
    emit statusMessageChanged();
}

void SurgiViewController::startPlayback()
{
    m_recorder.play();
    m_statusMessage = QStringLiteral("Playback started");
    emit statusMessageChanged();
}

void SurgiViewController::pausePlayback()
{
    m_recorder.pause();
    m_statusMessage = QStringLiteral("Playback paused");
    emit statusMessageChanged();
}

void SurgiViewController::onTrackerSample(const surgiview::TrackerSample& sample)
{
    applySample(sample);
    m_recorder.appendSample(sample);
}

void SurgiViewController::onPlaybackSample(const surgiview::TrackerSample& sample)
{
    applySample(sample);
}

void SurgiViewController::applySample(const TrackerSample& sample)
{
    m_toolTip = sample.toolTip;
    m_targetPoint = sample.target;
    m_depthMm = sample.depthMm;
    m_alignmentErrorPx = sample.alignmentErrorPx;
    emit overlayChanged();
    refreshFrame();
}

void SurgiViewController::refreshFrame()
{
    ++m_frameRevision;
    emit frameUpdated();
}

void SurgiViewController::onTelemetryServerMessage(const QString& message)
{
    m_statusMessage = message;
    emit statusMessageChanged();
}

void SurgiViewController::onSerialTelemetryMessage(const QString& message)
{
    m_statusMessage = message;
    emit statusMessageChanged();
}

} // namespace surgiview
