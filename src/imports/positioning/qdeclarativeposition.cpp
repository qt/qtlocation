/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#include "qdeclarativeposition_p.h"
#include <QtQml/qqml.h>
#include <qnmeapositioninfosource.h>
#include <QFile>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Position
    \instantiates QDeclarativePosition
    \inqmlmodule QtPositioning
    \since 5.2

    \brief The Position type holds positional data at a particular point in time,
    such as coordinate (longitude, latitude, altitude) and speed.

    The Position type holds values related to geographic location such as
    a \l coordinate (longitude, latitude, and altitude), the \l timestamp when
    the Position was obtained, the \l speed at that time, and the accuracy of
    the data.

    Primarily, it is used in the \l{PositionSource::position}{position} property
    of a \l{PositionSource}, as the basic unit of data available from the system
    location data source.

    Not all properties of a Position object are necessarily valid or available
    (for example latitude and longitude may be valid, but speed update has not been
    received or set manually). As a result, corresponding "valid" properties
    are available (for example \l{coordinate} and \l{longitudeValid}, \l{latitudeValid}
    etc) to discern whether the data is available and valid in this position
    update.

    Position objects are read-only and can only be produced by a PositionSource.

    \section2 Example Usage

    See the example given for the \l{PositionSource} type, or the
    \l{flickr}{Flickr} example application.

    \sa PositionSource, coordinate

*/

QDeclarativePosition::QDeclarativePosition(QObject *parent)
        : QObject(parent), m_latitudeValid(false), m_longitudeValid(false),
          m_altitudeValid(false), m_speed(-1), m_speedValid(false), m_horizontalAccuracyValid(false),
          m_verticalAccuracyValid(false), m_horizontalAccuracy(-1), m_verticalAccuracy(-1)
{
}

QDeclarativePosition::~QDeclarativePosition()
{
}

/*!
    \qmlproperty coordinate Position::coordinate

    This property holds the latitude, longitude, and altitude value of the Position.

    It is a read-only property.

    \sa longitudeValid, latitudeValid, altitudeValid
*/

void QDeclarativePosition::setCoordinate(const QGeoCoordinate &coordinate)
{
    if (m_coordinate == coordinate)
        return;

    m_coordinate = coordinate;

    if (coordinate.type() == QGeoCoordinate::Coordinate3D && !m_altitudeValid) {
        m_altitudeValid = true;
        emit altitudeValidChanged();
    } else if (m_altitudeValid) {
        m_altitudeValid = false;
        emit altitudeValidChanged();
    }
    if (coordinate.isValid()) {
        if (!m_longitudeValid) {
            m_longitudeValid = true;
            emit longitudeValidChanged();
        }
        if (!m_latitudeValid) {
            m_latitudeValid = true;
            emit latitudeValidChanged();
        }
    } else {
        if (m_longitudeValid) {
            m_longitudeValid = false;
            emit longitudeValidChanged();
        }
        if (m_latitudeValid) {
            m_latitudeValid = false;
            emit latitudeValidChanged();
        }
    }
    emit coordinateChanged();
}

QGeoCoordinate QDeclarativePosition::coordinate()
{
    return m_coordinate;
}

/*!
    \qmlproperty bool Position::latitudeValid

    This property is true if coordinate's latitude has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa coordinate

*/

bool QDeclarativePosition::isLatitudeValid() const
{
    return m_latitudeValid;
}


/*!
    \qmlproperty bool Position::longitudeValid

    This property is true if coordinate's longitude has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa coordinate

*/

bool QDeclarativePosition::isLongitudeValid() const
{
    return m_longitudeValid;
}


/*!
    \qmlproperty bool Position::speedValid

    This property is true if \l speed has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa speed

*/

bool QDeclarativePosition::isSpeedValid() const
{
    return m_speedValid;
}

/*!
    \qmlproperty bool Position::altitudeValid

    This property is true if coordinate's altitude has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa coordinate

*/

