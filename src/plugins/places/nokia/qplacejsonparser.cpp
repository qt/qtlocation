#include "qplacejsonparser_p.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QPlaceJSonParser::QPlaceJSonParser(QObject *parent) :
    QObject(parent),
    engine(NULL)
{
}

void QPlaceJSonParser::processData(const QString &data)
{
    if (!engine) {
        engine = new QScriptEngine(this);
    }
    QScriptValue sv = engine->evaluate("(" + data + ")");
    processJSonData(sv);
}
