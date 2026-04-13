#pragma once

#include <QQuickImageProvider>

namespace surgiview {

class SurgiViewController;

class FrameImageProvider : public QQuickImageProvider {
public:
    explicit FrameImageProvider(SurgiViewController* controller);

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

private:
    SurgiViewController* m_controller;
};

} // namespace surgiview
