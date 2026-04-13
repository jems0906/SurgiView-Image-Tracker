#include "measurement/DepthMeasurement.h"

#include <QtMath>

namespace surgiview {

double DepthMeasurement::distancePx(const QPointF& a, const QPointF& b)
{
    const double dx = a.x() - b.x();
    const double dy = a.y() - b.y();
    return qSqrt((dx * dx) + (dy * dy));
}

double DepthMeasurement::distanceMm(const QPointF& a, const QPointF& b, double pixelSpacingMm)
{
    return distancePx(a, b) * pixelSpacingMm;
}

} // namespace surgiview
