/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecoordinate_p.h"
#include <qnumeric.h>
#include "qdeclarative.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Coordinate QDeclarativeCoordinate
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-positioning
    \since QtLocation 5.0

    \brief The Coordinate element holds various positional data, such as \l
    latitude, \l longitude and \l altitude.
*/

QDeclarativeCoordinate::QDeclarativeCoordinate(QObject* parent)
        : QObject(parent) {}

QDeclarativeCoordinate::QDeclarativeCoordinate(const QGeoCoordinate &coordinate,
        QObject *parent)
        : QObject(parent),
        m_coordinate(coordinate) {}

QDeclarativeCoordinate::~QDeclarativeCoordinate() {}

/*!
    \qmlproperty QGeoCoordinate Coordinate::coordinate

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    Coordinate object must be obtained from C++, then use the \l {QObject::property()}{property()}
    and \l {QObject::setProperty()}{setProperty()} functions to get and set the \c coordinate
    property.

    The following gets the QGeoCoordinate representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp Coordinate get

    The following sets the properties of this object based on a QGeoCoordinate object from C++:

    \snippet snippets/cpp/cppqml.cpp Coordinate set
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

qreal QDeclarativeCoordinate::distanceTo(QObject* coordinate)
{
    QDeclarativeCoordinate* coord = static_cast<QDeclarativeCoordinate*>(coordinate);
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
    QDeclarativeCoordinate *coord = static_cast<QDeclarativeCoordinate*>(coordinate);
    return m_coordinate.azimuthTo(coord->coordinate());
}

/*!
    \qmlmethod Coordinate::atDistanceAndAzimuth(qreal, qreal)

    Returns the coordinate that is reached by traveling distance metres
    from the current coordinate at azimuth (or bearing) along a great-circle.

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

