#include "recording/PlaybackRecorder.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QProcess>

namespace surgiview {

PlaybackRecorder::PlaybackRecorder(QObject* parent)
    : QObject(parent)
{
    m_playbackTimer.setInterval(40);
    connect(&m_playbackTimer, &QTimer::timeout, this, &PlaybackRecorder::onPlaybackTick);
}

bool PlaybackRecorder::recording() const
{
    return m_recording;
}

bool PlaybackRecorder::playingBack() const
{
    return m_playingBack;
}

void PlaybackRecorder::appendSample(const TrackerSample& sample)
{
    if (!m_recording) {
        return;
    }
    m_samples.push_back(sample);
}

void PlaybackRecorder::startRecording()
{
    clear();
    m_recording = true;
    emit recordingChanged();
}

void PlaybackRecorder::stopRecording()
{
    if (!m_recording) {
        return;
    }
    m_recording = false;
    emit recordingChanged();
}

void PlaybackRecorder::clear()
{
    m_samples.clear();
    m_playbackIndex = 0;
}

bool PlaybackRecorder::saveSession(const QString& outputJsonPath, QString* errorMessage) const
{
    QJsonArray samplesArray;
    for (const TrackerSample& s : m_samples) {
        QJsonObject sampleObj;
        sampleObj["timestampMs"] = static_cast<qint64>(s.timestampMs);
        sampleObj["toolX"] = s.toolTip.x();
        sampleObj["toolY"] = s.toolTip.y();
        sampleObj["targetX"] = s.target.x();
        sampleObj["targetY"] = s.target.y();
        sampleObj["depthMm"] = s.depthMm;
        sampleObj["alignmentErrorPx"] = s.alignmentErrorPx;
        samplesArray.push_back(sampleObj);
    }

    QJsonObject root;
    root["samples"] = samplesArray;

    QFile file(outputJsonPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Cannot write session file: %1").arg(outputJsonPath);
        }
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool PlaybackRecorder::loadSession(const QString& inputJsonPath, QString* errorMessage)
{
    QFile file(inputJsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Cannot open session file: %1").arg(inputJsonPath);
        }
        return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Invalid session JSON format");
        }
        return false;
    }

    const QJsonArray samplesArray = doc.object().value("samples").toArray();
    m_samples.clear();
    for (const QJsonValue& value : samplesArray) {
        const QJsonObject obj = value.toObject();
        TrackerSample s;
        s.timestampMs = obj.value("timestampMs").toInteger();
        s.toolTip = QPointF(obj.value("toolX").toDouble(), obj.value("toolY").toDouble());
        s.target = QPointF(obj.value("targetX").toDouble(), obj.value("targetY").toDouble());
        s.depthMm = obj.value("depthMm").toDouble();
        s.alignmentErrorPx = obj.value("alignmentErrorPx").toDouble();
        m_samples.push_back(s);
    }

    m_playbackIndex = 0;
    return true;
}

bool PlaybackRecorder::exportAnnotatedFrames(const QString& outputFolder, QString* errorMessage) const
{
    if (m_samples.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("No recorded samples to export");
        }
        return false;
    }

    QDir dir(outputFolder);
    if (!dir.exists() && !dir.mkpath(".")) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Cannot create output folder: %1").arg(outputFolder);
        }
        return false;
    }

    for (int i = 0; i < m_samples.size(); ++i) {
        const TrackerSample& s = m_samples.at(i);
        QImage frame(960, 540, QImage::Format_ARGB32_Premultiplied);
        frame.fill(QColor(20, 24, 30));

        QPainter painter(&frame);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QPen(QColor(250, 70, 70), 3));
        painter.setBrush(QColor(250, 70, 70, 90));
        painter.drawEllipse(s.toolTip, 8, 8);

        painter.setPen(QPen(QColor(70, 220, 130), 2));
        painter.setBrush(QColor(70, 220, 130, 90));
        painter.drawEllipse(s.target, 7, 7);
        painter.drawLine(s.toolTip, s.target);

        painter.setPen(Qt::white);
        painter.drawText(20, 30, QStringLiteral("Depth: %1 mm").arg(s.depthMm, 0, 'f', 2));
        painter.drawText(20, 55, QStringLiteral("Alignment Error: %1 px").arg(s.alignmentErrorPx, 0, 'f', 2));
        painter.drawText(20, 80, QStringLiteral("Timestamp: %1").arg(s.timestampMs));

        const QString filePath = dir.filePath(QStringLiteral("frame_%1.png").arg(i, 4, 10, QLatin1Char('0')));
        if (!frame.save(filePath)) {
            if (errorMessage != nullptr) {
                *errorMessage = QStringLiteral("Failed to save frame: %1").arg(filePath);
            }
            return false;
        }
    }

    return true;
}

bool PlaybackRecorder::exportAnnotatedVideo(
    const QString& outputFolder,
    const QString& outputVideoPath,
    QString* errorMessage) const
{
    QString frameError;
    if (!exportAnnotatedFrames(outputFolder, &frameError)) {
        if (errorMessage != nullptr) {
            *errorMessage = frameError;
        }
        return false;
    }

    QProcess ffmpeg;
    QStringList args;
    args << "-y"
         << "-framerate"
         << "25"
         << "-i"
         << QDir(outputFolder).filePath("frame_%04d.png")
         << "-pix_fmt"
         << "yuv420p"
         << outputVideoPath;

    ffmpeg.start("ffmpeg", args);
    if (!ffmpeg.waitForStarted(1000)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral(
                "Frames exported, but ffmpeg not found in PATH. Install ffmpeg to generate MP4.");
        }
        return false;
    }

    ffmpeg.waitForFinished(60 * 1000);
    if (ffmpeg.exitStatus() != QProcess::NormalExit || ffmpeg.exitCode() != 0) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("ffmpeg failed: %1")
                                .arg(QString::fromUtf8(ffmpeg.readAllStandardError()));
        }
        return false;
    }

    return true;
}

void PlaybackRecorder::play()
{
    if (m_samples.isEmpty() || m_playingBack) {
        return;
    }
    m_playbackIndex = 0;
    m_playingBack = true;
    m_playbackTimer.start();
    emit playingBackChanged();
}

void PlaybackRecorder::pause()
{
    if (!m_playingBack) {
        return;
    }
    m_playbackTimer.stop();
    m_playingBack = false;
    emit playingBackChanged();
}

void PlaybackRecorder::onPlaybackTick()
{
    if (m_playbackIndex >= m_samples.size()) {
        pause();
        return;
    }

    emit playbackSampleReady(m_samples.at(m_playbackIndex));
    ++m_playbackIndex;
}

} // namespace surgiview
