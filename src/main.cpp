#include <QCoreApplication>

#include "ffqt.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FFQT *ff = new FFQT();
    QUrl url("http://i.imgur.com/gQghRNd.mp4");
    ff->play(url);


    return a.exec();
}
