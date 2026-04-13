#pragma once

#include <QObject>
#include <QTimer>
#include <QVector>

#include "models/Types.h"

namespace surgiview {

class PlaybackRecorder : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)
    Q_PROPERTY(bool playingBack READ playingBack NOTIFY playingBackChanged)

public:
    explicit PlaybackRecorder(QObject* parent = nullptr);

    bool recording() const;
    bool playingBack() const;

    void appendSample(const TrackerSample& sample);

    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
    Q_INVOKABLE void clear();

    Q_INVOKABLE bool saveSession(const QString& outputJsonPath, QString* errorMessage = nullptr) const;
    Q_INVOKABLE bool loadSession(const QString& inputJsonPath, QString* errorMessage = nullptr);

    Q_INVOKABLE bool exportAnnotatedFrames(const QString& outputFolder, QString* errorMessage = nullptr) const;
    Q_INVOKABLE bool exportAnnotatedVideo(
        const QString& outputFolder,
        const QString& outputVideoPath,
        QString* errorMessage = nullptr) const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();

signals:
    void recordingChanged();
    void playingBackChanged();
    void playbackSampleReady(const surgiview::TrackerSample& sample);

private slots:
    void onPlaybackTick();

private:
    bool m_recording = false;
    bool m_playingBack = false;
    QVector<TrackerSample> m_samples;
    int m_playbackIndex = 0;
    QTimer m_playbackTimer;
};

} // namespace surgiview
