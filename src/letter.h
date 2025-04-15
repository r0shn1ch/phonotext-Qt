#pragma once

#include <QString>

class Letter
{
public:
    Letter();
    Letter(const QString& symbol);

    ~Letter();

    QString origin;
    QString technic;
    QString printable;
    bool isConsonant;
    bool isVolve;
    int syll;
    int pwr;
    int fw_pos;
    int number;
    int word;
    int pEnd;
    bool accent;
    int w_pos;

    QString getLetter();
    QString getLetterRepr() const; // Добавлен const

    bool operator ==(const Letter& letter) const;
};
