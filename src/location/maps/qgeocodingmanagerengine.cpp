/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeocodingmanagerengine.h"
#include "qgeocodingmanagerengine_p.h"

#include "qgeoaddress.h"
#include "qgeocoordinate.h"

#include <QtLocation/QGeoShape>

QT_BEGIN_NAMESPACE

/*!
    \class QGeocodingManagerEngine
    \inmodule QtLocation
    \ingroup QtLocation-impl
    \since Qt Location 5.0

    \brief The QGeocodingManagerEngine class provides an interface and
    convenience methods to implementers of QGeoServiceProvider plugins who want
    to provide support for geocoding operations.

    In the default implementation, supportsGeocoding() and supportsReverseGeocoding() returns false while
    geocode() and reverseGeocode()
    cause QGeocodeReply::UnsupportedOptionError to occur.

    If the service provider supports geocoding the subclass should provide an
    implementation of geocode() and call setSupportsGeocoding(true) at
    some point in time before geocode() is called.

    Similarly, if the service provider supports reverse geocoding the subclass
    should provide an implementation reverseGeocode() and call
    setSupportsReverseGeocoding(true) at some point in time before
    reverseGeocode() is called.

    A subclass of QGeocodingManagerEngine will often make use of a subclass
    fo QGeocodeReply internally, in order to add any engine-specific
    data (such as a QNetworkReply object for network-based services) to the
    QGeocodeReply instances used by the engine.

    \sa QGeocodingManager
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters
    to pass any implementation specific data to the engine.
*/
QGeocodingManagerEngine::QGeocodingManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeocodingManagerEnginePrivate())
{
    Q_UNUSED(parameters)
}

/*!
    Destroys this engine.
*/
QGeocodingManagerEngine::~QGeocodingManagerEngine()
{
    delete d_ptr;
}

/*!
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeocodingManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
QString QGeocodingManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeocodingManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
int QGeocodingManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

/*!
    Begins the geocoding of \a address. Geocoding is the process of finding a
    coordinate that corresponds to a given address.

    A QGeocodeReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This engine and the returned QGeocodeReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false an
    QGeocodeReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeocodeReply::locations() can be used to
    retrieve the results, which will consist of a list of QGeoLocation objects.
    These objects represent a combination of coordinate and address data.

    The address data returned in the results may be different from \a address.
    This will usually occur if the geocoding service backend uses a different
    canonical form of addresses or if \a address was only partially filled out.

    If \a bounds is non-null and a valid QGeoShape it will be used to
    limit the results to those that are contained by \a bounds. This is
    particularly useful if \a address is only partially filled out, as the
    service will attempt to geocode all matches for the specified data.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeocodingManagerEngine::finished(),
    QGeocodingManagerEngine::error(), QGeocodeReply::finished() or
    QGeocodeReply::error() with deleteLater().
*/
QGeocodeReply *QGeocodingManagerEngine::geocode(const QGeoAddress &address,
        const QGeoShape &bounds)
{
    Q_UNUSED(address)
    Q_UNUSED(bounds)
    return new QGeocodeReply(QGeocodeReply::UnsupportedOptionError,
                               QLatin1String("Geocoding is not supported by this service provider."), this);
}

/*!
    Begins the reverse geocoding of \a coordinate. Reverse geocoding is the
    process of finding an address that corresponds to a given coordinate.

    A QGeocodeReply object will be returned, which can be used to manage the
    reverse geocoding operation and to return the results of the operation.

    This engine and the returned QGeocodeReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsReverseGeocoding() returns false an
    QGeocodeReply::UnsupportedOptionError will occur.

    At that point QGeocodeReply::locations() can be used to retrieve the
    results, which will consist of a list of QGeoLocation objects. These objects
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

    If \a bounds is non-null and a valid QGeoShape it will be used to
    limit the results to those that are contained by \a bounds.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeocodingManagerEngine::finished(),
    QGeocodingManagerEngine::error(), QGeocodeReply::finished() or
    QGeocodeReply::error() with deleteLater().
*/
QGeocodeReply *QGeocodingManagerEngine::reverseGeocode(const QGeoCoordinate &coordinate,
        const QGeoShape &bounds)
{
    Q_UNUSED(coordinate)
    Q_UNUSED(bounds)
    return new QGeocodeReply(QGeocodeReply::UnsupportedOptionError,
                               QLatin1String("Reverse geocoding is not supported by this service provider."), this);
}

/*!
    Begins geocoding for a location matching \a address.

    A QGeocodeReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This engine and the returned QGeocodeReply object will emit signals
    indicating if the operation completes or if errors occur.

    Once the operation has completed, QGeocodeReply::locations() can be used to
    retrieve the results, which will consist of a list of QGeoLocation objects.
    These objects represent a combination of coordinate and address data.

    If \a limit is -1 the entire result set will be returned, otherwise at most
    \a limit results will be returned.

    The \a offset parameter is used to ask the geocoding service to not return the
    first \a offset results.

    The \a limit and \a offset results are used together to implement paging.

    If \a bounds is non-null and a valid QGeoShape it will be used to
    limit the results to those that are contained by \a bounds.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeocodingManagerEngine::finished(),
    QGeocodingManagerEngine::error(), QGeocodeReply::finished() or
    QGeocodeReply::error() with deleteLater().
*/
QGeocodeReply *QGeocodingManagerEngine::geocode(const QString &address,
        int limit,
        int offset,
        const QGeoShape &bounds)
{
    Q_UNUSED(address)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    Q_UNUSED(bounds)

    return new QGeocodeReply(QGeocodeReply::UnsupportedOptionError,
                               QLatin1String("Searching is not supported by this service provider."), this);
}

/*!
    Sets the locale to be used by this manager to \a locale.

    If this geocoding manager supports returning the results
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeocodingManagerEngine::setLocale(const QLocale &locale)
{
    d_ptr->locale = locale;
}

/*!
    Returns the locale used to hint to this geocoding manager about what
    language to use for the results.
*/
QLocale QGeocodingManagerEngine::locale() const
{
    return d_ptr->locale;
}

/*!
\fn void QGeocodingManagerEngine::finished(QGeocodeReply *reply)

    This signal is emitted when \a reply has finished processing.

    If reply::error() equals QGeocodeReply::NoError then the processing
    finished successfully.

    This signal and QGeocodeReply::finished() will be emitted at the same
    time.

    \note Do not delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*!
\fn void QGeocodingManagerEngine::error(QGeocodeReply *reply, QGeocodeReply::Error error, QString errorString)

    This signal is emitted when an error has been detected in the processing of
    \a reply. The QGeocodingManagerEngine::finished() signal will probably follow.

    The error will be described by the error code \a error. If \a errorString is
    not empty it will contain a textual description of the error.

    This signal and QGeocodeReply::error() will be emitted at the same time.

    \note Do not delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeocodingManagerEnginePrivate::QGeocodingManagerEnginePrivate()
    : managerVersion(-1)
{}

QGeocodingManagerEnginePrivate::~QGeocodingManagerEnginePrivate()
{
}

#include "moc_qgeocodingmanagerengine.cpp"

QT_END_NAMESPACE
