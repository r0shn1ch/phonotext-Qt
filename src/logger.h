#pragma once
#include <QString>
#include <iostream>
#include <QDebug>

void Log(QString);

template <typename T>
std::ostream& Log(T str){
    return (std::cout << str);
}


