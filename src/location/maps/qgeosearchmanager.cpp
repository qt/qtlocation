/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeosearchmanager.h"
#include "qgeosearchmanager_p.h"
#include "qgeosearchmanagerengine.h"

#include "qlandmarkmanager.h"

#include "qgeoboundingbox.h"
#include "qgeoboundingcircle.h"

#include "qlandmarkboxfilter.h"
#include "qlandmarkfetchrequest.h"
#include "qlandmarkintersectionfilter.h"
#include "qlandmarknamefilter.h"
#include "qlandmarkproximityfilter.h"

#include <QLocale>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoSearchManager

    \brief The QGeoSearchManager class provides support for searching
    operations related to geographic information.


    \inmodule QtLocation
    \since 1.1

    \ingroup maps-places

    The geocode(), reverseGeocode() and search() functions return
    QGeoSearchReply objects, which manage these operations and report on the
    result of the operations and any errors which may have occurred.

    The geocode() and reverseGeocode() functions can be used to convert
    QGeoAddress instances to QGeoCoordinate instances and vice-versa.

    The search() function allows a user to perform a free text search
    for place information.  If the string provided can be interpreted as
    an address it can be geocoded to coordinate information, and the string
    can also be used to search a landmarks database, depending on the level
    of support supplied by the service provider.

    The defaultLandmarkManager() function will return a QLandmarkManager
    instance if access to the service providers landmark database is
    available outside of the search() method.

    A user can supply other QLandmarkManager instances to be searched during
    the execution of search() with setAdditionalLandmarkManagers(). This
    means that a personal database store can be combined with a public source
    of database information with very little effort.

    \note At present the additional landmark managers only search for the
    search string in the name of the landmarks.

    Instances of QGeoSearchManager can be accessed with
    QGeoServiceProvider::searchManager().

    A small example of the usage of QGeoSearchManager and the handling of
    QLandmark results follows:

    \code
class MySearchHandler : public QObject
{
    Q_OBJECT
public:
    MySearchHandler(QGeoSearchManager *searchManager, QString searchString)
    {
        QGeoSearchReply *reply = searchManager->search(searchString);

        if (reply->isFinished()) {
            if (reply->error() == QGeoSearchReply::NoError) {
                searchResults(reply);
            else
                searchError(reply, reply->error(), reply->errorString());
            return;
        }

        connect(searchManager,
                SIGNAL(finished(QGeoSearchReply*)),
                this,
                SLOT(searchResults(QGeoSearchReply*)));

        connect(searchManager,
                SIGNAL(error(QGeoSearchReply*,QGeoSearchReply::Error,QString)),
                this
                SLOT(searchError(QGeoSearchReply*,QGeoSearchReply::Error,QString)));
    }

private slots:
    void searchResults(QGeoSearchReply *reply)
    {
        // The QLandmark results can be created from the simpler
        // QGeoPlace results if that is required.
        QList<QLandmark> landmarks;
        for (int i = 0; i < reply->places().size(); ++i) {
            if (reply->places().at(i).isLandmark())
                landmarks.append(QLandmark(reply->places().at(i)));
        }

        // ... now we have to make use of the places and landmarks ...
        reply->deleteLater();
    }

    void searchError(QGeoSearchReply *reply, QGeoSearchReply::Error error, const QString &errorString)
    {
        // ... inform the user that an error has occurred ...
        reply->deleteLater();
    }
};
    \endcode
*/

/*!
\enum QGeoSearchManager::SearchType

Describes the type of search that should be performed by search().

\value SearchNone
    Do not use the search string.
\value SearchGeocode
    Use the search string as a textual address in a geocoding operation.
\value SearchLandmarks
    Use the search string for free-text search against the available landmark
    information sources.
\value SearchAll
    All available information sources should be used as part of the search.
*/

/*!
    Constructs a new manager with the specified \a parent and with the
    implementation provided by \a engine.

    This constructor is used interally by QGeoServiceProviderFactory. Regular
    users should acquire instances of QGeoSearchManager with
    QGeoServiceProvider::searchManager();
*/
QGeoSearchManager::QGeoSearchManager(QGeoSearchManagerEngine *engine, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoSearchManagerPrivate())
{
    d_ptr->engine = engine;
    if (d_ptr->engine) {
        d_ptr->engine->setParent(this);

        connect(d_ptr->engine,
                SIGNAL(finished(QGeoSearchReply*)),
                this,
                SIGNAL(finished(QGeoSearchReply*)));

        connect(d_ptr->engine,
                SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString)),
                this,
                SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString)));
    } else {
        qFatal("The search manager engine that was set for this search manager was NULL.");
    }
}

