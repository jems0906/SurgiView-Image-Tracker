#include "imaging/DicomDecoder.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include <QByteArray>
#include <QFile>
#include <QtEndian>
#include <QtGlobal>

#if defined(SURGIVIEW_HAS_DCMTK) && SURGIVIEW_HAS_DCMTK
    #include <dcmtk/dcmdata/dcdeftag.h>
    #include <dcmtk/dcmdata/dctk.h>
#endif

namespace surgiview {

namespace {

struct BaselineDicomFrame {
    int rows = 0;
    int columns = 0;
    int samplesPerPixel = 1;
    int bitsAllocated = 0;
    int bitsStored = 0;
    int pixelRepresentation = 0;
    double pixelSpacingMm = 0.25;
    double rescaleSlope = 1.0;
    double rescaleIntercept = 0.0;
    double windowCenter = std::numeric_limits<double>::quiet_NaN();
    double windowWidth = std::numeric_limits<double>::quiet_NaN();
    QString photometricInterpretation = QStringLiteral("MONOCHROME2");
    QByteArray pixelData;
};

constexpr quint32 kUndefinedLength = 0xffffffffu;

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

quint16 readLe16(const char* data)
{
    return qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data));
}

quint32 readLe32(const char* data)
{
    return qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data));
}

QString trimmedLatin1(const QByteArray& raw)
{
    QByteArray compact = raw;
    compact.replace('\0', ' ');
    return QString::fromLatin1(compact).trimmed();
}

double parseFirstNumber(const QByteArray& raw, double fallback)
{
    const QString text = trimmedLatin1(raw);
    if (text.isEmpty()) {
        return fallback;
    }

    const int sep = text.indexOf(QLatin1Char('\\'));
    const QString first = (sep >= 0) ? text.left(sep) : text;
    bool ok = false;
    const double value = first.toDouble(&ok);
    if (!ok) {
        return fallback;
    }
    return value;
}

bool isLongValueRepresentation(const QByteArray& vr)
{
    return vr == "OB" || vr == "OD" || vr == "OF" || vr == "OL" || vr == "OV" ||
           vr == "OW" || vr == "SQ" || vr == "UC" || vr == "UN" || vr == "UR" ||
           vr == "UT" || vr == "UV";
}

bool parseBaselineDicom(const QString& filePath, BaselineDicomFrame* frame, QString* errorMessage)
{
    if (frame == nullptr) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Decoder output frame is null");
        }
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Failed to open DICOM file");
        }
        return false;
    }

    const QByteArray bytes = file.readAll();
    if (bytes.size() < 132) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("DICOM file is too small");
        }
        return false;
    }

    if (bytes.mid(128, 4) != "DICM") {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Missing DICM preamble");
        }
        return false;
    }

    qsizetype offset = 132;
    QString transferSyntax = QStringLiteral("1.2.840.10008.1.2.1");

    while (offset + 8 <= bytes.size()) {
        const char* ptr = bytes.constData() + offset;
        const quint16 group = readLe16(ptr);
        const quint16 element = readLe16(ptr + 2);
        const QByteArray vr = bytes.mid(offset + 4, 2);

        quint32 valueLength = 0;
        qsizetype valueOffset = offset + 8;
        if (isLongValueRepresentation(vr)) {
            if (offset + 12 > bytes.size()) {
                break;
            }
            valueLength = readLe32(ptr + 8);
            valueOffset = offset + 12;
        } else {
            valueLength = readLe16(ptr + 6);
        }

        if (valueLength == kUndefinedLength || valueOffset + valueLength > bytes.size()) {
            if (errorMessage != nullptr) {
                *errorMessage = QStringLiteral("Unsupported DICOM structure or truncated value");
            }
            return false;
        }

        const QByteArray value = bytes.mid(valueOffset, valueLength);

        if (group == 0x0002 && element == 0x0010) {
            transferSyntax = trimmedLatin1(value);
        } else if (group == 0x0028 && element == 0x0010) {
            frame->rows = static_cast<int>(readLe16(value.constData()));
        } else if (group == 0x0028 && element == 0x0011) {
            frame->columns = static_cast<int>(readLe16(value.constData()));
        } else if (group == 0x0028 && element == 0x0002) {
            frame->samplesPerPixel = static_cast<int>(readLe16(value.constData()));
        } else if (group == 0x0028 && element == 0x0100) {
            frame->bitsAllocated = static_cast<int>(readLe16(value.constData()));
        } else if (group == 0x0028 && element == 0x0101) {
            frame->bitsStored = static_cast<int>(readLe16(value.constData()));
        } else if (group == 0x0028 && element == 0x0103) {
            frame->pixelRepresentation = static_cast<int>(readLe16(value.constData()));
        } else if (group == 0x0028 && element == 0x0030) {
            frame->pixelSpacingMm = parseSpacingValue(trimmedLatin1(value), frame->pixelSpacingMm);
        } else if (group == 0x0028 && element == 0x0004) {
            frame->photometricInterpretation = trimmedLatin1(value).toUpper();
        } else if (group == 0x0028 && element == 0x1050) {
            frame->windowCenter = parseFirstNumber(value, frame->windowCenter);
        } else if (group == 0x0028 && element == 0x1051) {
            frame->windowWidth = parseFirstNumber(value, frame->windowWidth);
        } else if (group == 0x0028 && element == 0x1052) {
            frame->rescaleIntercept = parseFirstNumber(value, frame->rescaleIntercept);
        } else if (group == 0x0028 && element == 0x1053) {
            frame->rescaleSlope = parseFirstNumber(value, frame->rescaleSlope);
        } else if (group == 0x7fe0 && element == 0x0010) {
            frame->pixelData = value;
            break;
        }

        offset = valueOffset + valueLength;
        if (offset & 1) {
            ++offset;
        }
    }

    if (transferSyntax != QStringLiteral("1.2.840.10008.1.2.1")) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Only Explicit VR Little Endian DICOM is supported without DCMTK");
        }
        return false;
    }

    if (frame->rows <= 0 || frame->columns <= 0 || frame->pixelData.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Missing required DICOM image attributes");
        }
        return false;
    }

    if (frame->samplesPerPixel != 1) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Only monochrome DICOM images are supported without DCMTK");
        }
        return false;
    }

    if (frame->bitsAllocated != 8 && frame->bitsAllocated != 16) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Only 8-bit and 16-bit grayscale DICOM images are supported without DCMTK");
        }
        return false;
    }

    return true;
}

