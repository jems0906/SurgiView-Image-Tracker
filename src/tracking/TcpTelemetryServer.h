#pragma once

#include <QObject>
#include <QHash>

class QTcpServer;
class QTcpSocket;

namespace surgiview {

class TcpTelemetryServer : public QObject {
    Q_OBJECT

public:
    explicit TcpTelemetryServer(QObject* parent = nullptr);
    ~TcpTelemetryServer() override;

    bool start(quint16 port, QString* errorMessage = nullptr);
    void stop();

    bool isListening() const;
    quint16 port() const;

signals:
    void listeningChanged();
    void telemetryReceived(double x, double y, double depthMm);
    void serverMessage(const QString& message);

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();

private:
    void parseLine(const QByteArray& line);

    QTcpServer* m_server = nullptr;
    QHash<QTcpSocket*, QByteArray> m_buffers;
};

} // namespace surgiview
