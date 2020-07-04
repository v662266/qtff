#ifndef __FF_QT_WIDGET_H
#define __FF_QT_WIDGET_H

#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>

class FFWidget : public QFrame
 {
     Q_OBJECT
 public:
     FFWidget(QObject *videoSource, QWidget *parent = 0);
     ~FFWidget();

public slots:
    void updateFrame(const QImage&);
    void stopPlayback();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    // QLabel *m_label;
    // QVBoxLayout *m_layout;

    bool m_playing;

    QImage m_currentFrame;

 };

#endif //__FF_QT_WIDGET_H
