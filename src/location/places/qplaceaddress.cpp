#include "qplaceaddress.h"
#include "qplaceaddress_p.h"

QTM_USE_NAMESPACE

QPlaceAddressPrivate::QPlaceAddressPrivate() : QSharedData()
{
}

QPlaceAddressPrivate::QPlaceAddressPrivate(const QPlaceAddressPrivate &other)
    : QSharedData()
{
    this->additionalData = other.additionalData;
    this->alternativeAttributes = other.alternativeAttributes;
    this->city = other.city;
    this->country = other.country;
    this->countryName = other.countryName;
    this->county = other.county;
    this->district = other.district;
    this->floor = other.floor;
    this->houseNumber = other.houseNumber;
    this->label = other.label;
    this->postalCode = other.postalCode;
    this->state = other.state;
    this->street = other.street;
    this->suite = other.suite;
}

QPlaceAddressPrivate::~QPlaceAddressPrivate()
{
}

bool QPlaceAddressPrivate::operator==(const QPlaceAddressPrivate &other) const
{
    return (
            this->additionalData == other.additionalData
            && this->alternativeAttributes == other.alternativeAttributes
            && this->city == other.city
            && this->country == other.country
            && this->countryName == other.countryName
            && this->county == other.county
            && this->district == other.district
            && this->floor == other.floor
            && this->houseNumber == other.houseNumber
            && this->label == other.label
            && this->postalCode == other.postalCode
            && this->state == other.state
            && this->street == other.street
            && this->suite == other.suite
    );
}

/*!
    \class QPlaceAddress
    \inmodule QPlaces

    \brief The QPlaceAddress class represents a address object.

    Each QPlaceAddress represents a address object with a number of attributes
    such as street, country etc. Each QPlaceAddress is associated with location.

    Address objects are read-only, e.g. user of API might get address objects
    associated to specific place but can not edit its content. User might also create new address.

    QPlaceAddress is an in memory representation of a address object.
*/

/*!
    Default constructor. Constructs an new address object.
*/
QPlaceAddress::QPlaceAddress()
    : d(new QPlaceAddressPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceAddress::QPlaceAddress(const QPlaceAddress &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceAddress::~QPlaceAddress()
{
}

QPlaceAddress &QPlaceAddress::operator =(const QPlaceAddress &other) {
    d = other.d;
    return *this;
}

bool QPlaceAddress::operator==(const QPlaceAddress &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns additional data.
*/
QVariantHash QPlaceAddress::additionalData() const
{
    return d->additionalData;
}

/*!
    Sets additional data.
*/
void QPlaceAddress::setAdditionalData(const QVariantHash &data)
{
    d->additionalData = data;
}

/*!
    Returns alternative attributes.
*/
QList<QPlaceAlternativeValue> QPlaceAddress::alternativeAttributes() const
{
    return d->alternativeAttributes;
}

/*!
    Sets alternative attributes.
*/
void QPlaceAddress::setAlternativeAttributes(const QList<QPlaceAlternativeValue> &attributes)
{
    d->alternativeAttributes = attributes;
}

/*!
    Returns city.
*/
QString QPlaceAddress::city() const
{
    return d->city;
}

/*!
    Sets city.
*/
void QPlaceAddress::setCity(const QString &city)
{
    d->city = city;
}

/*!
    Returns country.
*/
QString QPlaceAddress::country() const
{
    return d->country;
}

/*!
    Sets country.
*/
void QPlaceAddress::setCountry(const QString &country)
{
    d->country = country;
}

/*!
    Returns country name.
*/
QString QPlaceAddress::countryName() const
{
    return d->countryName;
}

/*!
    Sets country name.
*/
void QPlaceAddress::setCountryName(const QString &name)
{
    d->countryName = name;
}

/*!
    Returns county.
*/
QString QPlaceAddress::county() const
{
    return d->county;
}

/*
    Sets county.
*/
void QPlaceAddress::setCounty(const QString &county)
{
    d->county = county;
}

/*!
    Returns district.
*/
QString QPlaceAddress::district() const
{
    return d->district;
}

/*!
    Sets district.
*/
void QPlaceAddress::setDistrict(const QString &district)
{
    d->district = district;
}

/*!
    Returns floor.
*/
QString QPlaceAddress::floor() const
{
    return d->floor;
}

/*!
    Sets floor.
*/
void QPlaceAddress::setFloor(const QString &floor)
{
    d->floor = floor;
}

/*!
    Returns house number.
*/
QString QPlaceAddress::houseNumber() const
{
    return d->houseNumber;
}

/*!
    Sets house number.
*/
void QPlaceAddress::setHouseNumber(const QString &houseNumber)
{
    d->houseNumber = houseNumber;
}

/*!
    Returns label.
*/
QString QPlaceAddress::label() const
{
    return d->label;
}

/*!
    Sets label.
*/
void QPlaceAddress::setLabel(const QString &label)
{
    d->label = label;
}

/*!
    Returns postal code.
*/
QString QPlaceAddress::postalCode() const
{
    return d->postalCode;
}

/*!
    Sets postal code.
*/
void QPlaceAddress::setPostalCode(const QString &code)
{
    d->postalCode = code;
}

/*!
    Returns state.
*/
QString QPlaceAddress::state() const
{
    return d->state;
}

/*!
    Sets state.
*/
void QPlaceAddress::setState(const QString &state)
{
    d->state = state;
}

/*!
    Returns street.
*/
QString QPlaceAddress::street() const
{
    return d->street;
}

/*!
    Sets street.
*/
void QPlaceAddress::setStreet(const QString &street)
{
    d->street = street;
}

/*!
    Returns suite.
*/
QString QPlaceAddress::suite() const
{
    return d->suite;
}

/*!
    Sets suite.
*/
void QPlaceAddress::setSuite(const QString &suite)
{
    d->suite = suite;
}