bool QDeclarativePosition::isAltitudeValid() const
{
    return m_altitudeValid;
}

/*!
    \qmlproperty double Position::speed

    This property holds the value of speed (groundspeed, meters / second).

    It is a read-only property.

    \sa speedValid, coordinate
*/

void QDeclarativePosition::setSpeed(double speed)
{
    if (speed == m_speed)
        return;
    m_speed = speed;
    if (!m_speedValid) {
        m_speedValid = true;
        emit speedValidChanged();
    }
    emit speedChanged();
}

double QDeclarativePosition::speed() const
{
    return m_speed;
}

/*!
    \qmlproperty real Position::horizontalAccuracy

    This property holds the horizontal accuracy of the coordinate (in meters).

    \sa horizontalAccuracyValid, coordinate
*/

void QDeclarativePosition::setHorizontalAccuracy(qreal horizontalAccuracy)
{
    if (horizontalAccuracy == m_horizontalAccuracy)
        return;
    m_horizontalAccuracy = horizontalAccuracy;
    if (!m_horizontalAccuracyValid) {
        m_horizontalAccuracyValid = true;
        emit horizontalAccuracyValidChanged();
    }
    emit horizontalAccuracyChanged();
}

qreal QDeclarativePosition::horizontalAccuracy() const
{
    return m_horizontalAccuracy;
}

/*!
    \qmlproperty bool Position::horizontalAccuracyValid

    This property is true if \l horizontalAccuracy has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa horizontalAccuracy

*/

bool QDeclarativePosition::isHorizontalAccuracyValid() const
{
    return m_horizontalAccuracyValid;
}

/*!
    \qmlproperty real Position::verticalAccuracy

    This property holds the vertical accuracy of the coordinate (in meters).

    \sa verticalAccuracyValid, coordinate
*/

void QDeclarativePosition::setVerticalAccuracy(qreal verticalAccuracy)
{
    if (verticalAccuracy == m_verticalAccuracy)
        return;
    m_verticalAccuracy = verticalAccuracy;
    if (!m_verticalAccuracyValid) {
        m_verticalAccuracyValid = true;
        emit verticalAccuracyValidChanged();
    }
    emit verticalAccuracyChanged();
}

qreal QDeclarativePosition::verticalAccuracy() const
{
    return m_verticalAccuracy;
}

/*!
    \qmlproperty bool Position::verticalAccuracyValid

    This property is true if \l verticalAccuracy has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa verticalAccuracy

*/

bool QDeclarativePosition::isVerticalAccuracyValid() const
{
    return m_verticalAccuracyValid;
}

/*!
    \qmlproperty date Position::timestamp

    This property holds the timestamp when this position
    was received. If the property has not been set, it is invalid.

    It is a read-only property.
*/

void QDeclarativePosition::setTimestamp(const QDateTime &timestamp)
{
    if (timestamp == m_timestamp)
        return;
    m_timestamp = timestamp;
    emit timestampChanged();
}

QDateTime QDeclarativePosition::timestamp() const
{
    return m_timestamp;
}

void QDeclarativePosition::invalidate()
{
    // Invalidate all data
    if (m_latitudeValid) {
        m_latitudeValid = false;
        emit latitudeValidChanged();
    }
    if (m_longitudeValid) {
        m_longitudeValid = false;
        emit longitudeValidChanged();
    }
    if (m_altitudeValid) {
        m_altitudeValid = false;
        emit altitudeValidChanged();
    }
     if (m_speedValid) {
         m_speedValid = false;
         emit speedValidChanged();
     }
     if (m_horizontalAccuracyValid) {
         m_horizontalAccuracyValid = false;
         emit horizontalAccuracyValidChanged();
     }
     if (m_verticalAccuracyValid) {
         m_verticalAccuracyValid = false;
         emit verticalAccuracyValidChanged();
     }
}

#include "moc_qdeclarativeposition_p.cpp"

QT_END_NAMESPACE
