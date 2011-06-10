#ifndef QPLACEJSONMEDIAPARSER_H
#define QPLACEJSONMEDIAPARSER_H

#include <QObject>
#include <QList>

#include <qplacemediaobject.h>

class QScriptEngine;
class QScriptValue;

QTM_BEGIN_NAMESPACE

class QPlaceJSonMediaParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonMediaParser(QObject *parent = 0);
    virtual ~QPlaceJSonMediaParser();

    QList<QPlaceMediaObject> resultMedia();
    uint allMediaCount();
    static QPlaceMediaObject buildMediaObject(const QScriptValue &place);

signals:
    void finished(const QPlaceJSonMediaParser::Error &error, const QString &errorMessage);

public slots:
    void processData(const QString &data);

private:
    void processMedia(const QScriptValue &contacts);

private:
    QScriptEngine *engine;
    QList<QPlaceMediaObject> media;
    uint allMedia;
};

QTM_END_NAMESPACE

#endif // QPLACEJSONMEDIAPARSER_H