/*!
    Destroys this manager.
*/
QGeoSearchManager::~QGeoSearchManager()
{
    delete d_ptr;
}

/*!
    Returns the name of the engine which implements the behaviour of this
    search manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
QString QGeoSearchManager::managerName() const
{
//    if (!d_ptr->engine)
//        return QString();

    return d_ptr->engine->managerName();
}

/*!
    Returns the version of the engine which implements the behaviour of this
    search manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
int QGeoSearchManager::managerVersion() const
{
//    if (!d_ptr->engine)
//        return -1;

    return d_ptr->engine->managerVersion();
}

/*!
    Begins the geocoding of \a address. Geocoding is the process of finding a
    coordinate that corresponds to a given address.

    A QGeoSearchReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This manager and the returned QGeoSearchReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false an
    QGeoSearchReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoSearchReply::places() can be used to
    retrieve the results, which will consist of a list of QGeoPlace objects.
    These object represent a combination of coordinate and address data.

    The address data returned in the results may be different from \a address.
    This will usually occur if the geocoding service backend uses a different
    canonical form of addresses or if \a address was only partially filled out.

    If \a bounds is non-null and valid QGeoBoundingArea it will be used to
    limit the results to thos that are contained within \a bounds. This is
    particularly useful if \a address is only partially filled out, as the
    service will attempt to geocode all matches for the specified data.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoSearchManager::finished(),
    QGeoSearchManager::error(), QGeoSearchReply::finished() or
    QGeoSearchReply::error() with deleteLater().
*/
QGeoSearchReply* QGeoSearchManager::geocode(const QGeoAddress &address, QGeoBoundingArea *bounds)
{
//    if (!d_ptr->engine)
//        return new QGeoSearchReply(QGeoSearchReply::EngineNotSetError, "The search manager was not created with a valid engine.", this);

    return d_ptr->engine->geocode(address, bounds);
}


/*!
    Begins the reverse geocoding of \a coordinate. Reverse geocoding is the
    process of finding an address that corresponds to a given coordinate.

    A QGeoSearchReply object will be returned, which can be used to manage the
    reverse geocoding operation and to return the results of the operation.

    This manager and the returned QGeoSearchReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsReverseGeocoding() returns false an
    QGeoSearchReply::UnsupportedOptionError will occur.

    At that point QGeoSearchReply::places() can be used to retrieve the
    results, which will consist of a list of QGeoPlace objects. These object
    represent a combination of coordinate and address data.

    The coordinate data returned in the results may be different from \a
    coordinate. This will usually occur if the reverse geocoding service
    backend shifts the coordinates to be closer to the matching addresses, or
    if the backend returns results at multiple levels of detail.

    If multiple results are returned by the reverse geocoding service backend
    they will be provided in order of specificity. This normally occurs if the
    backend is configured to reverse geocode across multiple levels of detail.
    As an example, some services will return address and coordinate pairs for
    the street address, the city, the state and the country.

    If \a bounds is non-null and a valid QGeoBoundingBox it will be used to
    limit the results to thos that are contained within \a bounds.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoSearchManager::finished(),
    QGeoSearchManager::error(), QGeoSearchReply::finished() or
    QGeoSearchReply::error() with deleteLater().
*/
QGeoSearchReply* QGeoSearchManager::reverseGeocode(const QGeoCoordinate &coordinate, QGeoBoundingArea *bounds)
{
//    if (!d_ptr->engine)
//        return new QGeoSearchReply(QGeoSearchReply::EngineNotSetError, "The search manager was not created with a valid engine.", this);

    return d_ptr->engine->reverseGeocode(coordinate, bounds);
}

