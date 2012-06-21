/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecoordinate_p.h"
#include <qnumeric.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Coordinate QDeclarativeCoordinate
    \inqmlmodule QtLocation 5.0
    \ingroup qml-QtLocation5-positioning
    \since Qt Location 5.0

    \brief The Coordinate element represents and stores a geographic position.

    Coordinate elements represent a geographic location in the form of numbers:
    in particular, \l latitude, \l longitude and \l altitude. These, together,
    specify a 3-dimensional position anywhere on or near the Earth's surface.

    Coordinates are used by many other elements in the Qt Location module, for
    specifying the position of an object on a Map, the current position of
    a device and many other tasks. They also feature a number of important
    utility methods that make otherwise complex calculations simple to use,
    such as atDistanceAndAzimuth.

    \section2 Accuracy

    The latitude, longitude and altitude numbers stored in a Coordinate are
    represented as doubles, giving them approximately 16 decimal digits of
    precision -- enough to specify micrometers. The calculations performed
    in Coordinate's methods such as azimuthTo and distanceTo also use doubles
    for all intermediate values, but the inherent inaccuracies in their
    spherical Earth model dominate the amount of error in their output.

    \section2 Example Usage

    The following snippet defines two coordinates near Brisbane, Australia.

    \code
    Coordinate {
        id: coord1
        latitude: -27.2
        longitude: 153.1
        altitude: 1.0
    }

    Coordinate {
        id: coord2
        latitude: -27.5
        longitude: 153.2
        altitude: 5.0
    }
    \endcode

    As an example, the value of \c{coord1.distanceTo(coord2)} would now be
    approximately 34790 (34.8 km).

    \note if another element has a Coordinate property, a change notification
    signal for that property will only be emitted if that property is assigned
    a new Coordinate, and \b{not} if the position data of the existing Coordinate
    assigned to the property changes.  The only exception to this rule is that
    the \l{Position} element does emit change signals if its coordinate property's values change.
*/

QDeclarativeCoordinate::QDeclarativeCoordinate(QObject *parent)
        : QObject(parent) {}

QDeclarativeCoordinate::QDeclarativeCoordinate(const QGeoCoordinate &coordinate,
        QObject *parent)
        : QObject(parent),
        m_coordinate(coordinate) {}

QDeclarativeCoordinate::~QDeclarativeCoordinate() {}

/*!
    \qmlproperty QGeoCoordinate Coordinate::coordinate

    For details on how to use this property to interface between C++ and QML see
    "\l {location-cpp-qml.html#geocoordinate} {Interfaces between C++ and QML Code}".
*/
void QDeclarativeCoordinate::setCoordinate(const QGeoCoordinate &coordinate)
{
    bool changed = false;
    QGeoCoordinate previousCoordinate = m_coordinate;
    m_coordinate = coordinate;

    // Comparing two NotANumbers is false which is not wanted here
    if (coordinate.altitude() != previousCoordinate.altitude() &&
        !(qIsNaN(coordinate.altitude()) && qIsNaN(previousCoordinate.altitude()))) {
        emit altitudeChanged(m_coordinate.altitude());
        changed = true;
    }
    if (coordinate.latitude() != previousCoordinate.latitude() &&
        !(qIsNaN(coordinate.latitude()) && qIsNaN(previousCoordinate.latitude()))) {
        emit latitudeChanged(m_coordinate.latitude());
        changed = true;
    }
    if (coordinate.longitude() != previousCoordinate.longitude() &&
        !(qIsNaN(coordinate.longitude()) && qIsNaN(previousCoordinate.longitude()))) {
        emit longitudeChanged(m_coordinate.longitude());
        changed = true;
    }

    if (changed)
        emit coordinateChanged(m_coordinate);
}

QGeoCoordinate QDeclarativeCoordinate::coordinate() const
{
    return m_coordinate;
}

/*!
    \qmlproperty double Coordinate::altitude

    This property holds the value of altitude (meters above sea level).
    If the property has not been set, its default value is zero.

*/

void QDeclarativeCoordinate::setAltitude(double altitude)
{
    bool wasValid = m_coordinate.isValid();
    if (m_coordinate.altitude() != altitude) {
        m_coordinate.setAltitude(altitude);
        emit altitudeChanged(m_coordinate.altitude());
        emit coordinateChanged(m_coordinate);

        if (wasValid != m_coordinate.isValid())
            emit validityChanged(m_coordinate.isValid());
    }
}

