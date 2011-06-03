#ifndef QPLACETEXTPREDICTIONREPLYIMPL_H
#define QPLACETEXTPREDICTIONREPLYIMPL_H

#include <QObject>
#include <QHash>

#include "../qplacetextpredictionreply.h"
#include "qplacerestreply.h"
#include "qplacejsontextpredictionparser.h"

QTM_BEGIN_NAMESPACE

class QPlaceTextPreditionReplyImpl : public QPlaceTextPredictionReply
{
    Q_OBJECT
public:
    explicit QPlaceTextPreditionReplyImpl(QPlaceRestReply *reply, QObject *parent = 0);
    ~QPlaceTextPreditionReplyImpl();
    void abort();

Q_SIGNALS:
    void processingFinished(QPlaceReply *reply);
    void processingError(QPlaceReply *reply, const Error &error, const QString &errorMessage);

private slots:
    void restError(QPlaceRestReply::Error error);
    void predictionsReady(const QPlaceJSonTextPredictionParser::Error &error,
                          const QString &errorMessage);

private:
    QPlaceRestReply *restReply;
    QPlaceJSonTextPredictionParser *parser;
};

QTM_END_NAMESPACE

#endif // QPLACETEXTPREDICTIONREPLYIMPL_H
