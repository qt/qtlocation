#include "qdeclarativeplacemanager_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass PlaceManager QDeclarativePlaceManager
    \brief The PlaceManager element provides optionts to work with place content.
    \ingroup qml-places

    PlaceManager provides a access to additional options of place. User might request
    rich content, media, reviews. It is also possible to rate place.

    To use PlaceManager user need to create it in qml file.
    \code
    import places 1.0

    PlaceManager {
        id: placeManager
    }

    ...
    // Starts place details request
    placeManager.getPlaceDetails(place)
    ...

    // Starts place rating request
    placeManager.ratePlace(place, rate_value)

    \endcode

    \sa {QPlaceManager}
*/
QDeclarativePlaceManager::QDeclarativePlaceManager(QObject *parent) :
    QObject(parent),
    m_place(NULL),
    m_manager(NULL),
    m_response(NULL)
{
}

QDeclarativePlaceManager::~QDeclarativePlaceManager()
{
}

/*!
    \qmlsignal PlaceManager::requestFinished(const int &error)

    This handler is called when the request processing is finished.
    0 means that no error occurs during processing and new data were
    added to \l Place object.
*/

/*!
    \qmlproperty Place PlaceManager::place

    This element holds current \l Place object for which data are requested.
    If no object returns null object.
*/
QDeclarativePlace *QDeclarativePlaceManager::place()
{
    return m_place;
}

/*!
    \qmlmethod PlaceManager::getPlaceDetails()
    Gets additional data for place object. When data will be available
    user object will be updated.
*/
void QDeclarativePlaceManager::getPlaceDetails(QDeclarativePlace *place)
{
    if (!m_manager) {
        m_manager = new QPlaceManager(this);
    }
    cancelPreviousRequest();
    connectNewResponse(m_manager->getPlaceDetails(place->placeId()));
    if (m_place != place) {
        m_place = place;
        emit placeChanged();
    }
}

/*!
    \qmlmethod PlaceManager::getPlaceDetails()
    Gets additional data for place object. When media list will be available
    user object will be updated.
*/
void QDeclarativePlaceManager::getPlaceMedia(QDeclarativePlace *place)
{
    getPlaceMedia(place, 0, 0);
}

/*!
    \qmlmethod PlaceManager::getPlaceDetails()
    Gets additional data for place object. When media list will be available
    user object will be updated.
*/
void QDeclarativePlaceManager::getPlaceMedia(QDeclarativePlace *place, int offset, int limit)
{
    if (!m_manager) {
        m_manager = new QPlaceManager(this);
    }
    cancelPreviousRequest();
    QPlaceQuery query;
    query.setLimit(limit);
    query.setOffset(offset);
    connectNewResponse(m_manager->getMedia(place->place(), query));
    if (m_place != place) {
        m_place = place;
        emit placeChanged();
    }
}

/*!
    \qmlmethod PlaceManager::getPlaceDetails()
    Gets additional data for place object. When reviews list will be available
    user object will be updated.
*/
void QDeclarativePlaceManager::getPlaceReviews(QDeclarativePlace *place)
{
    getPlaceReviews(place, 0, 0);
}

/*!
    \qmlmethod PlaceManager::getPlaceDetails()
    Gets additional data for place object. When reviews list will be available
    user object will be updated.
*/
void QDeclarativePlaceManager::getPlaceReviews(QDeclarativePlace *place, int offset, int limit)
{
    if (!m_manager) {
        m_manager = new QPlaceManager(this);
    }
    cancelPreviousRequest();
    QPlaceQuery query;
    query.setLimit(limit);
    query.setOffset(offset);
    connectNewResponse(m_manager->getReviews(place->place(), query));
    if (m_place != place) {
        m_place = place;
        emit placeChanged();
    }
}


/*!
    \qmlmethod PlaceManager::ratePlace()
    Gets additional data for place object. When data will be available
    user object will be updated.
*/
void QDeclarativePlaceManager::ratePlace(QDeclarativePlace *place, qreal rateValue, const QString &userName)
{
    if (!m_manager) {
        m_manager = new QPlaceManager(this);
    }
    cancelPreviousRequest();
    //TODO: handling of user name when rating a place
    connectNewResponse(m_manager->postRating(place->place(), rateValue));
    if (m_place != place) {
        m_place = place;
        emit placeChanged();
    }
}

/*!
    \qmlmethod PlaceManager::cancelRequest()
    Cancels ongoing request.
*/
void QDeclarativePlaceManager::cancelRequest()
{
    cancelPreviousRequest();
}

void QDeclarativePlaceManager::replyFinished()
{
    if (!m_response) {
        return;
    }
    if (m_response->type() == QPlaceReply::PlaceDetailsReply) {
        QPlaceDetailsReply *reply = qobject_cast<QPlaceDetailsReply*>(m_response);
        m_place->setPlace(reply->result());
    } else if (m_response->type() == QPlaceReply::MediaReply) {
        QPlaceMediaReply *reply = qobject_cast<QPlaceMediaReply*>(m_response);
        QDeclarativeMediaPaginationList *newList = new QDeclarativeMediaPaginationList(reply->mediaObjects());
        m_place->setMedia(newList);
        delete newList;
        m_place->setMediaCount(reply->totalCount());
    } else if (m_response->type() == QPlaceReply::ReviewReply) {
        QPlaceReviewReply *reply = qobject_cast<QPlaceReviewReply*>(m_response);
        QDeclarativeReviewPaginationList *newList = new QDeclarativeReviewPaginationList(reply->reviews());
        m_place->setReviews(newList);
        delete newList;
        m_place->setReviewCount(reply->totalCount());
    }
    m_response->deleteLater();
    m_response = NULL;
    emit requestFinished(0);
}

void QDeclarativePlaceManager::replyError(QPlaceReply::Error error,
                                                 const QString &errorString)
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    emit requestFinished(-1);
}

void QDeclarativePlaceManager::cancelPreviousRequest()
{
    if (m_response) {
        if (!m_response->isFinished()) {
            m_response->abort();
        }
        m_response->deleteLater();
        m_response = NULL;
    }
}

void QDeclarativePlaceManager::connectNewResponse(QPlaceReply *newResponse)
{
    if (newResponse) {
        m_response = newResponse;
        m_response->setParent(this);
        connect(m_response, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(m_response, SIGNAL(error(QPlaceReply::Error,QString)),
                this, SLOT(replyError(QPlaceReply::Error,QString)));
    } else {
        emit requestFinished(-1);
    }
}
