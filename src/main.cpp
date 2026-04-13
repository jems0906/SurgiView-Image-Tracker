#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "app/FrameImageProvider.h"
#include "app/SurgiViewController.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    qRegisterMetaType<surgiview::TrackerSample>("surgiview::TrackerSample");

    surgiview::SurgiViewController controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("surgiView"), &controller);
    engine.addImageProvider(QStringLiteral("surgiview"), new surgiview::FrameImageProvider(&controller));

    const QUrl url(QStringLiteral("qrc:/SurgiView/qml/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
