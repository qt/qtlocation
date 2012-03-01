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

#include "qgeoaddress.h"
#include "qgeoaddress_p.h"

#ifdef QGEOADDRESS_DEBUG
#include <QDebug>
#endif

QT_BEGIN_NAMESPACE

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
        sPostalCode(other.sPostalCode),
        sText(other.sText)
{
}

QGeoAddressPrivate::~QGeoAddressPrivate()
{
}

/*!
    \class QGeoAddress
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \ingroup QtLocation-places-data
    \ingroup QtLocation-places
    \since QtLocation 5.0

    \brief The QGeoAddress class represents an address of a \l QGeoLocation.

    The address' attributes are normalized to US feature names and can be mapped
    to the local feature levels (e.g. State matches "Bundesland" in Germany).

    The address contains a \l text() for displaying purposes and additional
    properties to access the components of an address:

    \list
        \o QGeoAddress::country()
        \o QGeoAddress::countryCode()
        \o QGeoAddress::state()
        \o QGeoAddress::city()
        \o QGeoAddress::district()
        \o QGeoAddress::street()
        \o QGeoAddress::postalCode()
    \endlist
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
    qDebug() << "postalCode:" << (d->sPostalCode == other.postalCode());
#endif

    return d->sCountry == other.country() &&
           d->sCountryCode == other.countryCode() &&
           d->sState == other.state() &&
           d->sCounty == other.county() &&
           d->sCity == other.city() &&
           d->sDistrict == other.district() &&
           d->sStreet == other.street() &&
           d->sPostalCode == other.postalCode() &&
           d->sText == other.text();
}

/*!
    \fn bool QGeoAddress::operator!=(const QGeoAddress &other) const

    Returns true if this address is not equal to \a other,
    otherwise returns false.
*/

/*!
    Returns the address as a single formatted string.  If the returned string is not empty then
    it is the recommended string to use to display the address to the user.

    The address text may contain a subset of all address properties and is provided by the
    plugin.  A common pattern is for the text to take the format of an address as found on an
    envelope, but this is not always necessarily the case.
*/
QString QGeoAddress::text() const
{
    return d->sText;
}

/*!
    Sets the address text to \a address.
*/
void QGeoAddress::setText(const QString &address)
{
    d->sText = address;
}

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
    Returns the postal code.
*/
QString QGeoAddress::postalCode() const
{
    return d->sPostalCode;
}

/*!
    Sets the \a postalCode.
*/
void QGeoAddress::setPostalCode(const QString &postalCode)
{
    d->sPostalCode = postalCode;
}

/*!
    Returns whether this address is empty. An address is considered empty
    if \e all of its fields are empty.
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
           d->sPostalCode.isEmpty() &&
           d->sText.isEmpty();

}

/*!
    Clears all of the address' data fields.
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
    d->sPostalCode.clear();
    d->sText.clear();
}

QT_END_NAMESPACE

