#include "phonotext.h"
Phonotext::Phonotext()
{
}

Phonotext::Phonotext(QString& text)
{
    if (text == "")
        return;

    // Генерация односвязного списка букв
    basetext.emplace_front(Letter(" "));

    std::forward_list<Letter>::iterator iter = basetext.begin();
    for (int i = 0, l = 0; i < text.size(); i++)
    {
        //for(l = 0; text[i].unicode() & (0x80 >> l); ++l); l = (l)?l:1; // find next letter
        //qDebug() << text[i] << " " << l << "\n";
        basetext.emplace_after(iter, Letter(text.at(i)));
        ++iter;
    }
}

Phonotext::~Phonotext()
{
}

std::pair<int, int> Phonotext::countLetters()
{
	std::pair<int, int> count(0, 0); // <countSymbols, countVolves>

	for (auto &symb : basetext)
	{
		if (symb.isConsonant)
			count.first++;
		if (symb.isVolve && symb.printable != "\n" && symb.printable != "--endl") // Вернуться к вопросу хранения данных ("--endl")
		{
			count.second++;
			count.first++;
		}
	}

	return count;
}

QString Phonotext::getOrigin()
{
    QString originText = "";

	for (auto& symb : basetext)
        originText += symb.origin;

	return originText;
}

QString Phonotext::getTechnic()
{
    QString technicText = "";

	for (auto& symb : basetext)
        technicText += symb.technic;

	return technicText;
}

QString Phonotext::getPrintable()
{
    QString printableText = "";

	for (auto& symb : basetext)
        printableText += symb.printable;

	return printableText;
}

QString Phonotext::getPhonotextRepr()
{
    QString reprText = "";

	for (auto& symb : basetext)
        reprText += symb.getLetterRepr();

	return reprText;
}

int Phonotext::length()
{
	int len = 0;

	for (auto& symb : basetext)
		len++;

	return len;
}
