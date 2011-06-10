#ifndef QPLACEDETAILSREPLYIMPL_H
#define QPLACEDETAILSREPLYIMPL_H

#include <QObject>
#include <QHash>

#include <qplacedetailsreply.h>
#include "qplacerestreply.h"
#include "qplacejsondetailsparser.h"

QTM_BEGIN_NAMESPACE

class QPlaceDetailsReplyImpl : public QPlaceDetailsReply
{
    Q_OBJECT
public:
    explicit QPlaceDetailsReplyImpl(QPlaceRestReply *reply, QObject *parent = 0);
    ~QPlaceDetailsReplyImpl();
    void abort();

Q_SIGNALS:
    void processingFinished(QPlaceReply *reply);
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);

private slots:
    void restError(QPlaceRestReply::Error error);
    void predictionsReady(const QPlaceJSonDetailsParser::Error &error,
                          const QString &errorMessage);

private:
    QPlaceRestReply *restReply;
    QPlaceJSonDetailsParser *parser;
};

QTM_END_NAMESPACE

#endif // QPLACEDETAILSREPLYIMPL_H
