#include "tracking/TcpTelemetryServer.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>

namespace surgiview {

TcpTelemetryServer::TcpTelemetryServer(QObject* parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &TcpTelemetryServer::onNewConnection);
}

TcpTelemetryServer::~TcpTelemetryServer() = default;

bool TcpTelemetryServer::start(quint16 port, QString* errorMessage)
{
    if (m_server->isListening()) {
        stop();
    }

    if (!m_server->listen(QHostAddress::Any, port)) {
        if (errorMessage != nullptr) {
            *errorMessage = m_server->errorString();
        }
        return false;
    }

    emit listeningChanged();
    emit serverMessage(QStringLiteral("Telemetry server listening on port %1").arg(m_server->serverPort()));
    return true;
}

void TcpTelemetryServer::stop()
{
    const QList<QTcpSocket*> sockets = m_buffers.keys();
    for (QTcpSocket* socket : sockets) {
        if (socket != nullptr) {
            socket->disconnect(this);
            socket->close();
            socket->deleteLater();
        }
    }
    m_buffers.clear();

    if (m_server->isListening()) {
        m_server->close();
        emit listeningChanged();
        emit serverMessage(QStringLiteral("Telemetry server stopped"));
    }
}

bool TcpTelemetryServer::isListening() const
{
    return m_server->isListening();
}

quint16 TcpTelemetryServer::port() const
{
    return m_server->serverPort();
}

void TcpTelemetryServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket* socket = m_server->nextPendingConnection();
        if (socket == nullptr) {
            continue;
        }

        m_buffers.insert(socket, QByteArray());
        connect(socket, &QTcpSocket::readyRead, this, &TcpTelemetryServer::onSocketReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &TcpTelemetryServer::onSocketDisconnected);

        emit serverMessage(QStringLiteral("Telemetry client connected"));
    }
}

void TcpTelemetryServer::onSocketReadyRead()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr || !m_buffers.contains(socket)) {
        return;
    }

    QByteArray& buffer = m_buffers[socket];
    buffer.append(socket->readAll());

    int newlineIndex = buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = buffer.left(newlineIndex).trimmed();
        buffer.remove(0, newlineIndex + 1);
        if (!line.isEmpty()) {
            parseLine(line);
        }
        newlineIndex = buffer.indexOf('\n');
    }
}

void TcpTelemetryServer::onSocketDisconnected()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr) {
        return;
    }

    m_buffers.remove(socket);
    socket->deleteLater();
    emit serverMessage(QStringLiteral("Telemetry client disconnected"));
}

void TcpTelemetryServer::parseLine(const QByteArray& line)
{
    const QJsonDocument doc = QJsonDocument::fromJson(line);
    if (!doc.isObject()) {
        emit serverMessage(QStringLiteral("Invalid telemetry payload received"));
        return;
    }

    const QJsonObject obj = doc.object();
    const QJsonValue xVal = obj.value(QStringLiteral("x"));
    const QJsonValue yVal = obj.value(QStringLiteral("y"));
    const QJsonValue depthVal = obj.contains(QStringLiteral("depthMm"))
        ? obj.value(QStringLiteral("depthMm"))
        : obj.value(QStringLiteral("depth"));

    if (!xVal.isDouble() || !yVal.isDouble() || !depthVal.isDouble()) {
        emit serverMessage(QStringLiteral("Telemetry payload missing numeric x/y/depth"));
        return;
    }

    emit telemetryReceived(xVal.toDouble(), yVal.toDouble(), depthVal.toDouble());
}

} // namespace surgiview
