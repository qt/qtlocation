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

#include "qgeosearchmanagerengine.h"
#include "qgeosearchmanagerengine_p.h"

#include "qgeoaddress.h"
#include "qgeocoordinate.h"
#include "qlandmarkmanager.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoSearchManagerEngine

    \brief The QGeoSearchManagerEngine class provides an interface and
    convenience methods to implementers of QGeoServiceProvider plugins who want
    to provide support for searching operations related to geographic data.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps-impl

    In the default implementation, supportsGeocoding() and supportsReverseGeocoding() returns false while
    geocode() and reverseGeocode()
    cause QGeoSearchReply::UnsupportedOptionError to occur.

    If the service provider supports geocoding the subclass should provide an
    implementation of geocode() and call setSupportsGeocoding(true) at
    some point in time before geoocode() is called.

    Similarly, if the service provider supports reverse geocoding the subclass
    should provide an implementation reverseGeocode() and call
    setSupportsReverseGeocoding(true) at some point in time before
    reverseGeoocode() is called.

    The search() function will make use of the QLandmarkManager instances
    returned by additionalLandmarkManagers(). If a QLandmarkManager is used
    internally to query the service providers landmark data the
    QLandmarkManager can be made available to the users with
    setDefaultLandmarkManager().

    The subclass should call setSupportedSearchTypes() at some point in time
    before search() is called.

    If the service supports searching for places the subclass should provide
    an implementetation of search() and call setSupportedSearchTypes() at
    some point in time before search() is called.

    A subclass of QGeoSearchManagerEngine will often make use of a subclass
    fo QGeoSearchReply internally, in order to add any engine-specific
    data (such as a QNetworkReply object for network-based services) to the
    QGeoSearchReply instances used by the engine.

    \sa QGeoSearchManager
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters
    to pass any implementation specific data to the engine.
*/
QGeoSearchManagerEngine::QGeoSearchManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoSearchManagerEnginePrivate())
{
    Q_UNUSED(parameters)
}

/*!
    Destroys this engine.
*/
QGeoSearchManagerEngine::~QGeoSearchManagerEngine()
{
    delete d_ptr;
}

/*!
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoSearchManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
QString QGeoSearchManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoSearchManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
int QGeoSearchManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

/*!
    Begins the geocoding of \a address. Geocoding is the process of finding a
    coordinate that corresponds to a given address.

    A QGeoSearchReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This engine and the returned QGeoSearchReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false an
    QGeoSearchReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoSearchReply::places() can be used to
    retrieve the results, which will consist of a list of QGeoPlace objects.
    These object represent a combination of coordinate and address data.

    The address data returned in the results may be different from \a address.
    This will usually occur if the geocoding service backend uses a different
    canonical form of addresses or if \a address was only partially filled out.

    If \a bounds is non-null and a valid QGeoBoundingArea it will be used to
    limit the results to those that are contained by \a bounds. This is
    particularly useful if \a address is only partially filled out, as the
    service will attempt to geocode all matches for the specified data.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoSearchManagerEngine::finished(),
    QGeoSearchManagerEngine::error(), QGeoSearchReply::finished() or
    QGeoSearchReply::error() with deleteLater().
*/
QGeoSearchReply* QGeoSearchManagerEngine::geocode(const QGeoAddress &address,
        QGeoBoundingArea *bounds)
{
    Q_UNUSED(address)
    Q_UNUSED(bounds)
    return new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError,
                               "Geocoding is not supported by this service provider.", this);
}

/*!
    Begins the reverse geocoding of \a coordinate. Reverse geocoding is the
    process of finding an address that corresponds to a given coordinate.

    A QGeoSearchReply object will be returned, which can be used to manage the
    reverse geocoding operation and to return the results of the operation.

    This engine and the returned QGeoSearchReply object will emit signals
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

    If \a bounds is non-null and a valid QGeoBoundingArea it will be used to
    limit the results to those that are contained by \a bounds.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoSearchManagerEngine::finished(),
    QGeoSearchManagerEngine::error(), QGeoSearchReply::finished() or
    QGeoSearchReply::error() with deleteLater().
*/
QGeoSearchReply* QGeoSearchManagerEngine::reverseGeocode(const QGeoCoordinate &coordinate,
        QGeoBoundingArea *bounds)
{
    Q_UNUSED(coordinate)
    Q_UNUSED(bounds)
    return new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError,
                               "Reverse geocoding is not supported by this service provider.", this);
}

/*!
    Begins searching for a place matching \a searchString.  The value of
    \a searchTypes will determine whether the search is for addresses only,
    for landmarks only or for both.

    A QGeoSearchReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This engine and the returned QGeoSearchReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false and \a searchTypes is
    QGeoSearchManagerEngine::SearchGeocode an
    QGeoSearchReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoSearchReply::places() can be used to
    retrieve the results, which will consist of a list of QGeoPlace objects.
    These object represent a combination of coordinate and address data.

    If any of the QGeoPlace instances in the results have landmark associated
    data, QGeoPlace::isLandmark() will return true and
    QLandmark::QLandmark(const QGeoPlace &place) can be used to convert the
    QGeoPlace instance into a QLandmark instance.

    If \a searchTypes is QGeoSearchManagerEngine::SearchLandmarks or
    QGeoSearchManagerEngine::SearchAll, a free text landmark search will be
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
    limit the results to those that are contained by \a bounds.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoSearchManagerEngine::finished(),
    QGeoSearchManagerEngine::error(), QGeoSearchReply::finished() or
    QGeoSearchReply::error() with deleteLater().
*/
QGeoSearchReply* QGeoSearchManagerEngine::search(const QString &searchString,
        QGeoSearchManager::SearchTypes searchTypes,
        int limit,
        int offset,
        QGeoBoundingArea *bounds)
{
    Q_UNUSED(searchString)
    Q_UNUSED(searchTypes)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    Q_UNUSED(bounds)

    return new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError,
                               "Searching is not supported by this service provider.", this);
}

