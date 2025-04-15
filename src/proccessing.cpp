#include "proccessing.h"
#include <climits>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include "logger.h"
Proccessing::Proccessing(Phonotext pt, QString lng, double min_pwr, double max_pwr)
    : pt(pt), CONFIG(lng), min_pwr(min_pwr), max_pwr(max_pwr)
{
    Log("proccess start\n");
    Log("Add config\n");
    Log("Config added\n");

    timer.start();
    this->proccess();
    Log("Processing time: ");
    Log(timer.elapsed());
    Log(" ms\n");
}

// Полная обработка текста
void Proccessing::proccess()
{
    Log("modify\n");
    modifyProccessor(); // Замена
    Log("same\n");
    sameProccessor(); // Замена и запись
    Log("join\n");
    joinProccessor(); // Объединение
    Log("number\n");
    numberProccessor(); // Подсчёт
    Log("finder\n");
    finderVolve(); // Поиск
    Log("SP\n");
    SPmaxProccessor(); // Выборка
    Log("combinations\n");
    combinationsProccessor(); // Комбинирование
    Log("repeat\n");
    repeatProccessor(); // Подсчёт
    Log("proccessing end\n");
}


// Изменение символов по json
// Если изменять, то только напрямую с преподавателем
void Proccessing::modifyProccessor()
{
    QString tmp_a;
    QString tmp_b;
    QString tmp_c;

    auto it = pt.basetext.begin();
    auto itPreviosLetter = pt.basetext.before_begin();
    bool needChange = false;

    while (it != pt.basetext.end())
    {
        if (needChange)
        {
            needChange = false;
        }

        QString origin = it->origin;
        if (it == pt.basetext.begin())
        {
            tmp_b = origin;
        }
        else
        {
            tmp_a = tmp_b;
            tmp_b = origin;

            QMap<QString, QMap<QString, QString>>* modifications = CONFIG.getModifications();

            if (modifications) {
                auto modFirstKey = modifications->find(tmp_a);
                if (modFirstKey != modifications->end())
                {
                    auto modSecondKey = modFirstKey.value().find(tmp_b);
                    if (modSecondKey != modFirstKey.value().end())
                    {
                        tmp_c = modSecondKey.value();

                        int firstCharLen = 1;
                        if (!tmp_c.isEmpty()) {
                            firstCharLen = tmp_c.at(0).unicode() > 127 ? (tmp_c.at(0).unicode() > 2047 ? (tmp_c.at(0).unicode() > 65535 ? 4 : 3) : 2) : 1;
                        }

                        int secondCharLen = 1;
                        if (tmp_c.length() > firstCharLen) {
                            secondCharLen = tmp_c.at(firstCharLen).unicode() > 127 ? (tmp_c.at(firstCharLen).unicode() > 2047 ? (tmp_c.at(firstCharLen).unicode() > 65535 ? 4 : 3) : 2) : 1;
                        }

                        QString firstChar = tmp_c.left(firstCharLen);
                        QString secondChar = tmp_c.mid(firstCharLen, secondCharLen);
                        QString thirdPart = tmp_c.mid(firstCharLen + secondCharLen);

                        itPreviosLetter->origin = firstChar;
                        it->origin = secondChar;
                        auto next_it = pt.basetext.emplace_after(it, Letter(thirdPart));

                        auto b = it;
                        (++b)->printable = it->printable;
                        it->printable = "`";
                        it = next_it;
                        needChange = true;
                    }
                }
            }
        }
        itPreviosLetter = it;
        ++it;
    }
}


// Запись technic и printable, а так же замена символов в первом, если они являются одинаковыми по json
// P.S. стоит вопрос по переписи
void Proccessing::sameProccessor()
{
    for (auto& symb : pt.basetext)
    {
        auto asSame = CONFIG.getAsSame();
        auto sameKey = asSame.find(symb.printable);
        auto sameKeyEnd = asSame.end();
        if (sameKey != sameKeyEnd)
            symb.technic = sameKey.value();
        else if (symb.technic == "+")
            symb.technic = "&";

        if (symb.origin == " ")
            symb.technic = "|";

        // if (symb.technic == "&" || symb.technic == "|")
            // symb.printable = symb.technic;

        if (symb.technic == "&")
            symb.technic = symb.origin;
    }
}