double QDeclarativeCoordinate::altitude() const
{
    return m_coordinate.altitude();
}

/*!
    \qmlproperty double Coordinate::longitude

    This property holds the longitude value of the geographical position
    (decimal degrees). A positive longitude indicates the Eastern Hemisphere,
    and a negative longitude indicates the Western Hemisphere
    If the property has not been set, its default value is zero.
*/

void QDeclarativeCoordinate::setLongitude(double longitude)
{
    bool wasValid = m_coordinate.isValid();
    if (m_coordinate.longitude() != longitude) {
        m_coordinate.setLongitude(longitude);
        emit longitudeChanged(m_coordinate.longitude());
        emit coordinateChanged(m_coordinate);

        if (wasValid != m_coordinate.isValid())
            emit validityChanged(m_coordinate.isValid());
    }
}

double QDeclarativeCoordinate::longitude() const
{
    return m_coordinate.longitude();
}

/*!
    \qmlproperty double Coordinate::latitude

    This property holds latitude value of the geographical position
    (decimal degrees). A positive latitude indicates the Northern Hemisphere,
    and a negative latitude indicates the Southern Hemisphere.
    If the property has not been set, its default value is zero.
*/

void QDeclarativeCoordinate::setLatitude(double latitude)
{
    bool wasValid = m_coordinate.isValid();
    if (m_coordinate.latitude() != latitude) {
        m_coordinate.setLatitude(latitude);
        emit latitudeChanged(latitude);
        emit coordinateChanged(m_coordinate);

        if (wasValid != m_coordinate.isValid())
            emit validityChanged(m_coordinate.isValid());
    }
}

double QDeclarativeCoordinate::latitude() const
{
    return m_coordinate.latitude();
}

/*!
    \qmlproperty bool Coordinate::isValid

    This property holds the current validity of the coordinate. Coordinates
    are considered valid if they have been set with a valid latitude and
    longitude (altitude is not required).

    The latitude must be between -90 to 90 inclusive to be considered valid,
    and the longitude must be between -180 to 180 inclusive to be considered
    valid.
*/

bool QDeclarativeCoordinate::isValid() const
{
    return m_coordinate.isValid();
}

/*!
    \qmlmethod Coordinate::distanceTo(Coordinate)

    Returns the distance (in meters) from this coordinate to the
    coordinate specified by other. Altitude is not used in the calculation.

    This calculation returns the great-circle distance between the two
    coordinates, with an assumption that the Earth is spherical for the
    purpose of this calculation.
*/

qreal QDeclarativeCoordinate::distanceTo(QObject *coordinate)
{
    QDeclarativeCoordinate *coord = static_cast<QDeclarativeCoordinate *>(coordinate);
    return m_coordinate.distanceTo(coord->coordinate());
}

/*!
    \qmlmethod Coordinate::azimuthTo(Coordinate)

    Returns the azimuth (or bearing) in degrees from this coordinate to the
    coordinate specified by other. Altitude is not used in the calculation.

    There is an assumption that the Earth is spherical for the purpose of
    this calculation.
*/
qreal QDeclarativeCoordinate::azimuthTo(QObject *coordinate)
{
    QDeclarativeCoordinate *coord = static_cast<QDeclarativeCoordinate *>(coordinate);
    return m_coordinate.azimuthTo(coord->coordinate());
}

/*!
    \qmlmethod Coordinate::atDistanceAndAzimuth(qreal, qreal)

    Returns the coordinate that is reached by traveling distance metres
    from the current coordinate at azimuth degrees along a great-circle.

    There is an assumption that the Earth is spherical for the purpose
    of this calculation.
*/
QDeclarativeCoordinate *QDeclarativeCoordinate::atDistanceAndAzimuth(qreal distance, qreal azimuth)
{
    QGeoCoordinate coord = m_coordinate.atDistanceAndAzimuth(distance, azimuth);
    return new QDeclarativeCoordinate(coord);
}

#include "moc_qdeclarativecoordinate_p.cpp"

QT_END_NAMESPACE

