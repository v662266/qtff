#include "ffqt.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
}

#include <QDateTime>
#include <QDebug>
#include <QImage>



FFQT::FFQT(QObject *parent)
{
    av_register_all();
    avformat_network_init();
    this->m_AvFormatContext = avformat_alloc_context();
    this->m_AvFrame = av_frame_alloc();
}

FFQT::~FFQT() 
{
    avformat_free_context(this->m_AvFormatContext);
    av_frame_free(&this->m_AvFrame);
    avformat_network_deinit();
}

void FFQT::play(QUrl url) {
    //open url
    const int urlOpen = avformat_open_input(&this->m_AvFormatContext, 
                                            url.toString().toStdString().data(),
                                            NULL, 
                                            NULL);
    if (urlOpen < 0) {
        qDebug() << "Failed to open url "
                 << url
                 <<", code "
                 << urlOpen;
        return;
    }

    //get stream info
    const int streamInfo = avformat_find_stream_info(this->m_AvFormatContext, NULL);
    if (streamInfo < 0) {
        qDebug() << "Failed to get stream info";
        return;
    }

    av_dump_format(this->m_AvFormatContext, 0, url.toString().toStdString().data(), 0);
    // Find the first video stream
    int videoStreamIndex = -1;
    for(int i = 0; i < this->m_AvFormatContext->nb_streams; ++i) {
      if(this->m_AvFormatContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
        videoStreamIndex = i;
        break;
      }
    }
    if(videoStreamIndex == -1)
      return; // Didn't find a video stream


    AVCodecContext *pAVCodecContext = this->m_AvFormatContext->streams[videoStreamIndex]->codec;
    int videoWidth=pAVCodecContext->width;
    int videoHeight=pAVCodecContext->height;

    AVPicture pAVPicture;


    avpicture_alloc(&pAVPicture, AV_PIX_FMT_RGB24,videoWidth,videoHeight);

    AVCodec *pAVCodec;
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    SwsContext *pSwsContext = sws_getContext(videoWidth,videoHeight,
                                AV_PIX_FMT_YUV420P,videoWidth,videoHeight,
                                AV_PIX_FMT_RGB24,
                                SWS_BICUBIC,
                                0,0,0);

    int result=avcodec_open2(pAVCodecContext,pAVCodec,NULL);
    if (result<0){
        qDebug()<<"failed to open codec";
        return;
    }

    qDebug()<<"init ok";

    AVPacket pAVPacket;

    int frameFinished=0;
    while (true){
        if (av_read_frame(this->m_AvFormatContext, &pAVPacket) >= 0){
            if(pAVPacket.stream_index==videoStreamIndex){
                qDebug()<<"---"<<QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
                avcodec_decode_video2(pAVCodecContext, this->m_AvFrame, &frameFinished, &pAVPacket);
                if (frameFinished){
                    this->m_mutex.lock();
                    sws_scale(pSwsContext,
                              (const uint8_t* const *)this->m_AvFrame->data,
                              this->m_AvFrame->linesize,
                              0,
                              videoHeight,
                              pAVPicture.data,
                              pAVPicture.linesize);
                    
                    QImage image(pAVPicture.data[0],videoWidth,videoHeight,QImage::Format_RGB888);
                    emit this->GetImage(image);
                    this->m_mutex.unlock();
                }
            }
        }
        av_free_packet(&pAVPacket);
    }

}