// Объединение объектов, если они являются одним символом
void Proccessing::joinProccessor()
{
    QString tmp_a; // Первый символ совмещающейся в комбинации
    QString tmp_b; // Второй символ совмещающейся в комбинации

    // Поиск итератора на последний объект в Phonotext
    auto itLast = pt.basetext.begin();
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++)
        itLast = it;

    auto it = pt.basetext.begin(); // Данная переменная будет итератором на объект, с которым происходит работа в данный момент
    auto itPreviosLetter = pt.basetext.begin(); // Данная переменная будет указателем на объект, с которым происходила работа в прошлый раз

    while (it != pt.basetext.end()) // Проходка до конца
    {
        if (it == pt.basetext.begin()) // В первый раз указывается только первый символ ищущейся комбинации
            tmp_a = it->origin;
        else
        {
            tmp_a = itPreviosLetter->origin; // Запись первого символа (origin из предыдущего итератора)
            tmp_b = it->origin; // Запись второго символа (origin из настоящего символа)

            auto asOne = CONFIG.getAsOne();
            auto oneKey = asOne.find(tmp_a[0]); // Поиск комбинации по первому символу
            auto oneKeyEnd = asOne.end();
            if (oneKey != oneKeyEnd) // Если комбинация найдена
            {
                if ((tmp_a + tmp_b) == oneKey.value()) // Если наша пара совпадает с комбинацией
                {
                    // Перезапись прошлого итератора
                    itPreviosLetter->origin = oneKey.value();
                    itPreviosLetter->printable = oneKey.value();
                    itPreviosLetter->technic = oneKey.value();
                    if (it == itLast) // Если это последний элемент, то удаление и выход
                    {
                        pt.basetext.erase_after(itPreviosLetter);
                        break;
                    }
                    // Если это не последний элемент, то перескок на следующий итератор, во избежание ошибки и удаление настоящего символа
                    it = itPreviosLetter;
                    pt.basetext.erase_after(itPreviosLetter);
                }
            }
            if (tmp_b == "\u0301"){
                // ударение
                itPreviosLetter->printable = itPreviosLetter->printable + tmp_b;
                itPreviosLetter->accent = true;
                it = itPreviosLetter;
                pt.basetext.erase_after(itPreviosLetter);
            }
            if (tmp_b == "\u0484"){
                // удаление символа мягкости
                it = itPreviosLetter;
                // itPreviosLetter ->printable = itPreviosLetter->printable + tmp_b;
                pt.basetext.erase_after(itPreviosLetter);
            }
            // if (tmp_a.c_str()[1] == 0 && tmp_b.c_str()[1] == 0){
            //     if (it->origin == "\n"){
            //         itPreviosLetter->origin = "\n";
            //     }
            //     else {
            //         itPreviosLetter->origin = " ";
            //     }

            //     itPreviosLetter->printable = itPreviosLetter->printable + tmp_b;
            //     it = itPreviosLetter;
            //     pt.basetext.erase_after(itPreviosLetter);
            // }
            itPreviosLetter = it; // Смена прошлого итератора на настоящий
        }
        it++; // Следующий итератор
    }
}

// Функция для подсчёта различной информации по тексту
void Proccessing::numberProccessor()
{
    int i = 0; // Для подсчёта номера буквы в тексте
    int j = 1; // Для подсчёта слога в тексте
    int k = 0; // Для подсчёта слова в тексте
    int l = 0; // Для подсчёта первой буквы в слове
    int num = 1;
    int word_len = 0;
    bool space = false; // Несёт информацию, является ли данный итерируемый объект пробелом
    int space_pos = 0; // Для подсчёта нахождения объекта в слове после пробела
    std::forward_list<Letter>::iterator beginWord = pt.basetext.begin();
    std::forward_list<Letter>::iterator lastWord = beginWord;
    int l_number = 0; // номер символа в тексте - 1
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++, l_number++)
    {
        it->number = i;
        if (it->origin == " " || it->technic == "|" || it->technic == "\n")
        {
            space_pos = 0;
            k++;
            l = 0;
            space = true;
        }
        else
        {
            auto words = CONFIG.getWords();
            space_pos++;
            // Для чего всё делается дальше не совсем помню, но по-факту это ничего не делает
            bool flag = false;
            for (int t = 0; t < words.size(); t++)
                if (it->technic == words[t])
                    flag = true;
            space = (space && flag);
            // До этого момента
        }
        if (it->technic == "\n")
        {
            int stopper = 0;
            if (j > 3) stopper = j - 3;
            lastWord = beginWord;
            while (beginWord != it)
            {
                beginWord->pEnd = 1;
                ++beginWord;
            }
        }
        // Определение гласных объектов
        auto volves = CONFIG.getVolves();
        for (int t = 0; t < volves.size(); t++)
            if (it->printable == volves[t])
            {
                it->isVolve = true;
                j++;
                break;
            }
        // Определение согласных объектов
        auto consonants = CONFIG.getConsonants();
        for (int t = 0; t < consonants.size(); t++)
            if (it->printable == consonants[t])
            {
                it->isConsonant = true;
                break;
            }
        if(it->isConsonant || it->isVolve) {
            it->word = k;
            word_len++;
        }
        else it->word = k - 1;
        it->syll = j;
        i++;
        num++;
        if (l == 1)
        {
            beginWord = it;
        }
        it->fw_pos = l;
        if (it->origin == " " || it->origin == "\n")
        {
            auto tmpIt = beginWord;
            if (word_len <= 2)
            {
                while (tmpIt != it)
                {
                    tmpIt->fw_pos = 0;
                    ++tmpIt;
                }
            }
            word_len = 0;
        }
        l++;
        it->w_pos = space_pos;

    }
}

