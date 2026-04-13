#pragma once

#include <QObject>
#include <QVector>

#include "models/Types.h"

namespace surgiview {

class DicomSeriesLoader : public QObject {
    Q_OBJECT

public:
    explicit DicomSeriesLoader(QObject* parent = nullptr);

    bool loadFolder(const QString& folderPath, QString* errorMessage = nullptr);
    bool hasFrames() const;
    int frameCount() const;
    int currentIndex() const;

    SliceFrame currentFrame() const;
    bool next();
    bool previous();

private:
    SliceFrame readFrameFile(const QString& filePath) const;

    QVector<SliceFrame> m_frames;
    int m_index = 0;
};

} // namespace surgiview
