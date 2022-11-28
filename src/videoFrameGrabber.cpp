#include "videoFrameGrabber.h"

#include <QVideoFrame>
#include <QCamera>
#include <QCameraViewfinder>


VideoFrameGrabber::VideoFrameGrabber(QObject *parent)
    : QAbstractVideoSurface(parent)
    , source(0)
    , m_videoFinderSettings(nullptr)
{
}

QList<QVideoFrame::PixelFormat> VideoFrameGrabber::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
           << QVideoFrame::Format_ARGB32
           << QVideoFrame::Format_ARGB32_Premultiplied
           << QVideoFrame::Format_RGB32
           << QVideoFrame::Format_RGB24
           << QVideoFrame::Format_RGB565
           << QVideoFrame::Format_RGB555
           << QVideoFrame::Format_ARGB8565_Premultiplied
           << QVideoFrame::Format_BGRA32
           << QVideoFrame::Format_BGRA32_Premultiplied
           << QVideoFrame::Format_BGR32
           << QVideoFrame::Format_BGR24
           << QVideoFrame::Format_BGR565
           << QVideoFrame::Format_BGR555
           << QVideoFrame::Format_BGRA5658_Premultiplied
           << QVideoFrame::Format_AYUV444
           << QVideoFrame::Format_AYUV444_Premultiplied
           << QVideoFrame::Format_YUV444
           << QVideoFrame::Format_YUV420P
           << QVideoFrame::Format_YV12
           << QVideoFrame::Format_UYVY
           << QVideoFrame::Format_YUYV
           << QVideoFrame::Format_NV12
           << QVideoFrame::Format_NV21
           << QVideoFrame::Format_IMC1
           << QVideoFrame::Format_IMC2
           << QVideoFrame::Format_IMC3
           << QVideoFrame::Format_IMC4
           << QVideoFrame::Format_Y8
           << QVideoFrame::Format_Y16
           << QVideoFrame::Format_Jpeg
           << QVideoFrame::Format_CameraRaw
           << QVideoFrame::Format_AdobeDng;
}

bool VideoFrameGrabber::present(const QVideoFrame &frame)
{
    if (frame.isValid()) {
        emit videoFrameProbed(frame);
        return true;
    }
    return false;
}

bool VideoFrameGrabber::setSource(QCamera *source)
{
    this->source=source;
    source->setViewfinder(this);
    QCameraViewfinderSettings viewfinderSettings;
    // I have a broken camera that only support these settings, but you should put whatever settings you need here
    viewfinderSettings.setResolution(1920, 1440);
    if(m_videoFinderSettings)
    {
        source->setViewfinderSettings(*m_videoFinderSettings);
    }
    else
    {
        source->setViewfinderSettings(viewfinderSettings);
    }
    return true;
}


bool VideoFrameGrabber::isActive() const
{
    return (0!=source);
}

void VideoFrameGrabber::setVideoFinderSettings(QCameraViewfinderSettings * settings)
{
    m_videoFinderSettings = settings;
}
