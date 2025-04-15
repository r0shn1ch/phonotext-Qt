#include "qtmainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "logger.h"
#include <QElapsedTimer>
#include <QTextCodec>

#include "phonotext.h"
#include "proccessing.h"

int main(int argc, char *argv[])
{
    QElapsedTimer timer;
    timer.start();

#ifdef _WIN32
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#endif

    QFile file("res/data/in1.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File cannot be opened";
        return 0;
    }

    Log("start\n");

    QString data;
    QTextStream in(&file);
    in.setCodec("UTF-8"); // кодировка UTF-8
    data = in.readAll();

    file.close();
    Log("Read End\n");

    Phonotext pt(data);
    Proccessing proc(pt, "rus", 0., 100.);

    proc.createJson("res/data/outJson.json");
    proc.print("res/data/out.txt");

    qint64 elapsed_ms = timer.elapsed();
    Log("The time: ");
    Log(elapsed_ms);
    Log(" ms\n");

    QApplication a(argc, argv);
    QtMainWindow w;
    w.show();
    return a.exec();
}
