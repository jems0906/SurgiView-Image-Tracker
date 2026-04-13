#include "app/FrameImageProvider.h"

#include "app/SurgiViewController.h"

namespace surgiview {

FrameImageProvider::FrameImageProvider(SurgiViewController* controller)
    : QQuickImageProvider(QQuickImageProvider::Image)
    , m_controller(controller)
{
}

QImage FrameImageProvider::requestImage(const QString&, QSize* size, const QSize& requestedSize)
{
    QImage image = m_controller->currentImage();

    if (requestedSize.isValid()) {
        image = image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (size != nullptr) {
        *size = image.size();
    }

    return image;
}

} // namespace surgiview
