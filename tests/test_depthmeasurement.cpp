#include <QtTest>

#include "measurement/DepthMeasurement.h"

class DepthMeasurementTests : public QObject {
    Q_OBJECT

private slots:
    void computesPixelDistance();
    void computesMmDistance();
};

void DepthMeasurementTests::computesPixelDistance()
{
    const QPointF a(0.0, 0.0);
    const QPointF b(3.0, 4.0);
    QCOMPARE(surgiview::DepthMeasurement::distancePx(a, b), 5.0);
}

void DepthMeasurementTests::computesMmDistance()
{
    const QPointF a(10.0, 10.0);
    const QPointF b(13.0, 14.0);
    QCOMPARE(surgiview::DepthMeasurement::distanceMm(a, b, 0.5), 2.5);
}

QTEST_APPLESS_MAIN(DepthMeasurementTests)
#include "test_depthmeasurement.moc"
