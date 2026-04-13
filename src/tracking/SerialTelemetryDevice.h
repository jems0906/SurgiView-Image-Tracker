#pragma once

#include <QObject>
#include <QByteArray>

class QSerialPort;

namespace surgiview {

class SerialTelemetryDevice : public QObject {
    Q_OBJECT

public:
    explicit SerialTelemetryDevice(QObject* parent = nullptr);
    ~SerialTelemetryDevice() override;

    bool start(const QString& portName, qint32 baudRate, QString* errorMessage = nullptr);
    void stop();

    bool isOpen() const;
    QString portName() const;
    qint32 baudRate() const;

signals:
    void openChanged();
    void telemetryReceived(double x, double y, double depthMm);
    void deviceMessage(const QString& message);

private slots:
    void onReadyRead();
    void onErrorOccurred();

private:
    void parseLine(const QByteArray& line);

    QSerialPort* m_serial = nullptr;
    QByteArray m_buffer;
};

} // namespace surgiview
