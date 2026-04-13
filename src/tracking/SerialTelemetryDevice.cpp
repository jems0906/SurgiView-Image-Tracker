#include "tracking/SerialTelemetryDevice.h"

#include <QSerialPort>

#include "tracking/TelemetryJsonCodec.h"

namespace surgiview {

SerialTelemetryDevice::SerialTelemetryDevice(QObject* parent)
    : QObject(parent)
    , m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::readyRead, this, &SerialTelemetryDevice::onReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        Q_UNUSED(error);
        onErrorOccurred();
    });
}

SerialTelemetryDevice::~SerialTelemetryDevice() = default;

bool SerialTelemetryDevice::start(const QString& portName, qint32 baudRate, QString* errorMessage)
{
    if (m_serial->isOpen()) {
        stop();
    }

    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);

    if (!m_serial->open(QIODevice::ReadOnly)) {
        if (errorMessage != nullptr) {
            *errorMessage = m_serial->errorString();
        }
        return false;
    }

    m_buffer.clear();
    emit openChanged();
    emit deviceMessage(QStringLiteral("Serial telemetry opened on %1 @ %2 baud").arg(portName).arg(baudRate));
    return true;
}

void SerialTelemetryDevice::stop()
{
    m_buffer.clear();
    if (m_serial->isOpen()) {
        const QString name = m_serial->portName();
        m_serial->close();
        emit openChanged();
        emit deviceMessage(QStringLiteral("Serial telemetry stopped on %1").arg(name));
    }
}

bool SerialTelemetryDevice::isOpen() const
{
    return m_serial->isOpen();
}

QString SerialTelemetryDevice::portName() const
{
    return m_serial->portName();
}

qint32 SerialTelemetryDevice::baudRate() const
{
    return m_serial->baudRate();
}

void SerialTelemetryDevice::onReadyRead()
{
    m_buffer.append(m_serial->readAll());

    int newlineIndex = m_buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = m_buffer.left(newlineIndex).trimmed();
        m_buffer.remove(0, newlineIndex + 1);
        if (!line.isEmpty()) {
            parseLine(line);
        }
        newlineIndex = m_buffer.indexOf('\n');
    }
}

void SerialTelemetryDevice::onErrorOccurred()
{
    if (m_serial->error() == QSerialPort::NoError) {
        return;
    }

    emit deviceMessage(QStringLiteral("Serial telemetry error: %1").arg(m_serial->errorString()));
}

void SerialTelemetryDevice::parseLine(const QByteArray& line)
{
    double x = 0.0;
    double y = 0.0;
    double depthMm = 0.0;
    QString error;
    if (!parseTelemetryJson(line, &x, &y, &depthMm, &error)) {
        emit deviceMessage(error);
        return;
    }

    emit telemetryReceived(x, y, depthMm);
}

} // namespace surgiview
