#include "videoFrameGrabber.h"

#include <QVideoFrame>
#include <QCamera>
#include <QVideoSink>
#include <QMediaCaptureSession>

VideoFrameGrabber::VideoFrameGrabber(QObject *parent)
    : QObject(parent)
    , source(nullptr)
    , m_videoSink(nullptr)
    , m_captureSession(nullptr)
{
}

VideoFrameGrabber::~VideoFrameGrabber()
{
}

bool VideoFrameGrabber::setSource(QCamera *source)
{
    this->source = source;
    if (!source)
        return false;

    m_captureSession = new QMediaCaptureSession(this);
    m_videoSink = new QVideoSink(this);
    
    m_captureSession->setCamera(source);
    m_captureSession->setVideoSink(m_videoSink);

    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame) {
        if (frame.isValid()) {
            emit videoFrameProbed(frame);
        }
    });

    return true;
}

bool VideoFrameGrabber::isActive() const
{
    return (nullptr != source);
}
