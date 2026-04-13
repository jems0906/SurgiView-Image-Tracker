#include <QtTest>

#include "tracking/InstrumentTracker.h"

class TrackerTests : public QObject {
    Q_OBJECT

private slots:
    void emitsSamplesWhenRunning();
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

QTEST_MAIN(TrackerTests)
#include "test_tracker.moc"
