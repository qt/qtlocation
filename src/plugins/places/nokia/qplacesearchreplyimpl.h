#ifndef QPLACESEARCHREPLYIMPL_H
#define QPLACESEARCHREPLYIMPL_H

#include <QObject>
#include <QHash>

#include <qplacesearchreply.h>
#include "qplacerestreply.h"
#include "qplacejsonsearchparser.h"

QTM_BEGIN_NAMESPACE

class QPlaceSearchReplyImpl : public QPlaceSearchReply
{
    Q_OBJECT
public:
    explicit QPlaceSearchReplyImpl(QPlaceRestReply *reply, QObject *parent = 0);
    ~QPlaceSearchReplyImpl();
    void abort();

Q_SIGNALS:
    void processingFinished(QPlaceReply *reply);
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);

private slots:
    void restError(QPlaceRestReply::Error error);
    void predictionsReady(const QPlaceJSonSearchParser::Error &error,
                          const QString &errorMessage);

private:
    QList<QPlaceSearchResult> filterSecondSearchCenter(const QList<QPlaceSearchResult> &list);
    QPlaceRestReply *restReply;
    QPlaceJSonSearchParser *parser;
};

QTM_END_NAMESPACE

#endif // QPLACESEARCHREPLYIMPL_H
