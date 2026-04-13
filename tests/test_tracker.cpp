#include <QtTest>

#include "tracking/InstrumentTracker.h"

class TrackerTests : public QObject {
    Q_OBJECT

private slots:
    void emitsSamplesWhenRunning();
    void emitsExternalSamplesAfterTelemetryIngest();
};

void TrackerTests::emitsSamplesWhenRunning()
{
    qRegisterMetaType<surgiview::TrackerSample>("surgiview::TrackerSample");
    surgiview::InstrumentTracker tracker;

    QSignalSpy spy(&tracker, SIGNAL(sampleReady(surgiview::TrackerSample)));
    tracker.start();

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 300);

    tracker.stop();
    QVERIFY(spy.count() > 0);
}

void TrackerTests::emitsExternalSamplesAfterTelemetryIngest()
{
    qRegisterMetaType<surgiview::TrackerSample>("surgiview::TrackerSample");
    surgiview::InstrumentTracker tracker;
    tracker.setSourceMode("external");

    QSignalSpy spy(&tracker, SIGNAL(sampleReady(surgiview::TrackerSample)));
    tracker.start();

    QTest::qWait(120);
    QCOMPARE(spy.count(), 0);

    tracker.ingestExternalTelemetry(210.0, 160.0, 12.5);
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 300);

    tracker.stop();
}

QTEST_MAIN(TrackerTests)
#include "test_tracker.moc"
