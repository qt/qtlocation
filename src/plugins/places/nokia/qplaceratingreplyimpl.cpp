#include "qplaceratingreplyimpl.h"

#include <QHash>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceRatingReplyImpl::QPlaceRatingReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceReply(parent),
    restReply(reply)
{
    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &)),
                this, SLOT(restFinished(const QString &)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
    }
}

/*!
    Destructor.
*/
QPlaceRatingReplyImpl::~QPlaceRatingReplyImpl()
{
}

void QPlaceRatingReplyImpl::abort()
{
    if (restReply)
        restReply->cancelProcessing();
}

void QPlaceRatingReplyImpl::restError(QPlaceRestReply::Error errorId)
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

void QPlaceRatingReplyImpl::restFinished(const QString &data)
{
    Q_UNUSED(data);
    setFinished(true);
    emit finished();
    emit processingFinished(this);
    restReply->deleteLater();
    restReply = NULL;
}
