#pragma once

#include <QPointF>

namespace surgiview {

class DepthMeasurement {
public:
    static double distancePx(const QPointF& a, const QPointF& b);
    static double distanceMm(const QPointF& a, const QPointF& b, double pixelSpacingMm);
};

} // namespace surgiview
