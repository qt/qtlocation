#ifndef QPLACEMEDIAREPLYIMPL_H
#define QPLACEMEDIAREPLYIMPL_H

#include <QObject>
#include <QHash>

#include "../qplacemediareply.h"
#include "qplacerestreply.h"
#include "qplacejsonmediaparser.h"

QTM_BEGIN_NAMESPACE

class QPlaceMediaReplyImpl : public QPlaceMediaReply
{
    Q_OBJECT
public:
    explicit QPlaceMediaReplyImpl(QPlaceRestReply *reply, QObject *parent = 0);
    ~QPlaceMediaReplyImpl();
    void abort();

Q_SIGNALS:
    void processingFinished(QPlaceReply *reply);
    void processingError(QPlaceReply *reply, const Error &error, const QString &errorMessage);

private slots:
    void restError(QPlaceRestReply::Error error);
    void predictionsReady(const QPlaceJSonMediaParser::Error &error,
                          const QString &errorMessage);

private:
    QPlaceRestReply *restReply;
    QPlaceJSonMediaParser *parser;
};

QTM_END_NAMESPACE

#endif // QPLACEMEDIAREPLYIMPL_H
