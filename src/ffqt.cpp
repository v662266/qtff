#include "ffqt.h"

#include <QDateTime>
#include <QDebug>
#include <QImage>
#include <qtconcurrentrun.h>
#include <QPainter>

//https://github.com/joncampbell123/composite-video-simulator/issues/5
#undef av_err2str
#define av_err2str(errnum) av_make_error_string((char*)__builtin_alloca(AV_ERROR_MAX_STRING_SIZE), AV_ERROR_MAX_STRING_SIZE, errnum)


FFQT::FFQT(QUrl url, QObject *parent)
        : m_url(url),
        m_videoStreamIndex(-1),
        m_fps(-1)
{
    av_register_all();
    avformat_network_init();
    m_avFormatContext = avformat_alloc_context();
    m_avFrame = av_frame_alloc();
}

FFQT::~FFQT() 
{
    avformat_free_context(m_avFormatContext);
    av_frame_free(&m_avFrame);

    avformat_network_deinit();
}

void FFQT::initStream() 
{
    //open url
    int retCode = avformat_open_input(&m_avFormatContext, 
                                        m_url.toString().toStdString().data(),
                                        NULL, 
                                        NULL);
    if (retCode < 0) {
        qDebug() << "Failed to open url "
                << m_url
                <<", code "
                << retCode;
        throw std::runtime_error("Failed to open input");
    }

    //get stream info
    retCode = avformat_find_stream_info(m_avFormatContext, NULL);
    if (retCode < 0) {
        qDebug() << "Failed to get stream info";
        throw std::runtime_error("Failed to get stream info");
    }

    m_expectedDuration = m_avFormatContext->duration;

    //TODO: think how to move it to log
    av_dump_format(m_avFormatContext, 0, m_url.toString().toStdString().data(), 0);
    // Find the first video stream
    
    for(int i = 0; i < m_avFormatContext->nb_streams; ++i) {
        if(m_avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }
    if(m_videoStreamIndex == -1)
        throw std::runtime_error("No video stream found");
}

void FFQT::initDecoder() 
{
    const auto videoStream = m_avFormatContext->streams[m_videoStreamIndex];
    m_fps = videoStream->r_frame_rate.num / videoStream->r_frame_rate.den;

    m_avCodecContext = videoStream->codec;
    m_videoWidth = m_avCodecContext->width;
    m_videoHeight = m_avCodecContext->height;

    avpicture_alloc(&m_avPicture, 
                    AV_PIX_FMT_RGB24,
                    m_videoWidth,
                    m_videoHeight);

    m_avCodec = avcodec_find_decoder(m_avCodecContext->codec_id);
    m_swsContext = sws_getContext(m_videoWidth, m_videoHeight,
                                AV_PIX_FMT_YUV420P, m_videoWidth, m_videoHeight,
                                AV_PIX_FMT_RGB24,
                                SWS_BICUBIC,
                                0,0,0);

    int result=avcodec_open2(m_avCodecContext, m_avCodec, NULL);
    if (result<0){
        qDebug()<<"failed to open codec";
        throw std::runtime_error("Failed to open codec");
    }
}


void FFQT::runStreamLoop()
{
    AVPacket avPacket;
    int frameFinished = 0, frameCount = 0;
    const QDateTime startTime = QDateTime::currentDateTime();

    while (true){
        const QDateTime frameStartTime = QDateTime::currentDateTime();
        int avResult = av_read_frame(m_avFormatContext, &avPacket);
        if (avResult == AVERROR_EOF || (m_avFormatContext->pb && m_avFormatContext->pb->eof_reached)) {
            qDebug() << "End of stream";
            emit this->stopStream();
            break;
        }
        if (avResult < 0) {
            char* strError = av_err2str(avResult);

            qDebug() << "Read error "
                    << avResult
                    << " " << strError;
            break;
        }
        //Otherwise
        // qDebug() << "---";
        // qDebug() << "Frame "
        //         << "fps" << m_fps
        //         << "pos" << avPacket.pos
        //         << "cnt" << frameCount
        //         << "tim" << startTime.msecsTo(frameStartTime) / 1000.0;
        QString imgLog = "Expected duration: " + QString::number(m_expectedDuration / (1000.0 * 1000.0));
        imgLog += "; elapsed: " + QString::number(startTime.msecsTo(frameStartTime) / 1000.0);
        imgLog += "; frame no: " + QString::number(frameCount++);

        if(avPacket.stream_index == m_videoStreamIndex){
            avcodec_decode_video2(m_avCodecContext, m_avFrame, &frameFinished, &avPacket);
            if (frameFinished){
                this->m_mutex.lock();
                sws_scale(m_swsContext,
                        (const uint8_t* const *)m_avFrame->data,
                        m_avFrame->linesize,
                        0,
                        m_videoHeight,
                        m_avPicture.data,
                        m_avPicture.linesize);
                
                QImage image(m_avPicture.data[0],
                            m_videoWidth,
                            m_videoHeight,
                            QImage::Format_RGB888);
                QPainter p;
                if (p.begin(&image)) {
                    p.setPen(QPen(Qt::red));
                    p.setFont(QFont("Times", 12, QFont::Bold));
                    p.drawText(image.rect(), Qt::AlignRight, imgLog);
                    p.end();
                }
                emit this->getFrame(image);
                this->m_mutex.unlock();
            }
        }
   
        av_free_packet(&avPacket);

        const double frameDuration = 1000.0 / m_fps;
        const QDateTime frameEndTime = QDateTime::currentDateTime();
        const auto elapsed = frameStartTime.msecsTo(frameEndTime);
        const auto remains = frameDuration - elapsed;
        if (remains > 0) {
            // qDebug() << "frame duration" << frameDuration
            //     << "elapsed" << elapsed
            //     << "remains" << remains; 
            // qDebug() << "before" << QDateTime::currentDateTime();
            QThread::msleep(remains);
            // qDebug() << "after" << QDateTime::currentDateTime();
        }        
    }
}


void FFQT::play() 
{
    QtConcurrent::run([=]{
        try {
            this->initStream();
            this->initDecoder();
            this->runStreamLoop();
        }
        catch(const std::exception &e) {
            qDebug() << "Got exception: " << e.what();
            emit this->playbackError(QString(e.what()));
            throw(e);
        }
    });
}
