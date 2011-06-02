#include "qplacereviewreplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

using namespace QT_PLACES_NAMESPACE;

/*!
    Constructor.
*/
QPlaceReviewReplyImpl::QPlaceReviewReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceReviewReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonReviewParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &reply)),
                parser, SLOT(processData(const QString &data)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonReviewParser::Error,QString)),
                this, SLOT(predictionsReady(QPlaceJSonReviewParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceReviewReplyImpl::~QPlaceReviewReplyImpl()
{
}

void QPlaceReviewReplyImpl::abort()
{
    restReply->cancelProcessing();
}

void QPlaceReviewReplyImpl::restError(QPlaceRestReply::Error errorId)
{
    if (errorId == QPlaceRestReply::Canceled) {
        this->setError(CancelError, "ReauestCanceled");
    } else if (errorId == QPlaceRestReply::NetworkError) {
        this->setError(CommunicationError, "Network error");
    }
    emit error(this->error(), this->errorString());
    emit processingError(this, this->error(), this->errorString());
    emit finished();
    emit processingFinished(this);
}

void QPlaceReviewReplyImpl::predictionsReady(const QPlaceJSonReviewParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonReviewParser::NoError) {
        setReviews(parser->results());
        setTotalCount(parser->allReviewsCount());
    } else if (errorId == QPlaceJSonReviewParser::ParsingError) {
        setError(ParseError, errorMessage);
        emit error(this->error(), this->errorString());
        emit processingError(this, ParseError, errorMessage);
    }
    emit finished();
    emit processingFinished(this);
    delete parser;
    parser = NULL;
    restReply->deleteLater();
    restReply = NULL;
}
