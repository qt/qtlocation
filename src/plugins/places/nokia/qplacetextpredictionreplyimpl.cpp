#include "qplacetextpredictionreplyimpl.h"

#include <QHash>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceTextPreditionReplyImpl::QPlaceTextPreditionReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceTextPredictionReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonTextPredictionParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &)),
                parser, SLOT(processData(const QString &)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonTextPredictionParser::Error,QString)),
                this, SLOT(predictionsReady(QPlaceJSonTextPredictionParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceTextPreditionReplyImpl::~QPlaceTextPreditionReplyImpl()
{
}

void QPlaceTextPreditionReplyImpl::abort()
{
    if (restReply)
        restReply->cancelProcessing();
}

void QPlaceTextPreditionReplyImpl::restError(QPlaceRestReply::Error errorId)
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

void QPlaceTextPreditionReplyImpl::predictionsReady(const QPlaceJSonTextPredictionParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonTextPredictionParser::NoError) {
        setTextPredictions(parser->predictions());
    } else if (errorId == QPlaceJSonTextPredictionParser::ParsingError) {
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