// Нахождение итераторов гласных и добавление возврат массива со всеми этими итераторами
void Proccessing::finderVolve()
{
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++)
        if (it->isVolve)
            volveIterators.push_back(it);
}

// Создание массива элементов, которые представляют собой 
// итераторы на следующюю согласную после прошлой гласой и последнюю согласную перед следующей гласной

//SP это массив пар формата (согласная, гласная), где
//согласная это символ после текущей гласной из массива гласных из текста,
//а гласная это собственно сам текущий элемент из массива гласных,
//причем первый элемент может и не быть согласной
void Proccessing::SPmaxProccessor()
{
    std::vector<std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator>> dividedVolveIterators;

    std::forward_list<Letter>::iterator startVolveIt, middleVolveIt, endVolveIt; // итераторы на прошлую, настоящую и следующую гласную
    bool firstVolve = false;

    if (volveIterators.size() > 1) // если гласных больше одной
    {
        // первоначальное расположение итератора в SP - второй символ, т.к. первый - пробел по умолчанию
        startVolveIt = pt.basetext.begin();
        startVolveIt++;
        // итератор на первый центр = итератор на первую гласную
        middleVolveIt = volveIterators[0];

        for (int i = 1; i < volveIterators.size(); i++) // проход по всем итераторам гласных, начиная со второго
        {
            endVolveIt = volveIterators[i]; // конечный итератор элемента массива, указывающий на последнюю согласную перед следующей гласной

            dividedVolveIterators.push_back(std::make_pair(startVolveIt, endVolveIt)); // Создание пары на последнюю согласную после последней гласной и последней согласной перед следующей гласной

            // следующее начало - следующая гласная после той, которая является центральной на данный момент
            // следовательно смещаем и переназначаем
            middleVolveIt++;
            startVolveIt = middleVolveIt;
            // следующая центральная - та, которая являлась следующей на данном цикле
            middleVolveIt = endVolveIt;
            // дальше идёт условие из оригинала на ударение

            int counter = 0;
            for (auto it = startVolveIt; it != endVolveIt; it++)
                if (it->technic == "\n")
                    counter++;
        }

        // добавление элементе массива, который включает согласные после последней гласной
        endVolveIt = pt.basetext.end();
        startVolveIt++;

        if (middleVolveIt->origin != "\n")
            dividedVolveIterators.push_back(std::make_pair(startVolveIt, endVolveIt));
    }
    else // если гласная одна - элемент в массиве только один: весь Phonotext
        dividedVolveIterators.push_back(std::make_pair(pt.basetext.begin(), pt.basetext.end()));

    pt.SP = dividedVolveIterators; // перенос всех элементов в Phonotext
}

// Нахождение индексов гласной и согласных
std::pair<int, std::vector<int>> Proccessing::findLocalWordsInds(std::pair<std::forward_list<Letter>::iterator, std::forward_list<Letter>::iterator> localSP)
{
    int indVolve;
    std::vector<int> indCons;

    int i = 0;
    for (auto it = localSP.first; it != localSP.second; it++, i++)
        if (it->isVolve)
            indVolve = i;
        else
            indCons.push_back(i);

    return std::make_pair(indVolve, indCons);
}

