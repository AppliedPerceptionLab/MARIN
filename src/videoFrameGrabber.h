#ifndef POORMANSPROBE_HPP
#define POORMANSPROBE_HPP

#include <QAbstractVideoSurface>
#include <QList>
#include <QCameraViewfinder>

class QCamera;
class QCameraViewfinder;
class QCameraViewfinderSettings;

class VideoFrameGrabber : public QAbstractVideoSurface
{
    Q_OBJECT

private:
    QCamera * source;
    QCameraViewfinderSettings * m_videoFinderSettings;
public:
    explicit VideoFrameGrabber(QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override;
    void setVideoFinderSettings(QCameraViewfinderSettings *);

    // Called from QAbstractVideoSurface whenever a new frame is present
    bool present(const QVideoFrame &frame) Q_DECL_OVERRIDE;

    bool setSource(QCamera *source);

    bool isActive() const;


signals:
    // Users of this class will get frames via this signal
    void videoFrameProbed(const QVideoFrame &videoFrame);
    void flush();

};

#endif // POORMANSPROBE_HPP
