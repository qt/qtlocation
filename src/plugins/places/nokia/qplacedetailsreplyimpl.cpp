#include "qplacedetailsreplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceDetailsReplyImpl::QPlaceDetailsReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceDetailsReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonDetailsParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &reply)),
                parser, SLOT(processData(const QString &data)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonDetailsParser::Error,QString)),
                this, SLOT(predictionsReady(QPlaceJSonDetailsParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceDetailsReplyImpl::~QPlaceDetailsReplyImpl()
{
}

void QPlaceDetailsReplyImpl::abort()
{
    if (restReply)
        restReply->cancelProcessing();
}

void QPlaceDetailsReplyImpl::restError(QPlaceRestReply::Error errorId)
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

void QPlaceDetailsReplyImpl::predictionsReady(const QPlaceJSonDetailsParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonDetailsParser::NoError) {
        setResult(parser->result());
    } else if (errorId == QPlaceJSonDetailsParser::ParsingError) {
        setError(ParseError, errorMessage);
        emit error(this->error(), this->errorString());
        emit processingError(this, ParseError, errorMessage);
    }
    setFinished(true);
    emit finished();
    emit processingFinished(this);
    parser->deleteLater();
    parser = NULL;
    restReply->deleteLater();
    restReply = NULL;
}
