#ifndef QPLACEJSONCATEGORIESPARSER_H
#define QPLACEJSONCATEGORIESPARSER_H

#include <QObject>
#include <QList>
#include <QHash>

#include <qplacecategory.h>

class QScriptEngine;
class QScriptValue;

QTM_BEGIN_NAMESPACE

class QPlaceJSonCategoriesParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonCategoriesParser(QObject *parent = 0);
    virtual ~QPlaceJSonCategoriesParser();

    static QList<QPlaceCategory> processCategories(const QScriptValue &categories);

    QList<QPlaceCategory> resultCategories();

signals:
    void finished(const Error &error, const QString &errorMessage);

public slots:
    void processData(const QString &data);

private:
    static QPlaceCategory processCategory(const QScriptValue &categoryValue);
    static QList<QPlaceCategory> processGroups(const QScriptValue &categories);
    static QList<QPlaceCategory> processGroup(const QScriptValue &group);
private:
    QScriptEngine *engine;
    QList<QPlaceCategory> allCategories;
};

QTM_END_NAMESPACE

#endif // QPLACEJSONCATEGORIESPARSER_H
