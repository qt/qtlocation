#ifndef QPLACEREVIEWREPLYIMPL_H
#define QPLACEREVIEWREPLYIMPL_H

#include <QObject>
#include <QHash>

#include "../qplacereviewreply.h"
#include "qplacerestreply.h"
#include "qplacejsonreviewparser.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceReviewReplyImpl : public QPlaceReviewReply
{
    Q_OBJECT
public:
    explicit QPlaceReviewReplyImpl(QPlaceRestReply *reply, QObject *parent = 0);
    ~QPlaceReviewReplyImpl();
    void abort();

Q_SIGNALS:
    void processingFinished(QPlaceReply *reply);
    void processingError(QPlaceReply *reply, const Error &error, const QString &errorMessage);

private slots:
    void restError(QPlaceRestReply::Error error);
    void predictionsReady(const QPlaceJSonReviewParser::Error &error,
                          const QString &errorMessage);

private:
    QPlaceRestReply *restReply;
    QPlaceJSonReviewParser *parser;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEREVIEWREPLYIMPL_H
