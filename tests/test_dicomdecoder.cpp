#include <QtTest>

#include <QFile>
#include <QTemporaryDir>

#include "imaging/DicomDecoder.h"

namespace {

void appendTag(QByteArray* out, quint16 group, quint16 element, const char vr[2], const QByteArray& value)
{
    out->append(static_cast<char>(group & 0xff));
    out->append(static_cast<char>((group >> 8) & 0xff));
    out->append(static_cast<char>(element & 0xff));
    out->append(static_cast<char>((element >> 8) & 0xff));
    out->append(vr, 2);

    const QByteArray vrString(vr, 2);
    const bool longVr = vrString == "OB" || vrString == "OW" || vrString == "SQ" ||
                        vrString == "UN" || vrString == "UT" || vrString == "OF" ||
                        vrString == "OD" || vrString == "OL" || vrString == "OV" ||
                        vrString == "UC" || vrString == "UR" || vrString == "UV";

    QByteArray normalized = value;
    if (normalized.size() & 1) {
        normalized.append(' ');
    }

    if (longVr) {
        out->append('\0');
        out->append('\0');
        const quint32 size = static_cast<quint32>(normalized.size());
        out->append(static_cast<char>(size & 0xff));
        out->append(static_cast<char>((size >> 8) & 0xff));
        out->append(static_cast<char>((size >> 16) & 0xff));
        out->append(static_cast<char>((size >> 24) & 0xff));
    } else {
        const quint16 size = static_cast<quint16>(normalized.size());
        out->append(static_cast<char>(size & 0xff));
        out->append(static_cast<char>((size >> 8) & 0xff));
    }

    out->append(normalized);
}

QByteArray uint16Value(quint16 value)
{
    QByteArray bytes;
    bytes.append(static_cast<char>(value & 0xff));
    bytes.append(static_cast<char>((value >> 8) & 0xff));
    return bytes;
}

QString writeDicomFile(const QByteArray& content)
{
    static int counter = 0;
    const QString path = QDir(QDir::tempPath()).filePath(QStringLiteral("surgiview_dicom_test_%1.dcm").arg(++counter));
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.close();
    return path;
}

QByteArray buildBaselineDicom8Bit()
{
    QByteArray dicom(128, '\0');
    dicom.append("DICM", 4);

    appendTag(&dicom, 0x0002, 0x0010, "UI", QByteArray("1.2.840.10008.1.2.1"));
    appendTag(&dicom, 0x0028, 0x0002, "US", uint16Value(1));
    appendTag(&dicom, 0x0028, 0x0004, "CS", QByteArray("MONOCHROME2"));
    appendTag(&dicom, 0x0028, 0x0010, "US", uint16Value(2));
    appendTag(&dicom, 0x0028, 0x0011, "US", uint16Value(2));
    appendTag(&dicom, 0x0028, 0x0030, "DS", QByteArray("0.70\\0.70"));
    appendTag(&dicom, 0x0028, 0x0100, "US", uint16Value(8));
    appendTag(&dicom, 0x0028, 0x0101, "US", uint16Value(8));
    appendTag(&dicom, 0x0028, 0x0103, "US", uint16Value(0));

    QByteArray pixels;
    pixels.append('\0');
    pixels.append(static_cast<char>(85));
    pixels.append(static_cast<char>(170));
    pixels.append(static_cast<char>(255));
    appendTag(&dicom, 0x7fe0, 0x0010, "OB", pixels);

    return dicom;
}

QByteArray buildWindowedDicom16Bit()
{
    QByteArray dicom(128, '\0');
    dicom.append("DICM", 4);

    appendTag(&dicom, 0x0002, 0x0010, "UI", QByteArray("1.2.840.10008.1.2.1"));
    appendTag(&dicom, 0x0028, 0x0002, "US", uint16Value(1));
    appendTag(&dicom, 0x0028, 0x0004, "CS", QByteArray("MONOCHROME1"));
    appendTag(&dicom, 0x0028, 0x0010, "US", uint16Value(1));
    appendTag(&dicom, 0x0028, 0x0011, "US", uint16Value(2));
    appendTag(&dicom, 0x0028, 0x0030, "DS", QByteArray("0.50\\0.50"));
    appendTag(&dicom, 0x0028, 0x0100, "US", uint16Value(16));
    appendTag(&dicom, 0x0028, 0x0101, "US", uint16Value(16));
    appendTag(&dicom, 0x0028, 0x0103, "US", uint16Value(0));
    appendTag(&dicom, 0x0028, 0x1050, "DS", QByteArray("50"));
    appendTag(&dicom, 0x0028, 0x1051, "DS", QByteArray("100"));

    QByteArray pixels;
    pixels.append(uint16Value(0));
    pixels.append(uint16Value(100));
    appendTag(&dicom, 0x7fe0, 0x0010, "OW", pixels);

    return dicom;
}

} // namespace

class DicomDecoderTests : public QObject {
    Q_OBJECT

private slots:
    void decodesExplicitVrLittleEndian8Bit();
    void decodesWindowed16BitMonochrome1();
};

void DicomDecoderTests::decodesExplicitVrLittleEndian8Bit()
{
    const QString path = writeDicomFile(buildBaselineDicom8Bit());

    QImage image;
    double spacing = 0.0;
    QString error;
    QVERIFY2(surgiview::decodeDicomFrame(path, &image, &spacing, &error), qPrintable(error));

    QCOMPARE(image.width(), 2);
    QCOMPARE(image.height(), 2);
    QCOMPARE(spacing, 0.70);
    QCOMPARE(qGray(image.pixel(0, 0)), 0);
    QVERIFY(qGray(image.pixel(1, 0)) > qGray(image.pixel(0, 0)));
    QCOMPARE(qGray(image.pixel(1, 1)), 255);

    QFile::remove(path);
}

void DicomDecoderTests::decodesWindowed16BitMonochrome1()
{
    const QString path = writeDicomFile(buildWindowedDicom16Bit());

    QImage image;
    double spacing = 0.0;
    QString error;
    QVERIFY2(surgiview::decodeDicomFrame(path, &image, &spacing, &error), qPrintable(error));

    QCOMPARE(image.width(), 2);
    QCOMPARE(image.height(), 1);
    QCOMPARE(spacing, 0.50);
    QVERIFY(qGray(image.pixel(0, 0)) > qGray(image.pixel(1, 0)));

    QFile::remove(path);
}

QTEST_MAIN(DicomDecoderTests)
#include "test_dicomdecoder.moc"