bool isCorrectComb(std::forward_list<Letter>::iterator it1, std::forward_list<Letter>::iterator it2, std::forward_list<Letter>::iterator it3)
{            
    if (it1->origin == "\n" || it2->origin == "\n" || it3->origin == "\n")
        return false;
    if (it1->origin == "-" || it2->origin == "-" || it3->origin == "-")
        return false;
    if (it1->origin == "—" || it2->origin == "—" || it3->origin == "—")
        return false;
    if (it1->origin == "–" || it2->origin == "–" || it3->origin == "–")
        return false;
    if (it1->origin == "," || it2->origin == "," || it3->origin == ",")
        return false;
    if (it1->origin == "." || it2->origin == "." || it3->origin == ".")
        return false;
    if (it1->origin == "’" || it2->origin == "’" || it3->origin == "’")
        return false;
    if (it1->origin == "€" || it2->origin == "€" || it3->origin == "€")
        return false;
    if (it1->origin == "!" || it2->origin == "!" || it3->origin == "!")
        return false;
    if (it1->origin == "?" || it2->origin == "?" || it3->origin == "?")
        return false;
    if (it1->origin == "j" || it2->origin == "j" || it3->origin == "j")
        return false;
    if (it1->origin == " " || it2->origin == " " || it3->origin == " ")
        return false;
    if (it1->origin == ":" || it2->origin == ":" || it3->origin == ":")
        return false;

    // Замена проверкам ниже
    if ( (it1->origin[0] >= 'a' and it1->origin[0] <= 'z') or
        (it2->origin[0] >= 'a' and it2->origin[0] <= 'z') or
        (it3->origin[0] >= 'a' and it3->origin[0] <= 'z') )
        return false;
    /*
    if (it1->origin == "a" || it2->origin == "a" || it3->origin == "a")
		return false;
	if (it1->origin == "b" || it2->origin == "b" || it3->origin == "b")
			return false;
	if (it1->origin == "c" || it2->origin == "c" || it3->origin == "c")
			return false;
	if (it1->origin == "d" || it2->origin == "d" || it3->origin == "d")
			return false;
	if (it1->origin == "e" || it2->origin == "e" || it3->origin == "e")
			return false;
	if (it1->origin == "f" || it2->origin == "f" || it3->origin == "f")
			return false;
	if (it1->origin == "g" || it2->origin == "g" || it3->origin == "g")
			return false;
	if (it1->origin == "h" || it2->origin == "h" || it3->origin == "h")
			return false;
	if (it1->origin == "i" || it2->origin == "i" || it3->origin == "i")
			return false;
	if (it1->origin == "j" || it2->origin == "j" || it3->origin == "j")
			return false;
	if (it1->origin == "k" || it2->origin == "k" || it3->origin == "k")
			return false;
	if (it1->origin == "l" || it2->origin == "l" || it3->origin == "l")
			return false;
	if (it1->origin == "m" || it2->origin == "m" || it3->origin == "m")
			return false;
	if (it1->origin == "n" || it2->origin == "n" || it3->origin == "n")
			return false;
	if (it1->origin == "o" || it2->origin == "o" || it3->origin == "o")
			return false;
	if (it1->origin == "p" || it2->origin == "p" || it3->origin == "p")
			return false;
	if (it1->origin == "q" || it2->origin == "q" || it3->origin == "q")
			return false;
	if (it1->origin == "r" || it2->origin == "r" || it3->origin == "r")
			return false;
	if (it1->origin == "s" || it2->origin == "s" || it3->origin == "s")
			return false;
	if (it1->origin == "t" || it2->origin == "t" || it3->origin == "t")
			return false;
	if (it1->origin == "u" || it2->origin == "u" || it3->origin == "u")
			return false;
	if (it1->origin == "v" || it2->origin == "v" || it3->origin == "v")
			return false;
	if (it1->origin == "w" || it2->origin == "w" || it3->origin == "w")
			return false;
	if (it1->origin == "x" || it2->origin == "x" || it3->origin == "x")
			return false;
	if (it1->origin == "y" || it2->origin == "y" || it3->origin == "y")
			return false;
	if (it1->origin == "z" || it2->origin == "z" || it3->origin == "z")
			return false;
    */
    return true;
}

