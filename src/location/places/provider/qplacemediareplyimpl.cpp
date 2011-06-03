#include "qplacemediareplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceMediaReplyImpl::QPlaceMediaReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceMediaReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonMediaParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &reply)),
                parser, SLOT(processData(const QString &data)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonMediaParser::Error,QString)),
                this, SLOT(predictionsReady(QPlaceJSonMediaParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceMediaReplyImpl::~QPlaceMediaReplyImpl()
{
}

void QPlaceMediaReplyImpl::abort()
{
    restReply->cancelProcessing();
}

void QPlaceMediaReplyImpl::restError(QPlaceRestReply::Error errorId)
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

void QPlaceMediaReplyImpl::predictionsReady(const QPlaceJSonMediaParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonMediaParser::NoError) {
        setMediaObjects(parser->resultMedia());
        setTotalCount(parser->allMediaCount());
    } else if (errorId == QPlaceJSonMediaParser::ParsingError) {
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
