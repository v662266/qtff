#ifndef __FF_QT_H
#define __FF_QT_H

#include <QObject>
#include <QUrl>
#include <QMutex>
#include <QSharedPointer>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
}

class FFQT: public QObject
{
    Q_OBJECT
public:
    explicit FFQT(QUrl url, QObject *parent=0);
    virtual ~FFQT();

    void play();

private:
    const QUrl m_url;
    QMutex m_mutex;
    int m_videoStreamIndex;
    int m_fps;
    int m_expectedDuration;

    int m_videoWidth;
    int m_videoHeight;

    AVFormatContext *m_avFormatContext;
    AVFrame *m_avFrame;
    AVPicture m_avPicture;
    AVCodecContext *m_avCodecContext;
    AVCodec *m_avCodec;
    SwsContext *m_swsContext;

    void initStream();
    void initDecoder();
    void runStreamLoop();
signals:
    void getFrame(const QImage&);
    void stopStream();


};

#endif //__FF_QT_H
