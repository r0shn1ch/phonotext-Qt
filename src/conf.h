#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include <algorithm>
#include <map>
#include <string>
class Conf {
public:
    Conf();
    Conf(const QString& lng);
    ~Conf();

    QVector<QChar> getAlphabet() const { return alphabet; }
    QVector<QString> getConsonants() const { return consonants; }
    // std::vector<std::string> getCppConsonants() const {
    //     std::vector<std::string>cppconsonants(volves.size());
    //     std::transform(consonants.begin(), consonants.end(), cppconsonants.begin(),
    //                    [](const QString& str){ return str.toStdString(); });
    //     return cppconsonants;
    // }
    QVector<QString> getVolves() const { return volves; }
    // std::vector<std::string> getCppVolves() const {
    //     std::vector<std::string>cppvolves(volves.size());
    //     std::transform(volves.begin(), volves.end(), cppvolves.begin(),
    //                    [](const QString& str){ return str.toStdString(); });
    //     return cppvolves;
    // }
    QVector<QString> getWords() const { return words; }
    // std::vector<std::string> getCppWords() const {
    //     std::vector<std::string>cppwords(words.size());
    //     std::transform(words.begin(), words.end(), cppwords.begin(),
    //                    [](const QString& str){ return str.toStdString(); });
    //     return cppwords;
    // }
    QMap<QChar, QString> getAsOne() const { return asOne; }
    // std::map<char, std::string> getCppAsOne() const {
    //     std::map<char, std::string>cppAsOne;
    //     std::transform(asOne.begin(), asOne.end(), std::inserter(cppAsOne, cppAsOne.begin()),
    //                    [](const QPair<QChar, QString>& pair){ return std::make_pair(pair.first.toLatin1(), pair.second.toStdString()); });
    //     return cppAsOne;
    // }
    QMap<QString, QString> getAsSame() const { return asSame; }
    // std::map<std::string, std::string> getCppAsSame() const {
    //     std::map<std::string, std::string>cppAsSame;
    //     std::transform(asSame.begin(), asSame.end(), std::inserter(cppAsSame, cppAsSame.begin()),
    //                    [](const QPair<QString, QString>& pair){ return std::make_pair(pair.first.toStdString(), pair.second.toStdString()); });
    //     return cppAsSame;
    // }
    QMap<QString, QMap<QString, QString>>* getModifications() { return &modifications; }

    Conf& operator=(const Conf& other); // Оператор присваивания

private:
    void makeConfig(const QString& lngPath);
    void makeAlphabetConfig(const QString& jAlphabet);
    void makeAsOneConfig(const QVector<QString>& jAsOne);
    void makeAsSameConfig(const QVector<QVector<QString>>& jAsSame, const QString& jAlphabet);
    void makeModificationsConfig(const QMap<QString, QString>& jDictionary);

    QVector<QChar> alphabet;
    QVector<QString> consonants;
    QVector<QString> volves;
    QVector<QString> words;
    QMap<QChar, QString> asOne;
    QMap<QString, QString> asSame;
    QMap<QString, QMap<QString, QString>> modifications;
};
