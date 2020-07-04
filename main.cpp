#include <QCoreApplication>

#include "qtff.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtFF *ff = new QtFF();
    QUrl url("http://i.imgur.com/gQghRNd.mp4");
    ff->play(url);


    return a.exec();
}
