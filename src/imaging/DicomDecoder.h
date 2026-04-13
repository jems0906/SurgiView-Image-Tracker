#pragma once

#include <QImage>
#include <QString>

namespace surgiview {

// Attempts to decode a DICOM pixel frame.
// Returns true when decoding succeeds and outputs image + spacing.
bool decodeDicomFrame(
    const QString& filePath,
    QImage* outImage,
    double* outPixelSpacingMm,
    QString* errorMessage = nullptr);

} // namespace surgiview
