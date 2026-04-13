#include <QtTest>

#include "tracking/TelemetryJsonCodec.h"

class TelemetryJsonCodecTests : public QObject {
    Q_OBJECT

private slots:
    void parsesDepthMmPayload();
    void parsesLegacyDepthField();
    void rejectsInvalidPayload();
};

void TelemetryJsonCodecTests::parsesDepthMmPayload()
{
    double x = 0.0;
    double y = 0.0;
    double depthMm = 0.0;
    QString error;

    QVERIFY(surgiview::parseTelemetryJson("{\"x\":1.5,\"y\":2.5,\"depthMm\":3.5}", &x, &y, &depthMm, &error));
    QCOMPARE(x, 1.5);
    QCOMPARE(y, 2.5);
    QCOMPARE(depthMm, 3.5);
}

void TelemetryJsonCodecTests::parsesLegacyDepthField()
{
    double x = 0.0;
    double y = 0.0;
    double depthMm = 0.0;
    QString error;

    QVERIFY(surgiview::parseTelemetryJson("{\"x\":10,\"y\":20,\"depth\":30}", &x, &y, &depthMm, &error));
    QCOMPARE(x, 10.0);
    QCOMPARE(y, 20.0);
    QCOMPARE(depthMm, 30.0);
}

void TelemetryJsonCodecTests::rejectsInvalidPayload()
{
    double x = 0.0;
    double y = 0.0;
    double depthMm = 0.0;
    QString error;

    QVERIFY(!surgiview::parseTelemetryJson("not-json", &x, &y, &depthMm, &error));
    QVERIFY(!error.isEmpty());
}

QTEST_APPLESS_MAIN(TelemetryJsonCodecTests)
#include "test_telemetryjsoncodec.moc"
