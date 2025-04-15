#pragma once

#include <forward_list>
#include <vector>
#include <QMap>

#include <QSet>

#include "letter.h"

struct Repeat
{
	int count;
	double power;
	std::vector<Letter> letters;
	std::vector<std::vector<std::forward_list<Letter>::iterator>> combs;
    QSet<QString> _words;
};

class Phonotext
{
public:
	Phonotext();
    Phonotext(QString& text);
	~Phonotext();

	std::forward_list<Letter> basetext;
	std::vector<std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator>> SP;
	std::vector<std::vector<std::vector<std::forward_list<Letter>::iterator>>> syllableCombinations;
    QMap<QString, Repeat> repeats;


	std::pair<int, int> countLetters();
    QString getOrigin();
    QString getTechnic();
    QString getPrintable();
    QString getPhonotextRepr();
	int length();
};

