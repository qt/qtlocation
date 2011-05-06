
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

#include "qgeoaddress.h"
#include "qgeoaddress_p.h"

#ifdef QGEOADDRESS_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

QGeoAddressPrivate::QGeoAddressPrivate()
        : QSharedData()
{
}

QGeoAddressPrivate::QGeoAddressPrivate(const QGeoAddressPrivate &other)
        : QSharedData(other),
        sCountry(other.sCountry),
        sCountryCode(other.sCountryCode),
        sState(other.sState),
        sCounty(other.sCounty),
        sCity(other.sCity),
        sDistrict(other.sDistrict),
        sStreet(other.sStreet),
        sPostCode(other.sPostCode)
{
}

QGeoAddressPrivate::~QGeoAddressPrivate()
{
}

/*!
    \class QGeoAddress
    \brief The QGeoAddress class represents an address

    \inmodule QtLocation
    \since 1.1

    \ingroup location

    This class represents an address of a location.
*/

/*!
    Default constructor.
*/
QGeoAddress::QGeoAddress()
        : d(new QGeoAddressPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QGeoAddress::QGeoAddress(const QGeoAddress &other)
        : d(other.d)
{
}

/*!
    Destroys this address.
*/
QGeoAddress::~QGeoAddress()
{
}

/*!
    Assigns the given \a address to this address and
    returns a reference to this address.
*/
QGeoAddress &QGeoAddress::operator=(const QGeoAddress & address)
{
    d = address.d;
    return *this;
}

/*!
    Returns true if this address is equal to \a other,
    otherwise returns false.
*/
bool QGeoAddress::operator==(const QGeoAddress &other) const
{
#ifdef QGEOADDRESS_DEBUG
    qDebug() << "country" << (d->sCountry == other.country());
    qDebug() << "countryCode" << (d->sCountryCode == other.countryCode());
    qDebug() << "state:" <<  (d->sState == other.state());
    qDebug() << "county:" << (d->sCounty == other.county());
    qDebug() << "city:" << (d->sCity == other.city());
    qDebug() << "district:" << (d->sDistrict == other.district());
    qDebug() << "street:" << (d->sStreet == other.street());
    qDebug() << "postcode:" << (d->sPostCode == other.postcode());
#endif

    return d->sCountry == other.country() &&
           d->sCountryCode == other.countryCode() &&
           d->sState == other.state() &&
           d->sCounty == other.county() &&
           d->sCity == other.city() &&
           d->sDistrict == other.district() &&
           d->sStreet == other.street() &&
           d->sPostCode == other.postcode();
}

/*!
    \fn bool QGeoAddress::operator!=(const QGeoAddress &other) const

    Returns true if this address is not equal to \a other,
    otherwise returns false.
*/

/*!
    Returns the country name.
*/
QString QGeoAddress::country() const
{
    return d->sCountry;
}

/*!
    Sets the \a country name.
*/
void QGeoAddress::setCountry(const QString &country)
{
    d->sCountry = country;
}

/*!
    Returns the country code according to ISO 3166-1 alpha-3
*/
QString QGeoAddress::countryCode() const
{
    return d->sCountryCode;
}

/*!
    Sets the \a countryCode according to ISO 3166-1 alpha-3
*/
void QGeoAddress::setCountryCode(const QString &countryCode)
{
    d->sCountryCode = countryCode;
}

/*!
    Returns the state.  The state is considered the first subdivision below country.
*/
QString QGeoAddress::state() const
{
    return d->sState;
}

/*!
    Sets the \a state.
*/
void QGeoAddress::setState(const QString &state)
{
    d->sState = state;
}

/*!
    Returns the county.  The county is considered the second subdivision below country.
*/
QString QGeoAddress::county() const
{
    return d->sCounty;
}

/*!
    Sets the \a county.
*/
void QGeoAddress::setCounty(const QString &county)
{
    d->sCounty = county;
}

/*!
    Returns the city.
*/
QString QGeoAddress::city() const
{
    return d->sCity;
}

/*!
    Sets the \a city.
*/
void QGeoAddress::setCity(const QString &city)
{
    d->sCity = city;
}

/*!
    Returns the district.  The district is considered the subdivison below city.
*/
QString QGeoAddress::district() const
{
    return d->sDistrict;
}

/*!
    Sets the \a district.
*/
void QGeoAddress::setDistrict(const QString &district)
{
    d->sDistrict = district;
}

/*!
    Returns the street-level component of the address.

    This typically includes a street number and street name
    but may also contain things like a unit number, a building
    name, or anything else that might be used to
    distinguish one address from another.
*/
QString QGeoAddress::street() const
{
    return d->sStreet;
}

/*!
    Sets the street-level component of the address to \a street.

    This typically includes a street number and street name
    but may also contain things like a unit number, a building
    name, or anything else that might be used to
    distinguish one address from another.
*/
void QGeoAddress::setStreet(const QString &street)
{
    d->sStreet = street;
}

/*!
    Returns the post code.
*/
QString QGeoAddress::postcode() const
{
    return d->sPostCode;
}

/*!
    Sets the \a postcode.
*/
void QGeoAddress::setPostcode(const QString &postcode)
{
    d->sPostCode = postcode;
}

/*!
    Returns whether this address is empty. An address is considered empty
    if \i all of its fields are empty.
*/
bool QGeoAddress::isEmpty() const
{
    return d->sCountry.isEmpty() &&
           d->sCountryCode.isEmpty() &&
           d->sState.isEmpty() &&
           d->sCounty.isEmpty() &&
           d->sCity.isEmpty() &&
           d->sDistrict.isEmpty() &&
           d->sStreet.isEmpty() &&
           d->sPostCode.isEmpty();
}
/*!
    Clears the all the address' data fields.
*/
void QGeoAddress::clear()
{
    d->sCountry.clear();
    d->sCountryCode.clear();
    d->sState.clear();
    d->sCounty.clear();
    d->sCity.clear();
    d->sDistrict.clear();
    d->sStreet.clear();
    d->sPostCode.clear();
}

QTM_END_NAMESPACE

