#include <QtTest>
#include <QTcpSocket>

#include "tracking/TcpTelemetryServer.h"

class TcpTelemetryServerTests : public QObject {
    Q_OBJECT

private slots:
    void acceptsJsonTelemetryOverTcp();
};

void TcpTelemetryServerTests::acceptsJsonTelemetryOverTcp()
{
    surgiview::TcpTelemetryServer server;
    QString error;
    QVERIFY(server.start(0, &error));
    QVERIFY2(server.isListening(), qPrintable(error));

    QSignalSpy spy(&server, SIGNAL(telemetryReceived(double,double,double)));

    QTcpSocket socket;
    socket.connectToHost(QHostAddress::LocalHost, server.port());
    QVERIFY(socket.waitForConnected(1000));

    socket.write("{\"x\":210.0,\"y\":160.0,\"depthMm\":12.5}\n");
    QVERIFY(socket.waitForBytesWritten(1000));

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 1000);

    const QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toDouble(), 210.0);
    QCOMPARE(args.at(1).toDouble(), 160.0);
    QCOMPARE(args.at(2).toDouble(), 12.5);

    server.stop();
}

QTEST_MAIN(TcpTelemetryServerTests)
#include "test_tcptelemetryserver.moc"
