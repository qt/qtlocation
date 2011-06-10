#include "qplacerecommendationreplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceRecommendationReplyImpl::QPlaceRecommendationReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceSearchReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonRecommendationParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &)),
                parser, SLOT(processData(const QString &)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonRecommendationParser::Error,QString)),
                this, SLOT(predictionsReady(QPlaceJSonRecommendationParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceRecommendationReplyImpl::~QPlaceRecommendationReplyImpl()
{
}

void QPlaceRecommendationReplyImpl::abort()
{
    if (restReply)
        restReply->cancelProcessing();
}

void QPlaceRecommendationReplyImpl::restError(QPlaceRestReply::Error errorId)
{
    if (errorId == QPlaceRestReply::Canceled) {
        this->setError(CancelError, "ReauestCanceled");
    } else if (errorId == QPlaceRestReply::NetworkError) {
        this->setError(CommunicationError, "Network error");
    }
    emit error(this->error(), this->errorString());
    emit processingError(this, this->error(), this->errorString());
    setFinished(true);
    emit finished();
    emit processingFinished(this);
}

void QPlaceRecommendationReplyImpl::predictionsReady(const QPlaceJSonRecommendationParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonRecommendationParser::NoError) {
        setResults(parser->results());
    } else if (errorId == QPlaceJSonRecommendationParser::ParsingError) {
        setError(ParseError, errorMessage);
        emit error(this->error(), this->errorString());
        emit processingError(this, ParseError, errorMessage);
    }
    setFinished(true);
    emit finished();
    emit processingFinished(this);
    delete parser;
    parser = NULL;
    restReply->deleteLater();
    restReply = NULL;
}
