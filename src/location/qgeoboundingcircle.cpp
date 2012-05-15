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

#include "qgeoboundingcircle.h"
#include "qgeoboundingcircle_p.h"

#include "qgeocoordinate.h"
#include "qnumeric.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingCircle
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \since Qt Location 5.0

    \brief The QGeoBoundingCircle class defines a circular geographic area.

    The circle is defined in terms of a QGeoCoordinate which specifies the
    center of the circle and a qreal which specifies the radius of the circle
    in meters.

    The circle is considered invalid if the center coordinate is invalid
    or if the radius is less than zero.
*/

inline QGeoBoundingCirclePrivate *QGeoBoundingCircle::d_func()
{
    return static_cast<QGeoBoundingCirclePrivate *>(d_ptr.data());
}

inline const QGeoBoundingCirclePrivate *QGeoBoundingCircle::d_func() const
{
    return static_cast<const QGeoBoundingCirclePrivate *>(d_ptr.constData());
}

/*!
    Constructs a new, invalid bounding circle.
*/
QGeoBoundingCircle::QGeoBoundingCircle()
:   QGeoBoundingArea(new QGeoBoundingCirclePrivate)
{
}

/*!
    Constructs a new bounding circle centered at \a center and with a radius of \a
    radius meters.
*/
QGeoBoundingCircle::QGeoBoundingCircle(const QGeoCoordinate &center, qreal radius)
{
    d_ptr = new QGeoBoundingCirclePrivate(center, radius);
}

/*!
    Constructs a new bounding circle from the contents of \a other.
*/
QGeoBoundingCircle::QGeoBoundingCircle(const QGeoBoundingCircle &other)
:   QGeoBoundingArea(other)
{
}

/*!
    Constructs a new bounding circle from the contents of \a other.
*/
QGeoBoundingCircle::QGeoBoundingCircle(const QGeoBoundingArea &other)
:   QGeoBoundingArea(other)
{
    if (type() != QGeoBoundingArea::CircleType)
        d_ptr = new QGeoBoundingCirclePrivate;
}

/*!
    Destroys this bounding circle.
*/
QGeoBoundingCircle::~QGeoBoundingCircle() {}

/*!
    Assigns \a other to this bounding circle and returns a reference to this
    bounding circle.
*/
QGeoBoundingCircle &QGeoBoundingCircle::operator = (const QGeoBoundingCircle & other)
{
    if (this == &other)
        return *this;

    QGeoBoundingArea::operator=(other);
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this bounding circle is equal to \a other.
*/
bool QGeoBoundingCircle::operator==(const QGeoBoundingCircle &other) const
{
    Q_D(const QGeoBoundingCircle);

    return *d == *other.d_func();
}

/*!
    Returns whether this bounding circle is not equal to \a other.
*/
bool QGeoBoundingCircle::operator!=(const QGeoBoundingCircle &other) const
{
    Q_D(const QGeoBoundingCircle);

    return !(*d == *other.d_func());
}

bool QGeoBoundingCirclePrivate::isValid() const
{
    return center.isValid() && !qIsNaN(radius) && radius >= -1e-7;
}

bool QGeoBoundingCirclePrivate::isEmpty() const
{
    return !isValid() || radius <= 1e-7;
}

/*!
    Sets the center coordinate of this bounding circle to \a center.
*/
void QGeoBoundingCircle::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoBoundingCircle);

    d->center = center;
}

/*!
    Returns the center coordinate of this bounding circle.
*/
QGeoCoordinate QGeoBoundingCircle::center() const
{
    Q_D(const QGeoBoundingCircle);

    return d->center;
}

/*!
    Sets the radius in meters of this bounding circle to \a radius.
*/
void QGeoBoundingCircle::setRadius(qreal radius)
{
    Q_D(QGeoBoundingCircle);

    d->radius = radius;
}

/*!
    Returns the radius in meters of this bounding circle.
*/
qreal QGeoBoundingCircle::radius() const
{
    Q_D(const QGeoBoundingCircle);

    return d->radius;
}

bool QGeoBoundingCirclePrivate::contains(const QGeoCoordinate &coordinate) const
{
    if (!isValid() || !coordinate.isValid())
        return false;

    return center.distanceTo(coordinate) <= radius;
}

/*!
    Translates this bounding circle by \a degreesLatitude northwards and \a
    degreesLongitude eastwards.

    Negative values of \a degreesLatitude and \a degreesLongitude correspond to
    southward and westward translation respectively.
*/
void QGeoBoundingCircle::translate(double degreesLatitude, double degreesLongitude)
{
    // TODO handle dlat, dlon larger than 360 degrees

    Q_D(QGeoBoundingCircle);

    double lat = d->center.latitude();
    double lon = d->center.longitude();

    lat += degreesLatitude;
    lon += degreesLongitude;

    if (lon < -180.0)
        lon += 360.0;
    if (lon > 180.0)
        lon -= 360.0;

    if (lat > 90.0) {
        lat = 180.0 - lat;
        if (lon < 0.0)
            lon = 180.0;
        else
            lon -= 180;
    }

    if (lat < -90.0) {
        lat = 180.0 + lat;
        if (lon < 0.0)
            lon = 180.0;
        else
            lon -= 180;
    }

    d->center = QGeoCoordinate(lat, lon);
}

/*!
    Returns a copy of this bounding circle translated by \a degreesLatitude northwards and \a
    degreesLongitude eastwards.

    Negative values of \a degreesLatitude and \a degreesLongitude correspond to
    southward and westward translation respectively.

    \sa translate()
*/
QGeoBoundingCircle QGeoBoundingCircle::translated(double degreesLatitude, double degreesLongitude) const
{
    QGeoBoundingCircle result(*this);
    result.translate(degreesLatitude, degreesLongitude);
    return result;
}

/*******************************************************************************
*******************************************************************************/

QGeoBoundingCirclePrivate::QGeoBoundingCirclePrivate()
:   QGeoBoundingAreaPrivate(QGeoBoundingArea::CircleType), radius(-1.0)
{
}

QGeoBoundingCirclePrivate::QGeoBoundingCirclePrivate(const QGeoCoordinate &center, qreal radius)
:   QGeoBoundingAreaPrivate(QGeoBoundingArea::CircleType), center(center), radius(radius)
{
}

QGeoBoundingCirclePrivate::QGeoBoundingCirclePrivate(const QGeoBoundingCirclePrivate &other)
:   QGeoBoundingAreaPrivate(QGeoBoundingArea::CircleType), center(other.center),
    radius(other.radius)
{
}

QGeoBoundingCirclePrivate::~QGeoBoundingCirclePrivate() {}

QGeoBoundingAreaPrivate *QGeoBoundingCirclePrivate::clone() const
{
    return new QGeoBoundingCirclePrivate(*this);
}

bool QGeoBoundingCirclePrivate::operator==(const QGeoBoundingAreaPrivate &other) const
{
    if (!QGeoBoundingAreaPrivate::operator==(other))
        return false;

    const QGeoBoundingCirclePrivate &otherCircle = static_cast<const QGeoBoundingCirclePrivate &>(other);

    return radius == otherCircle.radius && center == otherCircle.center;
}

QT_END_NAMESPACE

