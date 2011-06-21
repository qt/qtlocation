#include "qdeclarativeaddress_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass Address

    \brief The Address element holds address data.
    \inherits QObject

    Address cointains many properties holding data of the address like street,
    city, country, etc.

    \ingroup qml-places
*/

QDeclarativeAddress::QDeclarativeAddress(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeAddress::QDeclarativeAddress(const QPlaceAddress &src,
        QObject *parent)
        : QObject(parent),
          m_src(src)
{
    synchronizeAlternativeValues();
}

QDeclarativeAddress::~QDeclarativeAddress()
{
}

void QDeclarativeAddress::setAddress(const QPlaceAddress &src)
{
    QPlaceAddress previous = m_src;
    m_src = src;

    if (previous.additionalData() != m_src.additionalData()) {
        emit additionalDataChanged();
    }
    if (previous.alternativeAttributes() != m_src.alternativeAttributes()) {
        synchronizeAlternativeValues();
        emit alternativeAttributesChanged();
    }
    if (previous.city() != m_src.city()) {
        emit cityChanged();
    }
    if (previous.country() != m_src.country()) {
        emit countryChanged();
    }
    if (previous.countryName() != m_src.countryName()) {
        emit countryNameChanged();
    }
    if (previous.county() != m_src.county()) {
        emit countyChanged();
    }
    if (previous.district() != m_src.district()) {
        emit districtChanged();
    }
    if (previous.floor() != m_src.floor()) {
        emit floorChanged();
    }
    if (previous.houseNumber() != m_src.houseNumber()) {
        emit houseNumberChanged();
    }
    if (previous.label() != m_src.label()) {
        emit labelChanged();
    }
    if (previous.state() != m_src.state()) {
        emit stateChanged();
    }
    if (previous.street() != m_src.street()) {
        emit streetChanged();
    }
    if (previous.suite() != m_src.suite()) {
        emit suiteChanged();
    }
}

QPlaceAddress QDeclarativeAddress::address() const
{
    return m_src;
}

/*!
    \qmlproperty QVariantHash Address::additionalData

    This property holds additional data for address. Those are pairs of strings (key/value).
*/

void QDeclarativeAddress::setAdditionalData(const QVariantHash &additionalData)
{
    if (m_src.additionalData() != additionalData) {
        m_src.setAdditionalData(additionalData);
        emit additionalDataChanged();
    }
}

QVariantHash QDeclarativeAddress::additionalData() const
{
    return m_src.additionalData();
}

/*!
    \qmlproperty string Address::city

    This property holds city name.
*/

void QDeclarativeAddress::setCity(const QString &city)
{
    if (m_src.city() != city) {
        m_src.setCity(city);
        emit cityChanged();
    }
}

QString QDeclarativeAddress::city() const
{
    return m_src.city();
}

/*!
    \qmlproperty string Address::country

    This property holds country code.
*/

void QDeclarativeAddress::setCountry(const QString &country)
{
    if (m_src.country() != country) {
        m_src.setCountry(country);
        emit countryChanged();
    }
}

QString QDeclarativeAddress::country() const
{
    return m_src.country();
}

/*!
    \qmlproperty string Address::countryName

    This property holds country name.
*/

void QDeclarativeAddress::setCountryName(const QString &countryName)
{
    if (m_src.countryName() != countryName) {
        m_src.setCountryName(countryName);
        emit countryNameChanged();
    }
}

QString QDeclarativeAddress::countryName() const
{
    return m_src.countryName();
}

/*!
    \qmlproperty string Address::county

    This property holds county name.
*/

void QDeclarativeAddress::setCounty(const QString &county)
{
    if (m_src.county() != county) {
        m_src.setCounty(county);
        emit countyChanged();
    }
}

QString QDeclarativeAddress::county() const
{
    return m_src.county();
}

/*!
    \qmlproperty string Address::district

    This property holds district name.
*/

void QDeclarativeAddress::setDistrict(const QString &district)
{
    if (m_src.district() != district) {
        m_src.setDistrict(district);
        emit districtChanged();
    }
}

QString QDeclarativeAddress::district() const
{
    return m_src.district();
}

/*!
    \qmlproperty string Address::floor

    This property holds floor.
*/

void QDeclarativeAddress::setFloor(const QString &floor)
{
    if (m_src.floor() != floor) {
        m_src.setFloor(floor);
        emit floorChanged();
    }
}

QString QDeclarativeAddress::floor() const
{
    return m_src.floor();
}

/*!
    \qmlproperty string Address::houseNumber

    This property holds house number.
*/

void QDeclarativeAddress::setHouseNumber(const QString &houseNumber)
{
    if (m_src.houseNumber() != houseNumber) {
        m_src.setHouseNumber(houseNumber);
        emit houseNumberChanged();
    }
}

QString QDeclarativeAddress::houseNumber() const
{
    return m_src.houseNumber();
}

/*!
    \qmlproperty string Address::label

    This property holds label.
*/

void QDeclarativeAddress::setLabel(const QString &label)
{
    if (m_src.label() != label) {
        m_src.setLabel(label);
        emit labelChanged();
    }
}

QString QDeclarativeAddress::label() const
{
    return m_src.label();
}

/*!
    \qmlproperty string Address::postalCode

    This property holds postal code.
*/

void QDeclarativeAddress::setPostalCode(const QString &postalCode)
{
    if (m_src.postalCode() != postalCode) {
        m_src.setPostalCode(postalCode);
        emit postalCodeChanged();
    }
}

QString QDeclarativeAddress::postalCode() const
{
    return m_src.postalCode();
}

/*!
    \qmlproperty string Address::state

    This property holds state name.
*/

void QDeclarativeAddress::setState(const QString &state)
{
    if (m_src.state() != state) {
        m_src.setState(state);
        emit stateChanged();
    }
}

QString QDeclarativeAddress::state() const
{
    return m_src.state();
}

/*!
    \qmlproperty string Address::street

    This property holds street name.
*/

void QDeclarativeAddress::setStreet(const QString &street)
{
    if (m_src.street() != street) {
        m_src.setStreet(street);
        emit streetChanged();
    }
}

QString QDeclarativeAddress::street() const
{
    return m_src.street();
}

/*!
    \qmlproperty string Address::suite

    This property holds suite.
*/

void QDeclarativeAddress::setSuite(const QString &suite)
{
    if (m_src.suite() != suite) {
        m_src.setSuite(suite);
        emit suiteChanged();
    }
}

QString QDeclarativeAddress::suite() const
{
    return m_src.suite();
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeAlternativeValue> Address::alternativeAttributes

    This property alternative values for label property.
*/
QDeclarativeListProperty<QDeclarativeAlternativeValue> QDeclarativeAddress::alternativeAttributes()
{
    return QDeclarativeListProperty<QDeclarativeAlternativeValue>(this,
                                                          0, // opaque data parameter
                                                          alternativeValue_append,
                                                          alternativeValue_count,
                                                          alternativeValue_at,
                                                          alternativeValue_clear);
}

void QDeclarativeAddress::alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                  QDeclarativeAlternativeValue *value)
{
    QDeclarativeAddress* object = static_cast<QDeclarativeAddress*>(prop->object);
    QDeclarativeAlternativeValue *altValue = new QDeclarativeAlternativeValue(object);
    altValue->setValueObject(value->valueObject());
    object->m_alternativeValues.append(altValue);
    QList<QPlaceAlternativeValue> list = object->m_src.alternativeAttributes();
    list.append(value->valueObject());
    object->m_src.setAlternativeAttributes(list);
    emit object->alternativeAttributesChanged();
}

int QDeclarativeAddress::alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    return static_cast<QDeclarativeAddress*>(prop->object)->m_alternativeValues.count();
}

QDeclarativeAlternativeValue* QDeclarativeAddress::alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                                          int index)
{
    QDeclarativeAddress* object = static_cast<QDeclarativeAddress*>(prop->object);
    QDeclarativeAlternativeValue *res = NULL;
    if (object->m_alternativeValues.count() > index && index > -1) {
        res = object->m_alternativeValues[index];
    }
    return res;
}

void QDeclarativeAddress::alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    QDeclarativeAddress* object = static_cast<QDeclarativeAddress*>(prop->object);
    qDeleteAll(object->m_alternativeValues);
    object->m_alternativeValues.clear();
    object->m_src.setAlternativeAttributes(QList<QPlaceAlternativeValue>());
    emit object->alternativeAttributesChanged();
}

void QDeclarativeAddress::synchronizeAlternativeValues()
{
    qDeleteAll(m_alternativeValues);
    m_alternativeValues.clear();
    foreach (QPlaceAlternativeValue value, m_src.alternativeAttributes()) {
        QDeclarativeAlternativeValue* declarativeValue = new QDeclarativeAlternativeValue(value, this);
        m_alternativeValues.append(declarativeValue);
    }
}
