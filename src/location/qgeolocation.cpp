/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
    this->additionalData = other.additionalData;
    this->address = other.address;
    this->coordinate = other.coordinate;
    this->navigationPositions = other.navigationPositions;
    this->locationId = other.locationId;
    this->viewport = other.viewport;
}

QGeoLocationPrivate::~QGeoLocationPrivate()
{
}

bool QGeoLocationPrivate::operator==(const QGeoLocationPrivate &other) const
{
    return  (this->additionalData == other.additionalData
            && this->address == other.address
            && this->coordinate == other.coordinate
            && this->navigationPositions == other.navigationPositions
            && this->locationId == other.locationId
            && this->viewport == other.viewport);

}

/*!
    \class QGeoLocation

    \inmodule QPlaces

    \brief The QGeoLocation class represents a location object.

    Each QGeoLocation represents a location object with a number of attributes
    such as address, display position etc.

    QGeoLocation is an in memory representation of a location object.
*/

/*!
    Default constructor. Constructs an new location object.
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
    Destructor.
*/
QGeoLocation::~QGeoLocation()
{
}

QGeoLocation &QGeoLocation::operator =(const QGeoLocation &other) {
    d = other.d;
    return *this;
}

bool QGeoLocation::operator==(const QGeoLocation &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns additional data.
*/
QVariantHash QGeoLocation::additionalData() const
{
    return d->additionalData;
}

/*!
    Sets additional data.
*/
void QGeoLocation::setAdditionalData(const QVariantHash &data)
{
    d->additionalData = data;
}

/*!
    Returns address.
*/
QGeoAddress QGeoLocation::address() const
{
    return d->address;
}

/*!
    Sets address.
*/
void QGeoLocation::setAddress(const QGeoAddress &address)
{
    d->address = address;
}

/*!
    Returns the location's coordinate.
*/
QGeoCoordinate QGeoLocation::coordinate() const
{
    return d->coordinate;
}

/*!
    Sets the location's \a coordinate.
*/
void QGeoLocation::setCoordinate(const QGeoCoordinate &coordinate)
{
    d->coordinate = coordinate;
}

/*!
    Returns navigation positions.
*/
QList<QGeoCoordinate> QGeoLocation::navigationPositions() const
{
    return d->navigationPositions;
}

/*!
    Sets navigation positions.
*/
void QGeoLocation::setNavigationPositions(const QList<QGeoCoordinate> &positions)
{
    d->navigationPositions = positions;
}

/*!
    Returns location id.
*/
QString QGeoLocation::locationId() const
{
    return d->locationId;
}

/*!
    Sets location id.
*/
void QGeoLocation::setLocationId(const QString &locationId)
{
    d->locationId = locationId;
}

/*!
    Returns view port.
*/
QGeoBoundingBox QGeoLocation::viewport() const
{
    return d->viewport;
}

/*!
    Sets view port.
*/
void QGeoLocation::setViewport(const QGeoBoundingBox &viewport)
{
    d->viewport = viewport;
}
