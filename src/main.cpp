#include <QApplication>

#include "ffqt.h"
#include "ff_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QUrl url("http://i.imgur.com/gQghRNd.mp4");
    FFQT ff(url);

    FFWidget ffWidget(&ff);
    ffWidget.show();

    ff.play();

    return a.exec();
}