// Создание комбинаций
void Proccessing::combinationsProccessor(int N)
{
    N++;
    for (int i = 0; i < pt.SP.size(); i++)
    {
        // Из каждого участка выбирается гласная и все согласные в отдельный массив
        std::pair<int, std::vector<int>> localInds = findLocalWordsInds(pt.SP[i]);
        int posVolve = localInds.first;
        std::vector<int> posCons = localInds.second;

        // Всевозможные пары комбинаций из индексов массива согласных
        std::vector<std::pair<int, int>> consCombs;
        for (int j = 0; j < posCons.size(); j++)
            for (int k = j + 1; k < posCons.size(); k++)
                consCombs.push_back(std::make_pair(posCons[j], posCons[k]));

        // Сборка комбинаций из СГС в разных вариантах (в итераторах)
        std::vector<std::vector<std::forward_list<Letter>::iterator>> itCombs;
        for (int j = 0; j < consCombs.size(); j++)
        {
            // Создание сортированных комбинаций из пар индексов соглассных + индекс гласной
            std::vector<int> combs;
            combs.push_back(consCombs[j].first);
            combs.push_back(consCombs[j].second);
            combs.push_back(posVolve);
            sort(combs.begin(), combs.end());

            // Поиск итераторов элементов выбранных в пары
            std::forward_list<Letter>::iterator it1, it2, it3;
            it1 = it2 = it3 = pt.SP[i].first;

            for (int k = 0; k < combs[0]; k++)
                it1++;
            for (int k = 0; k < combs[1]; k++)
                it2++;
            for (int k = 0; k < combs[2]; k++)
                it3++;

            if (isCorrectComb(it1, it2, it3))
            {
                std::vector<std::forward_list<Letter>::iterator> tmpItCombs;
                tmpItCombs.push_back(it1);
                tmpItCombs.push_back(it2);
                tmpItCombs.push_back(it3);
                itCombs.push_back(tmpItCombs);
            }
        }
        if (itCombs.size())
            pt.syllableCombinations.push_back(itCombs);

        // Добавить то, что вытаскивается из итераторов и обработку из filter_combination
    }
}

// Функция, которая подсчитывает количество повторений комбинаций и их силы
void Proccessing::repeatProccessor()
{
    for (int n_syll = 0; n_syll < pt.syllableCombinations.size(); n_syll++)
    {

        for (auto& comb : pt.syllableCombinations[n_syll])
        {
            QVector<QString> a;
            // Создание структуры Repeat, в которой распологаются комбинации и инфа о них (комбинации состоят из СС: согласная + согласная)
            for (auto& i : comb){
                if (i->isConsonant)
                    a.push_back(i->technic);
            }
            QVector<std::forward_list<Letter>::iterator> cppCombToQt(comb.size());
            int index = 0;
            for (auto &i : comb)
            {
                cppCombToQt[index] = i;
                ++index;
            }
            // Удаление повторяющихся символов
            QSet<QString> tmpWords(a.begin(), a.end());
            QString setToStr = "";
            for (auto& i : tmpWords)
                setToStr += i;


            // Отбрасывание невалидных комбинаций
            std::pair<bool, double> filter = rusFilterComb(cppCombToQt, CONFIG.getWords());
            if (filter.first)
            {
                QMap<QString, Repeat>::iterator it = pt.repeats.find(setToStr);
                if (it == pt.repeats.end()) // Создание структуры Repeat, если структуры с такой комбинации ещё небыло
                {
                    Repeat tmpRepeat;
                    tmpRepeat._words = tmpWords; // Уникальные символы структуры
                    tmpRepeat.count = 1; // Структура только создаётся, следовательно это только первая добавленная
                    tmpRepeat.power = filter.second; // Добавление силы первой добавленной комбинации
                    for (auto& i : comb) // Добавление уникальных объектов класса Letter из временного массива в структуру
                    {
                        bool flag = false;
                        for (int j = 0; j < tmpRepeat.letters.size(); j++)
                            if (*i == tmpRepeat.letters[j])
                                flag = true;
                        if (!flag)
                            tmpRepeat.letters.push_back(*i);
                    }

                    tmpRepeat.combs.push_back(comb); // Добавление всех комбинации в структуру


                    if (tmpRepeat._words.size() != 1)
                        pt.repeats.insert(setToStr, tmpRepeat); // Перенос структуры в объект Phonotext
                }
                else
                {
                    it.value()._words = tmpWords; // Уникальные символы структуры
                    it.value().count += 1; // Так как нашлась ещё одна комбинация, счётчик увеличивается
                    it.value().power = filter.second; // Добавление силы следующей добавленной комбинации
                    for (auto& i : comb) // Добавление уникальных объектов класса Letter из временного массива в структуру
                    {
                        bool flag = false;
                        for (int j = 0; j < it.value().letters.size(); j++)
                            if (*i == it.value().letters[j])
                                flag = true;
                        if (!flag)
                            it.value().letters.push_back(*i);
                    }

                    it.value().combs.push_back(comb); // Добавление всех комбинации в структуру
                }
            }
        }
    }
    auto tmp = handlePower(pt.repeats);
    // for(auto i = pt.basetext.begin(); i != pt.basetext.end(); ++i)
    // {
    //     Log(i->origin);
    // }
    Log("\nGlobal new power: ");
    Log(tmp);
    Log("\n");

    pt.repeats.erase(pt.repeats.begin());
}

