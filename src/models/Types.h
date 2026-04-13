#pragma once

#include <QImage>
#include <QMetaType>
#include <QPointF>
#include <QString>

namespace surgiview {

struct SliceFrame {
    QString sourceName;
    QImage image;
    double pixelSpacingMm = 0.2;
};

struct TrackerSample {
    qint64 timestampMs = 0;
    QPointF toolTip;
    QPointF target;
    double depthMm = 0.0;
    double alignmentErrorPx = 0.0;
};

} // namespace surgiview

Q_DECLARE_METATYPE(surgiview::TrackerSample)
