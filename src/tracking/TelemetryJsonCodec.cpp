#include "tracking/TelemetryJsonCodec.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace surgiview {

bool parseTelemetryJson(
    const QByteArray& payload,
    double* x,
    double* y,
    double* depthMm,
    QString* errorMessage)
{
    if (x == nullptr || y == nullptr || depthMm == nullptr) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Telemetry output pointers are null");
        }
        return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Invalid telemetry payload received");
        }
        return false;
    }

    const QJsonObject obj = doc.object();
    const QJsonValue xVal = obj.value(QStringLiteral("x"));
    const QJsonValue yVal = obj.value(QStringLiteral("y"));
    const QJsonValue depthVal = obj.contains(QStringLiteral("depthMm"))
        ? obj.value(QStringLiteral("depthMm"))
        : obj.value(QStringLiteral("depth"));

    if (!xVal.isDouble() || !yVal.isDouble() || !depthVal.isDouble()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Telemetry payload missing numeric x/y/depth");
        }
        return false;
    }

    *x = xVal.toDouble();
    *y = yVal.toDouble();
    *depthMm = depthVal.toDouble();
    return true;
}

} // namespace surgiview
