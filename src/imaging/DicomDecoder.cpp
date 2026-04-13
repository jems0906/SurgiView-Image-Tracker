#include "imaging/DicomDecoder.h"

#include <limits>

#include <QtGlobal>

#if defined(SURGIVIEW_HAS_DCMTK) && SURGIVIEW_HAS_DCMTK
    #include <dcmtk/dcmdata/dcdeftag.h>
    #include <dcmtk/dcmdata/dctk.h>
#endif

namespace surgiview {

namespace {

double parseSpacingValue(const QString& raw, double fallback)
{
    const int sep = raw.indexOf(QLatin1Char('\\'));
    const QString first = (sep >= 0) ? raw.left(sep) : raw;
    bool ok = false;
    const double value = first.toDouble(&ok);
    if (!ok || value <= 0.0) {
        return fallback;
    }
    return value;
}

void normalizeToGrayscale8(const quint16* src, int pixelCount, QImage* outImage)
{
    quint16 minVal = std::numeric_limits<quint16>::max();
    quint16 maxVal = 0;
    for (int i = 0; i < pixelCount; ++i) {
        minVal = qMin(minVal, src[i]);
        maxVal = qMax(maxVal, src[i]);
    }

    const int width = outImage->width();
    const int height = outImage->height();
    const int dynamicRange = qMax(1, static_cast<int>(maxVal) - static_cast<int>(minVal));

    for (int y = 0; y < height; ++y) {
        uchar* row = outImage->scanLine(y);
        const int base = y * width;
        for (int x = 0; x < width; ++x) {
            const int idx = base + x;
            const int shifted = static_cast<int>(src[idx]) - static_cast<int>(minVal);
            row[x] = static_cast<uchar>((shifted * 255) / dynamicRange);
        }
    }
}

} // namespace

bool decodeDicomFrame(
    const QString& filePath,
    QImage* outImage,
    double* outPixelSpacingMm,
    QString* errorMessage)
{
    if (outImage == nullptr || outPixelSpacingMm == nullptr) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Decoder output pointers are null");
        }
        return false;
    }

#if defined(SURGIVIEW_HAS_DCMTK) && SURGIVIEW_HAS_DCMTK
    DcmFileFormat fileFormat;
    const OFCondition loadStatus = fileFormat.loadFile(filePath.toStdString().c_str());
    if (!loadStatus.good()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("DCMTK failed to load DICOM file");
        }
        return false;
    }

    DcmDataset* dataset = fileFormat.getDataset();
    if (dataset == nullptr) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("DICOM dataset is null");
        }
        return false;
    }

    Uint16 rows = 0;
    Uint16 cols = 0;
    if (!dataset->findAndGetUint16(DCM_Rows, rows).good() ||
        !dataset->findAndGetUint16(DCM_Columns, cols).good() ||
        rows == 0 || cols == 0) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Missing or invalid DICOM dimensions");
        }
        return false;
    }

    *outPixelSpacingMm = 0.25;
    OFString spacingRaw;
    if (dataset->findAndGetOFStringArray(DCM_PixelSpacing, spacingRaw).good()) {
        *outPixelSpacingMm = parseSpacingValue(QString::fromLatin1(spacingRaw.c_str()), *outPixelSpacingMm);
    }

    const int pixelCount = static_cast<int>(rows) * static_cast<int>(cols);

    const Uint16* pixel16 = nullptr;
    unsigned long count16 = 0;
    if (dataset->findAndGetUint16Array(DCM_PixelData, pixel16, &count16).good() &&
        pixel16 != nullptr &&
        count16 >= static_cast<unsigned long>(pixelCount)) {
        *outImage = QImage(cols, rows, QImage::Format_Grayscale8);
        normalizeToGrayscale8(pixel16, pixelCount, outImage);
        return true;
    }

    const Uint8* pixel8 = nullptr;
    unsigned long count8 = 0;
    if (dataset->findAndGetUint8Array(DCM_PixelData, pixel8, &count8).good() &&
        pixel8 != nullptr &&
        count8 >= static_cast<unsigned long>(pixelCount)) {
        *outImage = QImage(cols, rows, QImage::Format_Grayscale8);
        for (int y = 0; y < rows; ++y) {
            uchar* row = outImage->scanLine(y);
            const int base = y * cols;
            for (int x = 0; x < cols; ++x) {
                row[x] = static_cast<uchar>(pixel8[base + x]);
            }
        }
        return true;
    }

    if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Unsupported DICOM pixel format");
    }
    return false;
#else
    Q_UNUSED(filePath);
    if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("DCMTK support disabled at build time");
    }
    return false;
#endif
}

} // namespace surgiview
