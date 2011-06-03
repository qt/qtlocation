#include "qplacesearchreplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceSearchReplyImpl::QPlaceSearchReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceSearchReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonSearchParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &reply)),
                parser, SLOT(processData(const QString &data)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonSearchParser::Error,QString)),
                this, SLOT(predictionsReady(QPlaceJSonSearchParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceSearchReplyImpl::~QPlaceSearchReplyImpl()
{
}

void QPlaceSearchReplyImpl::abort()
{
    restReply->cancelProcessing();
}

void QPlaceSearchReplyImpl::restError(QPlaceRestReply::Error errorId)
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

void QPlaceSearchReplyImpl::predictionsReady(const QPlaceJSonSearchParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonSearchParser::NoError) {
        setResults(parser->searchResults());
    } else if (errorId == QPlaceJSonSearchParser::ParsingError) {
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
