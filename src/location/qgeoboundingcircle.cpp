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

#include "qgeoboundingcircle.h"
#include "qgeoboundingcircle_p.h"

#include "qgeocoordinate.h"
#include "qnumeric.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingCircle
    \brief The QGeoBoundingCircle class defines a circular geographic area.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps

    The circle is defined in terms of a QGeoCoordinate which specifies the
    center of the circle and a qreal which specifies the radius of the circle
    in metres.

    The circle is considered invalid if the center coordinate is invalid
    or if the radius is less than zero.
*/

/*!
    Constructs a new, invalid bounding circle.
*/
QGeoBoundingCircle::QGeoBoundingCircle()
        : d_ptr(new QGeoBoundingCirclePrivate()) {}

/*!
    Constructs a new bounding circle centered at \a center and with a radius of \a
    radius metres.
*/
QGeoBoundingCircle::QGeoBoundingCircle(const QGeoCoordinate &center, qreal radius)
        : d_ptr(new QGeoBoundingCirclePrivate(center, radius)) {}

/*!
    Constructs a new bounding circle from the contents of \a other.
*/
QGeoBoundingCircle::QGeoBoundingCircle(const QGeoBoundingCircle &other)
        : QGeoBoundingArea(other),
          d_ptr(other.d_ptr) {}

/*!
    Destroys this bounding circle.
*/
QGeoBoundingCircle::~QGeoBoundingCircle() {}

/*!
    Assigns \a other to this bounding circle and returns a reference to this
    bounding circle.
*/
QGeoBoundingCircle& QGeoBoundingCircle::operator = (const QGeoBoundingCircle & other)
{
    QGeoBoundingArea::operator=(other);
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this bounding circle is equal to \a other.
*/
bool QGeoBoundingCircle::operator == (const QGeoBoundingCircle &other) const
{
    return ((*(d_ptr.constData()) == *(other.d_ptr.constData())));
}

/*!
    Returns whether this bounding circle is not equal to \a other.
*/
bool QGeoBoundingCircle::operator != (const QGeoBoundingCircle &other) const
{
    return !(this->operator==(other));
}

/*!
    Returns QGeoBoundingArea::CircleType to identify this as a
    QGeoBoundingCircle instance.

    This function is provided to help find the specific type of
    aQGeoBoundingArea instance.
*/
QGeoBoundingArea::AreaType QGeoBoundingCircle::type() const
{
    return QGeoBoundingArea::CircleType;
}

/*!
    Returns whether this bounding circle is valid.

    A valid bounding circle has a valid center coordinate and a radius
    greater than or equal to zero.
*/
bool QGeoBoundingCircle::isValid() const
{
    return (d_ptr->center.isValid()
            && !qIsNaN(d_ptr->radius)
            && d_ptr->radius >= -1e-7);
}

/*!
    Returns whether this bounding circle has a geometrical area of zero.

    Returns true if this bounding circle is invalid.
*/
bool QGeoBoundingCircle::isEmpty() const
{
    return (!isValid() || (d_ptr->radius <= 1e-7));
}

/*!
    Sets the center coordinate of this bounding circle to \a center.
*/
void QGeoBoundingCircle::setCenter(const QGeoCoordinate &center)
{
    d_ptr->center = center;
}

/*!
    Returns the center coordinate of this bounding circle.
*/
QGeoCoordinate QGeoBoundingCircle::center() const
{
    return d_ptr->center;
}

/*!
    Sets the radius in metres of this bounding circle to \a radius.
*/
void QGeoBoundingCircle::setRadius(qreal radius)
{
    d_ptr->radius = radius;
}

/*!
    Returns the radius in meters of this bounding circle.
*/
qreal QGeoBoundingCircle::radius() const
{
    return d_ptr->radius;
}

/*!
    Returns whether the coordinate \a coordinate is contained within this
    bounding circle.
*/
bool QGeoBoundingCircle::contains(const QGeoCoordinate &coordinate) const
{

    if (!isValid() || !coordinate.isValid())
        return false;

    if (d_ptr->center.distanceTo(coordinate) <= d_ptr->radius)
        return true;

    return false;
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

    double lat = d_ptr->center.latitude();
    double lon = d_ptr->center.longitude();

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

    d_ptr->center = QGeoCoordinate(lat, lon);
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
    : QSharedData(),
    radius(-1.0) {}

QGeoBoundingCirclePrivate::QGeoBoundingCirclePrivate(const QGeoCoordinate &center, qreal radius)
        : QSharedData(),
        center(center),
        radius(radius) {}

QGeoBoundingCirclePrivate::QGeoBoundingCirclePrivate(const QGeoBoundingCirclePrivate &other)
        : QSharedData(),
        center(other.center),
        radius(other.radius) {}

QGeoBoundingCirclePrivate::~QGeoBoundingCirclePrivate() {}

QGeoBoundingCirclePrivate& QGeoBoundingCirclePrivate::operator= (const QGeoBoundingCirclePrivate & other)
{
    center = other.center;
    radius = other.radius;

    return *this;
}

bool QGeoBoundingCirclePrivate::operator== (const QGeoBoundingCirclePrivate &other) const
{
    return ((center == other.center) && (radius == other.radius));
}

QTM_END_NAMESPACE

