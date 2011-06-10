#ifndef QPLACERECOMMENDATIONREPLYIMPL_H
#define QPLACERECOMMENDATIONREPLYIMPL_H

#include <QObject>
#include <QHash>

#include <qplacesearchreply.h>
#include "qplacerestreply.h"
#include "qplacejsonrecommendationparser.h"

QTM_BEGIN_NAMESPACE

class QPlaceRecommendationReplyImpl : public QPlaceSearchReply
{
    Q_OBJECT
public:
    explicit QPlaceRecommendationReplyImpl(QPlaceRestReply *reply, QObject *parent = 0);
    ~QPlaceRecommendationReplyImpl();
    void abort();

Q_SIGNALS:
    void processingFinished(QPlaceReply *reply);
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);

private slots:
    void restError(QPlaceRestReply::Error error);
    void predictionsReady(const QPlaceJSonRecommendationParser::Error &error,
                          const QString &errorMessage);

private:
    QPlaceRestReply *restReply;
    QPlaceJSonRecommendationParser *parser;
};

QTM_END_NAMESPACE

#endif // QPLACERECOMMENDATIONREPLYIMPL_H
