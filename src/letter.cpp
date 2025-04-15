#include "letter.h"

Letter::Letter()
{
    origin = "";
    technic = "";
    printable = "";
    isConsonant = false;
    isVolve = false;
    syll = 0;
    pwr = 0;
    number = 0;
    word = 0;
    pEnd = 0;
    fw_pos = 0;
    accent = false;
}

Letter::Letter(const QString& symbol)
{
    origin = symbol;
    technic = "+";
    printable = symbol;
    isConsonant = false;
    isVolve = false;
    syll = 0;
    pwr = 0;
    number = 0;
    word = 0;
    pEnd = 0;
    fw_pos = 0;
    accent = false;
}

Letter::~Letter()
{
}

QString Letter::getLetter()
{
    QString outLetter = "";
    outLetter += origin;
    outLetter += "(";
    outLetter += technic;
    outLetter += printable;
    outLetter += ")";
    return outLetter;
}

QString Letter::getLetterRepr() const // Добавлен const
{
    QString outLetter = "";
    outLetter += origin;
    outLetter += "'";
    outLetter += technic;
    outLetter += "'";
    outLetter += printable;
    outLetter += "'";
    outLetter += (isConsonant ? "C" : "-");
    outLetter += (isVolve ? "V" : "-");
    outLetter += QString::number(number);
    outLetter += "'";
    outLetter += QString::number(syll);
    outLetter += "'";
    outLetter += QString::number(word);
    return outLetter;
}

bool Letter::operator==(const Letter& letter) const
{
    if (this->origin != letter.origin)
        return false;
    if (this->technic != letter.technic)
        return false;
    if (this->printable != letter.printable)
        return false;
    if (this->isConsonant != letter.isConsonant)
        return false;
    if (this->isVolve != letter.isVolve)
        return false;
    if (this->syll != letter.syll)
        return false;
    if (this->pwr != letter.pwr)
        return false;
    if (this->number != letter.number)
        return false;
    if (this->word != letter.word)
        return false;
    if (this->pEnd != letter.pEnd)
        return false;
    if (this->accent != letter.accent)
        return false;
    return true;
}
