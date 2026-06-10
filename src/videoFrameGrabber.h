#ifndef POORMANSPROBE_HPP
#define POORMANSPROBE_HPP

#include <QObject>
#include <QVideoFrame>

class QCamera;
class QVideoSink;
class QMediaCaptureSession;

class VideoFrameGrabber : public QObject
{
    Q_OBJECT

private:
    QCamera * source;
    QVideoSink * m_videoSink;
    QMediaCaptureSession * m_captureSession;

public:
    explicit VideoFrameGrabber(QObject *parent = nullptr);
    ~VideoFrameGrabber() override;

    bool setSource(QCamera *source);
    bool isActive() const;

signals:
    // Users of this class will get frames via this signal
    void videoFrameProbed(const QVideoFrame &videoFrame);
    void flush();
};

#endif // POORMANSPROBE_HPP
