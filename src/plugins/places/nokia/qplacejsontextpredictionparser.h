#ifndef QPLACEJSONTEXTPREDICTIONPARSER_H
#define QPLACEJSONTEXTPREDICTIONPARSER_H

#include <QObject>
#include <QStringList>

#include <qmobilityglobal.h>

class QScriptEngine;

QTM_BEGIN_NAMESPACE

class QPlaceJSonTextPredictionParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonTextPredictionParser(QObject *parent = 0);

    QStringList predictions();
signals:
    void finished(const QPlaceJSonTextPredictionParser::Error &error, const QString &errorMessage);
public slots:
    void processData(const QString &data);
private:
    QScriptEngine *engine;
    QStringList suggestions;
};

QTM_END_NAMESPACE

#endif // QPLACEJSONTEXTPREDICTIONPARSER_H
