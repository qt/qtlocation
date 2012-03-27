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

#include "qdeclarativegeoaddress_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Address QDeclarativeGeoAddress
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-positioning
    \since Qt Location 5.0

    \brief The Address QML element represents a specific location as a street address.

    An Address is used as a unit of data for queries such as (Reverse) Geocoding
    or Places searches -- many of these operations either accept an Address
    or return one.

    Not all properties of an Address are necessarily available or relevant
    in all parts of the world and all locales. The \l district, \l state and
    \l county properties are particularly area-specific for many data sources
    (including the provided Nokia plugin), and often only one or two of these
    is available or useful.

    \section2 Example Usage

    The following code snippet shows the declaration of an Address element.

    \code
    Address {
        id: address
        street: "53 Brandl St"
        city: "Eight Mile Plains"
        country: "Australia"
    }
    \endcode

    This could then be used, for example, as the value of a \l{GeocodeModel}'s
    \l{GeocodeModel::query}{query} property, to get an exact longitude and
    latitude for the address.

    \sa {QGeoAddress}
*/

QDeclarativeGeoAddress::QDeclarativeGeoAddress(QObject* parent) :
        QObject(parent)
{
}

QDeclarativeGeoAddress::QDeclarativeGeoAddress(const QGeoAddress& address, QObject* parent) :
        QObject(parent), m_address(address)
{
}

/*!
    \qmlproperty QGeoAddress QtLocation5::Address::address

    For details on how to use this property to interface between C++ and QML see
    "\l {location-cpp-qml.html#geoaddress} {Interfaces between C++ and QML Code}".
*/
QGeoAddress QDeclarativeGeoAddress::address() const
{
    return m_address;
}

void QDeclarativeGeoAddress::setAddress(const QGeoAddress& address)
{
    // Elaborate but takes care of emiting needed signals
    setText(address.text());
    setCountry(address.country());
    setCountryCode(address.countryCode());
    setState(address.state());
    setCounty(address.county());
    setCity(address.city());
    setDistrict(address.district());
    setStreet(address.street());
    setPostalCode(address.postalCode());
    m_address = address;
}

/*!
    \qmlproperty string QtLocation5::Address::text

    This property holds the address as a single formatted string.  If this property is not empty
    then it is the recommended string to use to display the address to the user.

    The address text may contain a subset of all address properties.  A common pattern is for the
    text to take the format of an address as found on an envelope, but this is not always necessarily
    the case.
*/
QString QDeclarativeGeoAddress::text() const
{
    return m_address.text();
}

void QDeclarativeGeoAddress::setText(const QString &address)
{
    if (m_address.text() == address)
        return;

    m_address.setText(address);
    emit textChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::country

  This property holds the country of the address as a single formatted string.
*/
QString QDeclarativeGeoAddress::country() const
{
    return m_address.country();
}

void QDeclarativeGeoAddress::setCountry(const QString& country)
{
    if (m_address.country() == country)
        return;
    m_address.setCountry(country);
    emit countryChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::countryCode

  This property holds the country code of the address as a single formatted string.
*/
QString QDeclarativeGeoAddress::countryCode() const
{
    return m_address.countryCode();
}

void QDeclarativeGeoAddress::setCountryCode(const QString& countryCode)
{
    if (m_address.countryCode() == countryCode)
        return;
    m_address.setCountryCode(countryCode);
    emit countryCodeChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::state

  This property holds the state of the address as a single formatted string.
*/
QString QDeclarativeGeoAddress::state() const
{
    return m_address.state();
}

void QDeclarativeGeoAddress::setState(const QString& state)
{
    if (m_address.state() == state)
        return;
    m_address.setState(state);
    emit stateChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::county

  This property holds the county of the address as a single formatted string.
*/
QString QDeclarativeGeoAddress::county() const
{
    return m_address.county();
}

void QDeclarativeGeoAddress::setCounty(const QString& county)
{
    if (m_address.county() == county)
        return;
    m_address.setCounty(county);
    emit countyChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::city

  This property holds the city of the address as a single formatted string.
*/
QString QDeclarativeGeoAddress::city() const
{
    return m_address.city();
}

void QDeclarativeGeoAddress::setCity(const QString& city)
{
    if (m_address.city() == city)
        return;
    m_address.setCity(city);
    emit cityChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::district

  This property holds the district of the address as a single formatted string.
*/
QString QDeclarativeGeoAddress::district() const
{
    return m_address.district();
}

void QDeclarativeGeoAddress::setDistrict(const QString& district)
{
    if (m_address.district() == district)
        return;
    m_address.setDistrict(district);
    emit districtChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::street

  This property holds the street of the address but
  may also contain things like a unit number, a building
  name, or anything else that might be used to
  distinguish one address from another.
*/
QString QDeclarativeGeoAddress::street() const
{
    return m_address.street();
}

void QDeclarativeGeoAddress::setStreet(const QString& street)
{
    if (m_address.street() == street)
        return;
    m_address.setStreet(street);
    emit streetChanged();
}

/*!
  \qmlproperty string QtLocation5::Address::postalCode

  This property holds the postal code of the address as a single formatted string.
*/
QString QDeclarativeGeoAddress::postalCode() const
{
    return m_address.postalCode();
}

void QDeclarativeGeoAddress::setPostalCode(const QString& postalCode)
{
    if (m_address.postalCode() == postalCode)
        return;
    m_address.setPostalCode(postalCode);
    emit postalCodeChanged();
}

#include "moc_qdeclarativegeoaddress_p.cpp"

QT_END_NAMESPACE
