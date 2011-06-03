#include "qplacereply.h"

#include "qplace.h"
#include "qplacesearchresult.h"

QTM_USE_NAMESPACE

class QPlaceReplyPrivate
{
public:
    QPlaceReplyPrivate() : isFinished(false) {}
    QPlaceReply::Error error;
    QString errorString;
    bool isFinished;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    \class QPlaceReply

    \inmodule Location

    \brief The QPlaceReply class manages an operation started by
    an instance of QPlaceManager.

    \ingroup places-main
*/

/*!
    \enum QPlaceReply::Error

    Describes an error which prevented completion of an operation.
    \value NoError
        No error has occurred
    \value DoesNotExistError
        An entity does not exist eg an import file.
    \value PlaceDoesNotExistError
        A specified place could not be found
    \value CategoryDoesNotExistError
        A specified category could not be found
    \value CommunicationError
        An error occurred communicating with the service provider.
    \value ParseError
        The response from the service provider or an import file was in an unrecognizable format
    \value PermissionsError
        The operation failed because of insufficient permissions.
    \value UnsupportedError
        The operation was not supported by the service provider.
    \value CancelError
        The operation was canceled.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    \enum QPlaceReply::Type

    Describes the reply's type.
    \value Reply
        This is a generic reply.
    \value TextPredictionReply
        Thi is the reply for a text prediction operation.
    \value ReviewReply
        This is a reply for the retrieval of place reviews.
    \value MediaReply
        This is a reply for the retrieval of place media.
    \value PlaceDetailsReply
        This is a reply for the retrieval of place details.
    \value PlaceSearchReply
        This is a reply for a place search operation.
*/

/*!
    Constructs a reply object with a given \a parent.
*/
QPlaceReply::QPlaceReply(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destructor.
*/
QPlaceReply::~QPlaceReply()
{
    if (!isFinished()) {
        abort();
    }
}

/*!
    Return true if the reply has completed.
*/
bool QPlaceReply::isFinished() const
{
    return d->isFinished;
}

/*!
    Return type of operation.
*/
QPlaceReply::Type QPlaceReply::type() const
{
    return QPlaceReply::Reply;
}

/*!
    Sets the status of whether the reply is \a finished
    or not.  This function does not cause the finished() signal
    to be emitted.
*/
void QPlaceReply::setFinished(bool finished)
{
    d->isFinished = finished;
}

/*!
    Sets the \a error and \a errorString of the reply.
    This function does not cause the error(QPlaceReply::Error, const QString &errorString)
    signal to be emitted.
*/
void QPlaceReply::setError(QPlaceReply::Error error, const QString &errorString)
{
    d->error = error;
    d->errorString = errorString;
}

/*!
    Returns the error string.
*/
QString QPlaceReply::errorString() const
{
    return d->errorString;
}

/*!
    Returns error code.
*/
QPlaceReply::Error QPlaceReply::error() const
{
    return d->error;
}

/*!
    Aborts the operation.
*/
void QPlaceReply::abort()
{
}

/*!
    \fn void QPlaceReply::finished()

    This signal is emitted when this reply has finished processing.

    If error() equals QPlaceReply::NoError then the processing
    finished successfully.

    This signal and QPlaceManager::finished() will be
    emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*!
    \fn void QPlaceReply::error(QPlaceReply::Error error, const QString &errorString)

    This signal is emitted when an error has been detected in the processing of
    this reply. The finished() signal will probably follow.

    The error will be described by the error code \a error. If \a errorString is
    not empty it will contain a textual description of the error meant for
    developers and not end users.

    This signal and QPlaceManager::error() will be emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/
