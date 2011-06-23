#ifndef QPLACEJSONPARSER_P_H
#define QPLACEJSONPARSER_P_H

#include <QObject>
#include <QStringList>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

class QPlaceJSonParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonParser(QObject *parent = 0);

signals:
    void finished(const QPlaceJSonParser::Error &error, const QString &errorMessage);

public slots:
    void processData(const QString &data);

private:
    virtual void processJSonData(const QScriptValue &sv) = 0;

protected:
    QScriptEngine *engine;
};

#endif // QPLACEJSONPARSER_P_H