void renderToGrayscale8(const BaselineDicomFrame& frame, QImage* outImage)
{
    const int pixelCount = frame.rows * frame.columns;
    std::vector<double> scaledPixels(pixelCount);

    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    if (frame.bitsAllocated == 8) {
        const uchar* src = reinterpret_cast<const uchar*>(frame.pixelData.constData());
        for (int i = 0; i < pixelCount; ++i) {
            const double value = frame.rescaleIntercept + frame.rescaleSlope * static_cast<double>(src[i]);
            scaledPixels[static_cast<size_t>(i)] = value;
            minValue = qMin(minValue, value);
            maxValue = qMax(maxValue, value);
        }
    } else {
        const quint16* src = reinterpret_cast<const quint16*>(frame.pixelData.constData());
        for (int i = 0; i < pixelCount; ++i) {
            double rawValue = 0.0;
            if (frame.pixelRepresentation == 1) {
                rawValue = static_cast<double>(static_cast<qint16>(qFromLittleEndian<quint16>(src[i])));
            } else {
                rawValue = static_cast<double>(qFromLittleEndian<quint16>(src[i]));
            }
            const double value = frame.rescaleIntercept + frame.rescaleSlope * rawValue;
            scaledPixels[static_cast<size_t>(i)] = value;
            minValue = qMin(minValue, value);
            maxValue = qMax(maxValue, value);
        }
    }

    double low = minValue;
    double high = maxValue;
    if (!std::isnan(frame.windowCenter) && !std::isnan(frame.windowWidth) && frame.windowWidth > 1.0) {
        low = frame.windowCenter - (frame.windowWidth / 2.0);
        high = frame.windowCenter + (frame.windowWidth / 2.0);
    }

    const double range = qMax(1.0, high - low);
    const bool invert = frame.photometricInterpretation == QStringLiteral("MONOCHROME1");

    *outImage = QImage(frame.columns, frame.rows, QImage::Format_Grayscale8);
    for (int y = 0; y < frame.rows; ++y) {
        uchar* row = outImage->scanLine(y);
        const int base = y * frame.columns;
        for (int x = 0; x < frame.columns; ++x) {
            const double value = scaledPixels[static_cast<size_t>(base + x)];
            const double clamped = std::clamp((value - low) / range, 0.0, 1.0);
            int pixel = static_cast<int>(clamped * 255.0);
            if (invert) {
                pixel = 255 - pixel;
            }
            row[x] = static_cast<uchar>(pixel);
        }
    }
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

    BaselineDicomFrame baselineFrame;

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
    if (parseBaselineDicom(filePath, &baselineFrame, errorMessage)) {
        renderToGrayscale8(baselineFrame, outImage);
        *outPixelSpacingMm = baselineFrame.pixelSpacingMm;
        return true;
    }
    return false;
#else
    if (parseBaselineDicom(filePath, &baselineFrame, errorMessage)) {
        renderToGrayscale8(baselineFrame, outImage);
        *outPixelSpacingMm = baselineFrame.pixelSpacingMm;
        return true;
    }
    return false;
#endif
}

} // namespace surgiview
