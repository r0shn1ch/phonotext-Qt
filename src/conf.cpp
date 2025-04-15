#include "conf.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Conf::Conf() {}

Conf::Conf(const QString& lng)
{
    if (lng == "rus"){
        makeConfig("res/lng_conf/russian.json");
    }
    else if (lng == "eng")
        makeConfig("res/lng_conf/english.json");
    else if (lng == "lat")
        makeConfig("res/lng_conf/latin.json");
}

Conf::~Conf() {}

void Conf::makeConfig(const QString& lngPath)
{
    QFile file(lngPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open config file: " << lngPath;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qDebug() << "JSON document is invalid: " << lngPath;
        return;
    }

    QJsonObject data = doc.object();

    makeAlphabetConfig(data["alphabet"].toString());

    QJsonArray consonantsArray = data["consonants"].toArray();
    for (int i = 0; i < consonantsArray.size(); ++i)
        consonants.append(consonantsArray[i].toString());

    QJsonArray volvesArray = data["volves"].toArray();
    for (int i = 0; i < volvesArray.size(); ++i)
        volves.append(volvesArray[i].toString());

    words = consonants;
    words.append(volves);

    QJsonArray asOneArray = data["as_one"].toArray();
    QVector<QString> jAsOne;
    for (int i = 0; i < asOneArray.size(); ++i)
        jAsOne.append(asOneArray[i].toString());
    makeAsOneConfig(jAsOne);

    QJsonArray asSameArrays = data["as_same"].toArray();
    QVector<QVector<QString>> jAsSame;
    for (int i = 0; i < asSameArrays.size(); ++i) {
        QJsonArray innerArray = asSameArrays[i].toArray();
        QVector<QString> innerVector;
        for (int j = 0; j < innerArray.size(); ++j)
            innerVector.append(innerArray[j].toString());
        jAsSame.append(innerVector);
    }
    makeAsSameConfig(jAsSame, data["alphabet"].toString());

    QJsonObject modificationsObject = data["modifications"].toObject();
    QMap<QString, QString> jDictionary;
    for (QString key : modificationsObject.keys())
        jDictionary.insert(key, modificationsObject[key].toString());
    makeModificationsConfig(jDictionary);
}

void Conf::makeAlphabetConfig(const QString& jAlphabet)
{
    for (QChar i : jAlphabet)
        alphabet.append(i);
}

void Conf::makeAsOneConfig(const QVector<QString>& jAsOne)
{
    for (const QString& i : jAsOne)
    {
        if (asOne.find(i[0]) == asOne.end())
            asOne.insert(i[0], i);
        else
            asOne[i[0]] = i;
    }
}

void Conf::makeAsSameConfig(const QVector<QVector<QString>>& jAsSame, const QString& jAlphabet)
{
    for (int i = 0; i < jAlphabet.size(); i++)
    {
        QString tempStr(jAlphabet[i]);
        asSame.insert(tempStr, tempStr);
    }
    for (const auto& i : jAsSame)
    {
        for (const auto& j : i)
        {
            if (asSame.find(j) == asSame.end())
                asSame.insert(j, j);

            asSame[j] = i[0];
        }
    }
}

void Conf::makeModificationsConfig(const QMap<QString, QString>& jDictionary)
{
    for (const QString& key : jDictionary.keys())
    {
        QString value = jDictionary[key];

        int firstCharLen = 1;
        if (!key.isEmpty()) {
            firstCharLen = key.at(0).unicode() > 127 ? (key.at(0).unicode() > 2047 ? (key.at(0).unicode() > 65535 ? 4 : 3) : 2) : 1;
        }
        QString first = key.left(firstCharLen);
        QString second = key.mid(firstCharLen);

        if (modifications.contains(first)) {
            modifications[first].insert(second, value);
        }
        else {
            QMap<QString, QString> innerMap;
            innerMap.insert(second, value);
            modifications.insert(first, innerMap);
        }
    }
}

// Оператор присваивания
Conf& Conf::operator=(const Conf& other) {
    if (this == &other) // Проверка на самоприсваивание
        return *this;

    modifications = other.modifications;
    asOne = other.asOne;
    consonants = other.consonants;
    volves = other.volves;
    words = other.words;
    asSame = other.asSame;
    alphabet = other.alphabet;

    return *this;
}