/*!
    Sets whether geocoding is supported by this engine to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support geocoding.
*/
void QGeoSearchManagerEngine::setSupportsGeocoding(bool supported)
{
    d_ptr->supportsGeocoding = supported;
}

/*!
    Returns whether this engine supports geocoding.
*/
bool QGeoSearchManagerEngine::supportsGeocoding() const
{
    return d_ptr->supportsGeocoding;
}

/*!
    Sets whether reverse geocoding is supported by this engine to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support reverse geocoding.
*/
void QGeoSearchManagerEngine::setSupportsReverseGeocoding(bool supported)
{
    d_ptr->supportsReverseGeocoding = supported;
}

/*!
    Returns whether this engine supports reverse geocoding.
*/
bool QGeoSearchManagerEngine::supportsReverseGeocoding() const
{
    return d_ptr->supportsReverseGeocoding;
}

/*!
    Sets the search types supported by the search() with this engine to \a searchTypes.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support any search types.
*/
void QGeoSearchManagerEngine::setSupportedSearchTypes(QGeoSearchManager::SearchTypes searchTypes)
{
    d_ptr->supportedSearchTypes = searchTypes;
}

/*!
    Returns the search types supported by the search() with this engine.
*/
QGeoSearchManager::SearchTypes QGeoSearchManagerEngine::supportedSearchTypes() const
{
    return d_ptr->supportedSearchTypes;
}

/*!
    Sets the landmark manager provided by the service provider for
    use with search() to \a landmarkManager.

    This should only be set if search() makes use of a QLandmarkManager
    instance to provide landmark searching functionality.

    It is important that subclasses use this method to ensure that the engine
    is able to carry out landmark searches.  If this function is not used the
    engine will not be able to use or return the default landmark manager.
*/
void QGeoSearchManagerEngine::setDefaultLandmarkManager(QLandmarkManager *landmarkManager)
{
    d_ptr->defaultLandmarkManager = landmarkManager;
}

/*!
    Returns the landmark manager provided by the service provider for
    use with search().

    Will return 0 if the no landmark manager is associated with the service
    provider. This does not indicate that search() does not support
    landmark searching, only that any landmark searching which occurs within in
    search() is done without the use of a QLandmarkManager.
*/
QLandmarkManager* QGeoSearchManagerEngine::defaultLandmarkManager() const
{
    return d_ptr->defaultLandmarkManager;
}

/*!
    Sets the landmark managers to be used with search() to \a landmarkManagers.

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
void QGeoSearchManagerEngine::setAdditionalLandmarkManagers(const QList<QLandmarkManager *> &landmarkManagers)
{
    for (int i = 0; i < landmarkManagers.size(); ++i)
        if (landmarkManagers.at(i))
            d_ptr->additionalLandmarkManagers.append(landmarkManagers.at(i));
}

/*!
    Returns the landmark managers that will be used with search().

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
QList<QLandmarkManager *> QGeoSearchManagerEngine::additionalLandmarkManagers() const
{
    return d_ptr->additionalLandmarkManagers;
}

/*!
    Adds \a landmarkManager to the list of landmark managers that will be used with search().

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
void QGeoSearchManagerEngine::addAdditionalLandmarkManager(QLandmarkManager *landmarkManager)
{
    if (landmarkManager)
        d_ptr->additionalLandmarkManagers.append(landmarkManager);
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this search manager supports returning the results
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeoSearchManagerEngine::setLocale(const QLocale &locale)
{
    d_ptr->locale = locale;
}

/*!
    Returns the locale used to hint to this search manager about what
    language to use for the results.
*/
QLocale QGeoSearchManagerEngine::locale() const
{
    return d_ptr->locale;
}

/*!
\fn void QGeoSearchManagerEngine::finished(QGeoSearchReply* reply)

    This signal is emitted when \a reply has finished processing.

    If reply::error() equals QGeoSearchReply::NoError then the processing
    finished successfully.

    This signal and QGeoSearchReply::finished() will be emitted at the same
    time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*!
\fn void QGeoSearchManagerEngine::error(QGeoSearchReply* reply, QGeoSearchReply::Error error, QString errorString)

    This signal is emitted when an error has been detected in the processing of
    \a reply. The QGeoSearchManagerEngine::finished() signal will probably follow.

    The error will be described by the error code \a error. If \a errorString is
    not empty it will contain a textual description of the error.

    This signal and QGeoSearchReply::error() will be emitted at the same time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoSearchManagerEnginePrivate::QGeoSearchManagerEnginePrivate()
    : managerVersion(-1),
      defaultLandmarkManager(0),
      supportsGeocoding(false),
      supportsReverseGeocoding(false) {}

QGeoSearchManagerEnginePrivate::~QGeoSearchManagerEnginePrivate()
{
    if (defaultLandmarkManager)
        delete defaultLandmarkManager;
    // TODO check for null? or do that in the setter?
    qDeleteAll(additionalLandmarkManagers);
}

#include "moc_qgeosearchmanagerengine.cpp"

QTM_END_NAMESPACE
