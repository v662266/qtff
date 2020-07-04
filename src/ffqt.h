#ifndef __FF_QT_H
#define __FF_QT_H

#include <QObject>
#include <QUrl>
#include <QMutex>

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
    explicit FFQT(QObject *parent=0);
    virtual ~FFQT();

    void play(QUrl url);

private:
    AVFormatContext *m_AvFormatContext;
    AVFrame *m_AvFrame;

    QMutex m_mutex;

signals:
    void GetImage(const QImage &image);


};

#endif //__FF_QT_H
