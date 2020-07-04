#ifndef __QT_FF_H
#define __QT_FF_H

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


class QtFF: public QObject
{
    Q_OBJECT
public:
    explicit QtFF(QObject *parent=0);
    virtual ~QtFF();

    void play(QUrl url);

private:
    AVFormatContext *m_AvFormatContext;
    AVFrame *m_AvFrame;

    QMutex m_mutex;

signals:
    void GetImage(const QImage &image);


};

#endif //_QT_FF_H
