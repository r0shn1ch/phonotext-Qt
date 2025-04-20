#pragma once

#include <iostream>
#include <vector>
#include <forward_list>
#include <set>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <bitset>
#include <list>

#include <QDebug>
#include <QElapsedTimer>

#include "../include/nlohmann/json/json.hpp"
#include "conf.h"
#include "letter.h"
#include "phonotext.h"

using namespace nlohmann::literals;

class Proccessing
{
public:
	Proccessing(Phonotext pt, std::string lng, double min_pwr, double max_pwr);

	Phonotext getResult() { return this->pt; }
	void print(std::string);
	void createJson(std::string);

private:
	Phonotext pt;
	Conf CONFIG;
	double min_pwr = 0;
	double max_pwr = 0;
	std::vector<nlohmann::json> outJson;
    QElapsedTimer timer;
	std::vector<std::forward_list<Letter>::iterator> volveIterators;

	void proccess();

	void modifyProccessor();
	void sameProccessor();
	void joinProccessor();
	void numberProccessor();
	void finderVolve();
	void SPmaxProccessor();
	std::pair<int, std::vector<int>> findLocalWordsInds(std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator> localSP);
	void combinationsProccessor(int N = 2);
	void repeatProccessor();
    std::pair<bool, double> rusFilterComb(const QVector<std::forward_list<Letter>::iterator>& comb, const QVector<QString>& words);

    double get_pwr (const std::forward_list<Letter>::iterator &a, const std::forward_list<Letter>::iterator& b);
    double get_pwr_combs (const QVector<std::forward_list<Letter>::iterator>& combA, const QVector<std::forward_list<Letter>::iterator>& combsB);
    double handlePower(std::map<std::string, Repeat>& repeats);
	std::chrono::milliseconds ttttt;
};

