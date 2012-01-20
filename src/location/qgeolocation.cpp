/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeolocation.h"
#include "qgeolocation_p.h"

QT_USE_NAMESPACE

QGeoLocationPrivate::QGeoLocationPrivate()
    : QSharedData()
{
}

QGeoLocationPrivate::QGeoLocationPrivate(const QGeoLocationPrivate &other)
    : QSharedData()
{
    this->address = other.address;
    this->coordinate = other.coordinate;
    this->viewport = other.viewport;
}

QGeoLocationPrivate::~QGeoLocationPrivate()
{
}

bool QGeoLocationPrivate::operator==(const QGeoLocationPrivate &other) const
{
    return  (this->address == other.address
            && this->coordinate == other.coordinate
            && this->viewport == other.viewport);

}

/*!
    \class QGeoLocation
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QGeoLocation class represents basic information about a location.

    A QGeoLocation consists of a coordinate and corresponding address, along with an optional
    bounding box which is the recommended region to be displayed when viewing the location.
*/

/*!
    \fn bool QGeoLocation::operator!=(const QGeoLocation &other) const

    Returns true if this location is not equal to \a other, otherwise returns false.
*/

/*!
    Constructs an new location object.
*/
QGeoLocation::QGeoLocation()
    : d(new QGeoLocationPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QGeoLocation::QGeoLocation(const QGeoLocation &other)
    :d(other.d)
{
}

/*!
    Destroys the location object.
*/
QGeoLocation::~QGeoLocation()
{
}

/*!
    Assigns \a other to this location and returns a reference to this location.
*/
QGeoLocation &QGeoLocation::operator =(const QGeoLocation &other) {
    d = other.d;
    return *this;
}

/*!
    Returns true if this location is equal to \a other,
    otherwise returns false.
*/
bool QGeoLocation::operator==(const QGeoLocation &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns the address of the location.
*/
QGeoAddress QGeoLocation::address() const
{
    return d->address;
}

/*!
    Sets the \a address of the location.
*/
void QGeoLocation::setAddress(const QGeoAddress &address)
{
    d->address = address;
}

/*!
    Returns the coordinate of the location.
*/
QGeoCoordinate QGeoLocation::coordinate() const
{
    return d->coordinate;
}

/*!
    Sets the \a coordinate of the location.
*/
void QGeoLocation::setCoordinate(const QGeoCoordinate &coordinate)
{
    d->coordinate = coordinate;
}

/*!
    Returns a bounding box which represents the recommended region
    to display when viewing this location.
*/
QGeoBoundingBox QGeoLocation::boundingBox() const
{
    return d->viewport;
}

/*!
    Sets the \a boundingBox of the location.
*/
void QGeoLocation::setBoundingBox(const QGeoBoundingBox &boundingBox)
{
    d->viewport = boundingBox;
}