/*!
    Begins searching for a place matching \a searchString.  The value of
    \a searchTypes will determine whether the search is for addresses only,
    for landmarks only or for both.

    A QGeoSearchReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This manager and the returned QGeoSearchReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false and \a searchTypes is
    QGeoSearchManager::SearchGeocode an
    QGeoSearchReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoSearchReply::places() can be used to
    retrieve the results, which will consist of a list of QGeoPlace objects.
    These object represent a combination of coordinate and address data.

    If any of the QGeoPlace instances in the results have landmark associated
    data, QGeoPlace::isLandmark() will return true and
    QLandmark::QLandmark(const QGeoPlace &place) can be used to convert the
    QGeoPlace instance into a QLandmark instance.

    If \a searchTypes is QGeoSearchManager::SearchLandmarks or
    QGeoSearchManager::SearchAll, a free text landmark search will be
    performed. The results will be a combination of the backend specific
    landmark search and the same free text search applied to each of the
    QLandmarkManager instances in additionalLandmarkManagers().

    \note At present the additional landmark managers only search for the
    search string in the name of the landmarks.

    If \a limit is -1 the entire result set will be returned, otherwise at most
    \a limit results will be returned.

    The \a offset parameter is used to ask the search service to not return the
    first \a offset results.

    The \a limit and \a offset results are used together to implement paging.

    If additional landmark managers have been setup the number of results
    returned will be at most (1 + number of additional landmark managers) *
    \a limit.  This happens because the results are requested from all sources, combined, and returned once
    all sources have responded.

    If \a bounds is non-null and a valid QGeoBoundingArea it will be used to
    limit the results to thos that are contained within \a bounds.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoSearchManager::finished(),
    QGeoSearchManager::error(), QGeoSearchReply::finished() or
    QGeoSearchReply::error() with deleteLater().
*/
QGeoSearchReply* QGeoSearchManager::search(const QString &searchString,
        QGeoSearchManager::SearchTypes searchTypes,
        int limit,
        int offset,
        QGeoBoundingArea *bounds)
{
//    if (!d_ptr->engine)
//        return new QGeoSearchReply(QGeoSearchReply::EngineNotSetError, "The search manager was not created with a valid engine.", this);

    QGeoSearchReply *reply = d_ptr->engine->search(searchString,
                             searchTypes,
                             limit,
                             offset,
                             bounds);

    if ((d_ptr->engine->additionalLandmarkManagers().size() == 0)
            || (searchTypes == QGeoSearchManager::SearchNone)
            || (searchTypes == QGeoSearchManager::SearchGeocode))
        return reply;

    // TODO add default LM to this list and change doc?

    QList<QLandmarkFetchRequest*> fetchRequests;


    // TODO replace with free text search filter when it becomes available
    QLandmarkNameFilter searchFilter;
    searchFilter.setName(searchString);
    searchFilter.setMatchFlags(QLandmarkFilter::MatchContains);

    QLandmarkIntersectionFilter intersectFilter;
    intersectFilter.append(searchFilter);
    if (bounds) {
        QGeoBoundingBox* box = 0;
        QGeoBoundingCircle* circle = 0;
        switch (bounds->type()) {
            case QGeoBoundingArea::BoxType:
                box = static_cast<QGeoBoundingBox*>(bounds);
                if (box->isValid() && !box->isEmpty()) {
                    QLandmarkBoxFilter boxFilter;
                    boxFilter.setBoundingBox(*box);
                    intersectFilter.append(boxFilter);
                }
                break;
            case QGeoBoundingArea::CircleType:
                circle = static_cast<QGeoBoundingCircle*>(bounds);
                if (circle->isValid() && !circle->isEmpty()) {
                    QLandmarkProximityFilter proximityFilter(circle->center(),
                            circle->radius());
                    intersectFilter.append(proximityFilter);
                }
                break;
            default:
                break;
        }
    }

    for (int i = 0; i < d_ptr->engine->additionalLandmarkManagers().size(); ++i) {
        QLandmarkManager* lm = d_ptr->engine->additionalLandmarkManagers().at(i);

        QLandmarkFetchRequest* fetchRequest = new QLandmarkFetchRequest(lm, this);
        fetchRequest->setFilter(intersectFilter);
        fetchRequest->setLimit(limit);
        fetchRequest->setOffset(offset);
        fetchRequests.append(fetchRequest);
    }

    return new QGeoCombiningSearchReply(reply, fetchRequests);
}

/*!
    Returns whether this manager supports geocoding.
*/
bool QGeoSearchManager::supportsGeocoding() const
{
//    if (!d_ptr->engine)
//        return false;

    return d_ptr->engine->supportsGeocoding();
}

/*!
    Returns whether this manager supports reverse geocoding.
*/
bool QGeoSearchManager::supportsReverseGeocoding() const
{
    return d_ptr->engine->supportsReverseGeocoding();
}

/*!
    Returns the search types supported by the search() function with this manager.
*/
QGeoSearchManager::SearchTypes QGeoSearchManager::supportedSearchTypes() const
{
//    if (!d_ptr->engine)
//        return QGeoSearchManager::SearchTypes();

    return d_ptr->engine->supportedSearchTypes();
}

/*!
    Returns the landmark manager provided by the service provider for
    use with search().

    Will return 0 if the no landmark manager is associated with the service
    provider. This does not indicate that search() does not support
    landmark searching, only that any landmark searching which occurs within in
    search() is done without the use of a QLandmarkManager.
*/
QLandmarkManager* QGeoSearchManager::defaultLandmarkManager() const
{
//    if (!d_ptr->engine)
//        return 0;

    return d_ptr->engine->defaultLandmarkManager();
}

