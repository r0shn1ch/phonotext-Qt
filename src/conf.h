#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include "logger.h"

class Conf {
public:
    Conf();
    Conf(QString lng);
    ~Conf();

    QVector<QChar> getAlphabet() const { return alphabet; }
    QVector<QString> getConsonants() const { return consonants; }
    QVector<QString> getVolves() const { return volves; }
    QVector<QString> getWords() const { return words; }
    QMap<QChar, QString> getAsOne() const { return asOne; }
    QMap<QString, QString> getAsSame() const { return asSame; }
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