// Функция создана для фильтрации комбинаций, возвращает первым параметром <false, 0>, если комбинация является отбрасываемой,
// возвращает <true, pwr>, если комбинация валидна, где pwr - сила комбинации в тексте
// Доработать: сделать проверку на то, что текст, получаемый из комбинации являются двуконсонантным
std::pair<bool, double> Proccessing::rusFilterComb(const QVector<std::forward_list<Letter>::iterator>& comb, const QVector<QString>& words)
{
    // Запись technic символов комбинации
    std::string tmptxt = "";
    for (auto it = comb[0]; it != comb[2]; it++)
        tmptxt += it->technic.toStdString();

    // Отбрасывание повторяющихся символов
    std::string txt = "";
    for (int i = 0; i < tmptxt.size(); i++)
    {
        bool notUnique = false;
        for (int j = 0; j < txt.size(); j++)
            if (tmptxt[i] == txt[j])
                notUnique = true;

        if (!notUnique)
            txt += tmptxt[i];
    }

    // Комбинация не валидна, если получаемый текст меньше трёх символов
    if (txt.size() < 3)
        return std::make_pair(false, 0);

    // Подсчёт силы комбинации
    double pwr;
    if (comb[0]->isVolve) // Если гласная первая в комбинации
        pwr = 2;
    else if (comb[2]->isVolve) // Если гласная последняя в комбинации
        pwr = 1;
    else // Если гласная в середине комбинации
        pwr = 3;

    int count = 0;
    for (auto& i : txt) // Подсчёт количества пробелов
        if (i == '|')
            count++;
    pwr += (comb[2]->number - comb[0]->number - count == 2 ? 5 : 0); // Расположение гласных без пробелов
    pwr += (count == 0 ? 2 : 0); // количество пробелов
    count = 0;
    for (int i = txt.size() - 3; i < txt.size(); i++) // подсчёт количества букв "й" в последних трёх символах получаемого текста
        if ((txt[i] == "й"[0]) && (txt[i] == "й"[1]))
            count++;
    pwr += (count == 0 ? 4 : 0);
    pwr += (!comb[0]->w_pos || !comb[1]->w_pos || !comb[2]->w_pos ? 1 : 0); // Проверка на то, является ли символы из комбинации первым в слове
    pwr /= 15;

    return std::make_pair( (min_pwr <= pwr) && (pwr <= max_pwr), pwr);
}

