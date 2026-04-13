#pragma once

#include <QByteArray>
#include <QString>

namespace surgiview {

bool parseTelemetryJson(
    const QByteArray& payload,
    double* x,
    double* y,
    double* depthMm,
    QString* errorMessage = nullptr);

} // namespace surgiview
