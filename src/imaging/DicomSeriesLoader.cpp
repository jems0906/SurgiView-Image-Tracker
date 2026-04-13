#include "imaging/DicomSeriesLoader.h"

#include "imaging/DicomDecoder.h"

#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QStringList>

namespace surgiview {

DicomSeriesLoader::DicomSeriesLoader(QObject* parent)
    : QObject(parent)
{
}

bool DicomSeriesLoader::loadFolder(const QString& folderPath, QString* errorMessage)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Folder does not exist: %1").arg(folderPath);
        }
        return false;
    }

    const QStringList patterns = {"*.dcm", "*.png", "*.jpg", "*.jpeg", "*.bmp"};
    const QFileInfoList files = dir.entryInfoList(patterns, QDir::Files, QDir::Name);
    if (files.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("No slice files found (.dcm/.png/.jpg/.bmp)");
        }
        return false;
    }

    m_frames.clear();
    for (const QFileInfo& info : files) {
        m_frames.push_back(readFrameFile(info.absoluteFilePath()));
    }

    m_index = 0;
    return true;
}

bool DicomSeriesLoader::hasFrames() const
{
    return !m_frames.isEmpty();
}

int DicomSeriesLoader::frameCount() const
{
    return m_frames.size();
}

int DicomSeriesLoader::currentIndex() const
{
    return m_index;
}

SliceFrame DicomSeriesLoader::currentFrame() const
{
    if (m_frames.isEmpty()) {
        return {};
    }
    return m_frames.at(m_index);
}

bool DicomSeriesLoader::next()
{
    if (m_frames.isEmpty() || m_index >= m_frames.size() - 1) {
        return false;
    }
    ++m_index;
    return true;
}

bool DicomSeriesLoader::previous()
{
    if (m_frames.isEmpty() || m_index <= 0) {
        return false;
    }
    --m_index;
    return true;
}

SliceFrame DicomSeriesLoader::readFrameFile(const QString& filePath) const
{
    SliceFrame frame;
    frame.sourceName = QFileInfo(filePath).fileName();

    if (filePath.endsWith(QStringLiteral(".dcm"), Qt::CaseInsensitive)) {
        QString decodeError;
        QImage decoded;
        double spacingMm = 0.25;

        if (decodeDicomFrame(filePath, &decoded, &spacingMm, &decodeError)) {
            frame.image = decoded;
            frame.pixelSpacingMm = spacingMm;
            return frame;
        }

        // Keep simulation fallback if DCMTK is unavailable or decoding fails.
        QImage simulated(640, 480, QImage::Format_Grayscale8);
        for (int y = 0; y < simulated.height(); ++y) {
            uchar* row = simulated.scanLine(y);
            for (int x = 0; x < simulated.width(); ++x) {
                row[x] = static_cast<uchar>((x + y) % 255);
            }
        }
        frame.image = simulated;
        frame.pixelSpacingMm = 0.25;
        return frame;
    }

    QImageReader reader(filePath);
    QImage image = reader.read();
    if (image.isNull()) {
        image = QImage(640, 480, QImage::Format_Grayscale8);
        image.fill(Qt::black);
    }

    frame.image = image;
    frame.pixelSpacingMm = 0.2;
    return frame;
}

} // namespace surgiview
