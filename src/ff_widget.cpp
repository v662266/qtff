#include "ff_widget.h"

#include <QPainter>
#include <QDebug>
#include <QPixmap>

FFWidget::FFWidget(QObject *videoSource, QWidget *parent)
    : m_playing(false)
{
    if (videoSource == NULL) {
        throw std::runtime_error("Need video source to proceed");
    }

    // m_label = new QLabel(this);
    // m_layout = new QVBoxLayout(this);
    // m_layout->addWidget(m_label);
    // setLayout(m_layout);

    connect(videoSource, SIGNAL(getFrame(const QImage&)), this, SLOT(updateFrame(const QImage&)));
    connect(videoSource, SIGNAL(stopStream()), this, SLOT(stopPlayback()));
}

FFWidget::~FFWidget()
{
    //
}

void FFWidget::updateFrame(const QImage& frame)
{
    const QSize frameSize = frame.size();
    const QSize selfSize = this->size();
    if (frameSize != selfSize) {
        // this->resize(frameSize);
        this->setFixedSize(frameSize);
    }
    m_playing = true;
    this->m_currentFrame = frame;

    // this->m_label->setPixmap(QPixmap::fromImage(frame));
    this->update();
}

void FFWidget::stopPlayback()
{
    qDebug() << "Stop playback";
    this->m_playing = false;
    this->repaint();
    // this->m_label->setText("Stop");
}

void FFWidget::paintEvent(QPaintEvent *event) 
{
    // qDebug() << "Paint event" << m_playing;
    QPainter painter(this);
    QRect rect = this->rect();
    painter.drawImage(rect, this->m_currentFrame);
    QFont font("times", 24);
    painter.setFont(font);
    const QString textOnImg = m_playing ? "Streaming..." : "No stream";
    painter.drawText(rect, textOnImg);
}