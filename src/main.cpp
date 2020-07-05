#include <QApplication>
#include <QCommandLineParser>


#include "ffqt.h"
#include "ff_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion("0.01");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("url", "Stream URL to playback");

    parser.process(a);
    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        QString errorMessage("Required positional argument 'url' missing");
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    }    
    
    const QString urlStr = args.at(0);
    
    // const QUrl url("http://i.imgur.com/gQghRNd.mp4");
    QUrl url(urlStr);
    FFQT ff(url);

    FFWidget ffWidget(&ff);
    ffWidget.show();

    ff.play();

    return a.exec();
}