// Вывод
void Proccessing::print(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug()<< "Could not open file for writing:" << filename;
        return;
    }

    QTextStream fout(&file);
    fout << "===========\n";

    fout << "-----------\n";
    fout << "origin      : ";
    for (const auto& i : pt.basetext)
    {
        fout << i.origin;
    }
    fout << "\n";

    fout << "-----------\n";
    fout << "technic     : ";
    for (const auto& i : pt.basetext)
    {
        fout << i.technic;
    }
    fout << "\n";

    fout << "-----------\n";
    fout << "printable   : ";
    for (const auto& i : pt.basetext)
    {
        fout << i.printable;
    }
    fout << "\n";

    fout << "-----------\n";
    fout << "isWord      : ";
    for (const auto& i : pt.basetext)
    {
        if (i.isVolve)
            fout << "v";
        else if (i.isConsonant)
            fout << "c";
        else
            fout << "n";
    }
    fout << "\n";

    fout << "-----------\n";
    fout << "w_pos      : ";
    for (const auto& i : pt.basetext)
    {
        fout << i.w_pos;
    }
    fout << "\n";

    fout << "-----------\n";
    fout << "SP          :\n";
    for (int i = 0; i < pt.SP.size(); i++)
    {
        fout << i << ": \"";
        for (auto it = pt.SP[i].first; it != pt.SP[i].second; it++)
        {
            fout << it->technic;
        }
        fout << "\"\n";
    }
    fout << "\n";
    for (const auto& i : pt.basetext)
    {
        fout << i.origin << i.technic << i.printable << i.isVolve << i.isConsonant << i.number << '|';
    }

    fout << "-----------\n";

    fout << "combinations:\n";
    for (int i = 0; i < pt.syllableCombinations.size(); i++)
    {
        fout << i << ":\n";
        for (int j = 0; j < pt.syllableCombinations[i].size(); j++)
        {
            for (int k = 0; k < 3; k++)
                fout << pt.syllableCombinations[i][j][k]->origin;
            fout << "\n";
        }
        fout << "\n";
    }
    fout << "\n";

    QString printable = "";
    for (auto it = pt.basetext.begin(); it != pt.basetext.end(); it++)
        printable += it->printable;

    fout << "-----------\n";
    fout << "repeats:\n";
    for (auto i = pt.repeats.begin(); i != pt.repeats.end(); ++i)
    {
        QString letters = "";
        for (int j = 0; j < i.value().letters.size(); j++)
            letters += i.value().letters[j].origin;

        QString swords = "";
        for (const auto& j : i.value()._words)
            swords += "<" + j + ">";

        fout << "key : " << i.key() << "\n";
        fout << "Repeat.power : " << i.value().power << "\n";
        fout << "Repeat.count : " << i.value().count << "\n";
        fout << "Repeat.letters : " << letters << "\n";
        fout << "Repeat._words : " << swords << "\n";
        fout << "Repeat.combs : ";

        for (int j = 0; j < i.value().combs.size(); j++) // i.value() - Repeat
        {
            for (int k = 0; k < i.value().combs[j].size(); k++)  // i.value() - Repeat
            {
                fout << i.value().combs[j][k]->origin;
            }
            fout << " ";
        }
        fout << "\n\n";
    }
    fout << "\n";
    fout << "-----------\n";
    file.close();
}