/*!
    Sets the landmark managers to be used with search() to \a landmarkManagers.

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
void QGeoSearchManager::setAdditionalLandmarkManagers(const QList<QLandmarkManager *> &landmarkManagers)
{
//    if (d_ptr->engine)
    d_ptr->engine->setAdditionalLandmarkManagers(landmarkManagers);
}

/*!
    Returns the landmark managers that will be used with search().

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
QList<QLandmarkManager *> QGeoSearchManager::additionalLandmarkManagers() const
{
//    if (!d_ptr->engine)
//        return QList<QLandmarkManager *>();

    return d_ptr->engine->additionalLandmarkManagers();
}

/*!
    Adds \a landmarkManager to the list of landmark managers that will be used
    with search().

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
void QGeoSearchManager::addAdditionalLandmarkManager(QLandmarkManager *landmarkManager)
{
//    if (d_ptr->engine && landmarkManager)
    if (landmarkManager)
        d_ptr->engine->addAdditionalLandmarkManager(landmarkManager);
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this search manager supports returning the results
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeoSearchManager::setLocale(const QLocale &locale)
{
    d_ptr->engine->setLocale(locale);
}

/*!
    Returns the locale used to hint to this search manager about what
    language to use for the results.
*/
QLocale QGeoSearchManager::locale() const
{
    return d_ptr->engine->locale();
}

/*!
\fn void QGeoSearchManager::finished(QGeoSearchReply* reply)

    This signal is emitted when \a reply has finished processing.

    If reply::error() equals QGeoSearchReply::NoError then the processing
    finished successfully.

    This signal and QGeoSearchReply::finished() will be emitted at the same
    time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*!
\fn void QGeoSearchManager::error(QGeoSearchReply* reply, QGeoSearchReply::Error error, QString errorString)

    This signal is emitted when an error has been detected in the processing of
    \a reply. The QGeoSearchManager::finished() signal will probably follow.

    The error will be described by the error code \a error. If \a errorString is
    not empty it will contain a textual description of the error.

    This signal and QGeoSearchReply::error() will be emitted at the same time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoSearchManagerPrivate::QGeoSearchManagerPrivate()
    : engine(0) {}

QGeoSearchManagerPrivate::~QGeoSearchManagerPrivate()
{
    if (engine)
        delete engine;
}

/*******************************************************************************
*******************************************************************************/


QGeoCombiningSearchReply::QGeoCombiningSearchReply(QGeoSearchReply* searchReply,
        QList<QLandmarkFetchRequest*> fetchRequests,
        QObject *parent)
    : QGeoSearchReply(parent),
      searchReply(searchReply),
      fetchRequests(fetchRequests)
{
    connect(searchReply,
            SIGNAL(finished()),
            this,
            SLOT(searchReplyFinished()));

    for (int i = 0; i < fetchRequests.size(); ++i)
        connect(fetchRequests[i],
                SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,
                SLOT(landmarkFetchStateChanged(QLandmarkAbstractRequest::State)));
}

QGeoCombiningSearchReply::~QGeoCombiningSearchReply()
{
    if (searchReply)
        delete searchReply;

    qDeleteAll(fetchRequests);
}

void QGeoCombiningSearchReply::searchReplyFinished()
{
    if (searchReply->error() == QGeoSearchReply::NoError) {
        QList<QGeoPlace> searchReplyPlaces = searchReply->places();
        for (int i = 0; i < searchReplyPlaces.size(); ++i)
            addPlace(searchReplyPlaces.at(i));

        if (fetchRequests.size() == 0)
            emit finished();
    } else {
        emit error(error(), errorString());
        qDeleteAll(fetchRequests);
        fetchRequests.clear();
    }

    delete searchReply;
    searchReply = 0;
}

void QGeoCombiningSearchReply::landmarkFetchStateChanged(QLandmarkAbstractRequest::State newState)
{
    if (newState == QLandmarkAbstractRequest::FinishedState) {
        QLandmarkFetchRequest *req = qobject_cast<QLandmarkFetchRequest*>(sender());
        if (req->error() == QLandmarkManager::NoError) {

            QList<QLandmark> landmarks = req->landmarks();
            for (int i = 0; i < landmarks.size(); ++i)
                addPlace(landmarks.at(i));

            fetchRequests.removeAll(req);
            delete req;

            if (!searchReply && (fetchRequests.size() == 0))
                emit finished();
        } else {

            emit error(QGeoSearchReply::CombinationError, req->errorString());

            delete searchReply;
            searchReply = 0;
            qDeleteAll(fetchRequests);
            fetchRequests.clear();
        }
    }
}

/*******************************************************************************
*******************************************************************************/

#include "moc_qgeosearchmanager.cpp"
#include "moc_qgeosearchmanager_p.cpp"

QTM_END_NAMESPACE
