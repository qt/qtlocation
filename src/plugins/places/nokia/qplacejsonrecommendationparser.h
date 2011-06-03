#ifndef QPLACEJSONRECOMENDATIONPARSER_H
#define QPLACEJSONRECOMENDATIONPARSER_H

#include <QObject>
#include <QList>

#include <qplacesearchresult.h>

class QScriptEngine;
class QScriptValue;

QTM_BEGIN_NAMESPACE

class QPlaceJSonRecommendationParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonRecommendationParser(QObject *parent = 0);
    virtual ~QPlaceJSonRecommendationParser();

    QList<QPlaceSearchResult> results();

signals:
    void finished(const Error &error, const QString &errorMessage);

public slots:
    void processData(const QString &data);

private:
    void processResultElement(const QScriptValue &value);

private:
    QScriptEngine *engine;
    QList<QPlaceSearchResult> searchResults;
};

QTM_END_NAMESPACE

#endif // QPLACEJSONRECOMENDATIONPARSER_H