void Proccessing::createJson(QString filename)
{
    QJsonObject outJson;

    // Process basetext
    QJsonArray textArray;
    int counter = 0;
    for (const auto& j : pt.basetext)
    {
        QJsonObject tmpOutTextJson;
        tmpOutTextJson["origin"] = j.origin;
        tmpOutTextJson["technic"] = j.technic;
        tmpOutTextJson["printable"] = j.printable;
        tmpOutTextJson["is_consonant"] = j.isConsonant;
        tmpOutTextJson["is_accent"] = j.accent;
        tmpOutTextJson["is_volve"] = j.isVolve;
        tmpOutTextJson["word"] = j.word;

        QJsonObject positions;
        positions["text"] = j.number;
        positions["syllab"] = j.syll;
        positions["word_start"] = j.fw_pos;
        positions["word_end"] = 0; // word_end is incorrect
        positions["last_word_in_line"] = j.pEnd;

        tmpOutTextJson["positions"] = positions;
        textArray.append(tmpOutTextJson);
        ++counter;
    }
    outJson["text"] = textArray;

    // Process repeats
    QJsonArray repeatsArray;
    counter = 0;
    for (auto i = pt.repeats.begin(); i != pt.repeats.end(); ++i)
    {
        QString key = i.key();
        QString power = QString::number(i.value().power);
        QString count = QString::number(i.value().count);

        QJsonArray lettersArray;
        for (int j = 0; j < i.value().letters.size(); j++)
            lettersArray.append(i.value().letters[j].number);

        QString words = "";
        for (const auto& j : i.value()._words)
            words += j;

        QJsonArray combsArray;
        QJsonArray indCombsArray;

        for (int j = 0; j < i.value().combs.size(); j++)
        {
            QString tCombs = "";
            QJsonArray tmpArr;
            for (int k = 0; k < i.value().combs[j].size(); k++){
                tCombs += i.value().combs[j][k]->origin;
                tmpArr.append(i.value().combs[j][k]->number);
            }
            QJsonObject combObject;
            combObject["comb"] = tCombs;
            combObject["indices"] = tmpArr;

            combsArray.append(combObject);
            indCombsArray.append(tmpArr);

        }

        QJsonObject tmpOutRepeatJson;
        tmpOutRepeatJson["key"] = key;
        tmpOutRepeatJson["count"] = count;
        tmpOutRepeatJson["power"] = power;
        tmpOutRepeatJson["letters"] = lettersArray;
        tmpOutRepeatJson["combs"] = combsArray;
        tmpOutRepeatJson["indCombs"] = indCombsArray;

        repeatsArray.append(tmpOutRepeatJson);
        ++counter;
    }
    outJson["repeats"] = repeatsArray;

    // Write JSON to file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "File cannot be opened!" << filename;
        return;
    }

    QJsonDocument doc(outJson);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);

    QTextStream out(&file);
    out << jsonData;

    file.close();
}
double Proccessing::get_pwr (const std::forward_list<Letter>::iterator &a, const std::forward_list<Letter>::iterator &b)
{
    if (a->technic != b->technic) //если technic в letters разные, то power нулевая
        return 0;
    int dist = b->syll - a->syll; //разница между позициями слогов которые содержат данные символы (походу?)
    if (dist < 1){
        return 0;
    }
    double mul = 1; //множитель для расчета силы в зависимости от расстояния
    int dist_w = b->word - a->word; //расстояние между словами в которых находятся данные символы
    double pwr = 1. / dist + 1. / (dist_w + 2);
    if ( (a->origin == b->origin) && a->isConsonant) //если сами символы равны и а согласная
        mul += 1; //то модификатор +1
    mul *= 1. / (1 + a->fw_pos + b->fw_pos); //position.word_start - это что? как я понял позиция символа с которого начинается данное слово
    return pwr * mul; //вовзрат павер для символов
}
double Proccessing::get_pwr_combs (const QVector<std::forward_list<Letter>::iterator>& combA, const QVector<std::forward_list<Letter>::iterator>& combB)
{
    double pwr = 0;
    for (int i = 0; i < combA.size(); ++i){
        for (int j = 0; j < combB.size(); ++j){
            pwr += get_pwr(combA[i], combB[j]);
        }
    }      //обходим все символы каждой из комбинаций и считаем силу для них по формуле выше
    double mul_1 = 1;
    double mul_2 = 1;
    for (int i = 0; i < combA.size() - 1; ++i)
        mul_1 *= combA[i + 1]->number - combA[i]->number;
    //первый множитель это произведение разностей позиций в тексте каждого символа друг за другом
    //в первой комбинации
    for (int i = 0; i < combB.size() - 1; ++i)
        mul_2 *= combB[i + 1]->number - combB[i]->number;
    //второй множитель это произведение разностей позиций в тексте каждого символа друг за другом
    //во второй комбинации

    double mul = 10 * rusFilterComb(combA, CONFIG.getWords()).second
                 * rusFilterComb(combB, CONFIG.getWords()).second
                 * (1 + combA[combA.size()-1]->pEnd + combB[combB.size()-1]->pEnd);

    pwr *= 1. / (mul_1 + 1) + 1. / (mul_2 + 1);

    return pwr * mul; //вовзрат power для 2 комбинаций
}
double Proccessing::handlePower(QMap<QString, Repeat>& repeats)
{

    //общая сила повторов
    int cnt_rep = 0;
    double repeats_power = 0;
    for (auto &rep : repeats){
        double pwr = 0; //расчет
        //ищем по каждому из всех повторений
        //комбинации это вектор списков символов
        //здесь длина комбинаций это их количество в повторении
        //обходим все комбинации, в комбинациях обходим все символы
        //обходим все кобинации одного повторения каждая с каждой

        for (int i = 0; i < rep.combs.size(); ++i) {
            for (int j = i + 1; j < rep.combs.size(); ++j) {
                QVector<std::forward_list<Letter>::iterator> cppComb1ToQt(rep.combs[i].size());
                int index1 = 0;
                for (auto &comb : rep.combs[i])
                {
                    cppComb1ToQt[index1] = comb;
                    ++index1;
                }
                QVector<std::forward_list<Letter>::iterator> cppComb2ToQt(rep.combs[j].size());
                int index2 = 0;
                for (auto &comb : rep.combs[j])
                {
                    cppComb2ToQt[index2] = comb;
                    ++index2;
                }
                pwr += get_pwr_combs(cppComb1ToQt, cppComb2ToQt);
                if (rep.combs[j][0]->number - rep.combs[i][0]->number > 50)
                    break;
                //если разница в позициях у первых символов в комбинациях больше 50, то прервать
            }
        }
        rep.power = pwr;
        repeats_power += pwr;
    }

    return repeats_power;
    //для нахождения силы повторения обходим все комбинации со всеми и считаем их совместную силу, и суммируем
}
