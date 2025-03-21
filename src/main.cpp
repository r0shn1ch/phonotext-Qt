#include "qtmainwindow.h"
#include <QApplication>
#include <fstream>
#include <chrono>

#include "phonotext.h"
#include "proccessing.h"

int main(int argc, char *argv[])
{
    auto begin = std::chrono::steady_clock::now();

#ifdef _WIN32
    system("chcp 65001");
#endif

    std::ifstream fin;
    fin.open("res/data/in1.txt", std::ios_base::in);
    if (!fin.is_open()){
        std::cout << "File cannot be opened" << std::endl;
        return 0;
    };

    std::cout << "start\n";

    std::string data;

    while (!fin.eof())
    {
        std::string line;
        std::getline(fin, line);
        data += line + '\n';
    }

    fin.close();
    std::cout << "Read End";
    Phonotext pt(data);
    Proccessing proc(pt, "rus", 0., 100.);

    proc.createJson("res/data/outJson.json");
    proc.print("res/data/out.txt");

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "The time: " << elapsed_ms.count() << " ms\n";

    // frontend
    //QApplication a(argc, argv);
    //QtMainWindow w;
    //w.show();
    //return a.exec();
    return 0;
}
