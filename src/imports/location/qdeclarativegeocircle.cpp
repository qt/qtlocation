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
***************************************************************************/

#include <qnumeric.h>
#include "qdeclarativegeocircle.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype GeoCircle
    \instantiates QDeclarativeGeoCircle
    \inqmlmodule QtLocation 5.0
    \ingroup qml-QtLocation5-positioning
    \since Qt Location 5.0

    \brief The GeoCircle type represents a circular geographic area.

    The circle is defined in terms of a \l {QtLocation5::coordinate}{coordinate} which specifies
    the center of the circle and a qreal which specifies the radius of the circle in meters.

    The circle is considered invalid if the center coordinate is invalid
    or if the radius is less than zero.

    \section2 Example Usage

    The following code snippet shows the declaration of a GeoCircle object.

    \code
    GeoCircle {
        radius: 25.0
        center {
            latitude: 23.34
            longitude: 44.4
        }
    }
    \endcode

    This could then be used, for example, as a region to scan for landmarks,
    or refining searches.

    \sa QGeoCircle
*/

QDeclarativeGeoCircle::QDeclarativeGeoCircle(QObject *parent)
:   QDeclarativeGeoShape(parent)
{
}

QDeclarativeGeoCircle::QDeclarativeGeoCircle(const QGeoCircle &circle, QObject *parent)
:   QDeclarativeGeoShape(parent), m_circle(circle)
{
}

/*!
    \qmlproperty QGeoCircle GeoCircle::circle

    For details on how to use this property to interface between C++ and QML see
    "\l {location-cpp-qml.html#geocircle}{Interfaces between C++ and QML Code}".
*/
void QDeclarativeGeoCircle::setCircle(const QGeoCircle &circle)
{
    if (m_circle == circle)
        return;

    QGeoCircle oldCircle = m_circle;
    m_circle = circle;

    if (oldCircle.center() != m_circle.center())
        emit centerChanged();
    if (oldCircle.radius() != m_circle.radius())
        emit radiusChanged();
}

QGeoCircle QDeclarativeGeoCircle::circle() const
{
    return m_circle;
}

/*!
    \internal
*/
QGeoShape QDeclarativeGeoCircle::shape() const
{
    return circle();
}

/*!
    \qmlmethod bool QDeclarativeGeoCircle::contains(coordinate coordinate)

    Returns the true if \a coordinate is within the bounding circle; otherwise returns false.
*/
bool QDeclarativeGeoCircle::contains(const QGeoCoordinate &coordinate)
{
    return m_circle.contains(coordinate);
}

/*!
    \qmlproperty coordinate GeoCircle::center

    This property holds the coordinate of the center of the bounding circle.

    \note this property's changed() signal is currently emitted only if the
    whole object changes, not if only the contents of the object change.
*/
QGeoCoordinate QDeclarativeGeoCircle::center()
{
    return m_circle.center();
}

void QDeclarativeGeoCircle::setCenter(const QGeoCoordinate &coordinate)
{
    if (m_circle.center() == coordinate)
        return;

    m_circle.setCenter(coordinate);
    emit centerChanged();
}

/*!
    \qmlproperty real GeoCircle::radius

    This property holds the radius of the bounding circle in meters.

    The default value for the radius is -1 indicating an invalid bounding circle area.
*/
qreal QDeclarativeGeoCircle::radius() const
{
    return m_circle.radius();
}

void QDeclarativeGeoCircle::setRadius(qreal radius)
{
    if (m_circle.radius() == radius)
        return;

    m_circle.setRadius(radius);
    emit radiusChanged();
}

#include "moc_qdeclarativegeocircle.cpp"

QT_END_NAMESPACE
