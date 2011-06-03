#ifndef QPLACEJSONSEARCHPARSER_H
#define QPLACEJSONSEARCHPARSER_H

#include <QObject>
#include <QList>

#include "../qplacecategory.h"
#include "../qplacesearchresult.h"

class QScriptEngine;
class QScriptValue;

QTM_BEGIN_NAMESPACE

class QPlaceJSonSearchParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonSearchParser(QObject *parent = 0);
    virtual ~QPlaceJSonSearchParser();

    QList<QPlaceSearchResult> searchResults();

signals:
    void finished(const Error &error, const QString &errorMessage);

public slots:
    void processData(const QString &data);

private:
    void processResultElement(const QScriptValue &value);
    QPlaceSearchResult processPlaceElement(const QScriptValue &value);
    void processContacts(const QScriptValue &properties, QPlace *place);
    void processCategories(const QScriptValue &categories, QPlace *place);
    void processRating(const QScriptValue &properties, QPlace *place);
    void processAddress(const QScriptValue &properties, QPlaceLocation *location);
    void processLocation(const QScriptValue &properties, QPlace *place);
private:
    QScriptEngine *engine;
    QList<QPlaceSearchResult> searchResultsList;
};

QTM_END_NAMESPACE

#endif // QPLACEJSONSEARCHPARSER_H
