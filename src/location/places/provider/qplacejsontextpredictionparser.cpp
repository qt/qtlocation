#include "qplacejsontextpredictionparser.h"

#include <QtScript/QScriptEngine>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

using namespace QT_PLACES_NAMESPACE;

QPlaceJSonTextPredictionParser::QPlaceJSonTextPredictionParser(QObject *parent) :
    QObject(parent),
    engine(NULL)
{
}

QStringList QPlaceJSonTextPredictionParser::predictions()
{
    return suggestions;
}

void QPlaceJSonTextPredictionParser::processData(const QString &data)
{
    if (!engine) {
        engine = new QScriptEngine(this);
    }
    suggestions.clear();

    QScriptValue sv = engine->evaluate(data);
    if (sv.isValid() && sv.isArray()) {
        qScriptValueToSequence(sv, suggestions);

#if defined(QT_PLACES_LOGGING)
    qDebug() << "QJSonParserSuggestions::processData - list value: ";
    foreach (QString str, suggestions) {
         qDebug() << str;
     }
#endif
        emit finished(NoError, QString());
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}
