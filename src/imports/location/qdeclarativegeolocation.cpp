#include "qdeclarativegeolocation_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Location

    \brief The Location element holds location data.
    \inherits QObject

    Location cointains many properties holding data of the location like geo coordinates,
    address, etc.

    \ingroup qml-places
*/

QDeclarativeGeoLocation::QDeclarativeGeoLocation(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeGeoLocation::QDeclarativeGeoLocation(const QGeoLocation &src,
        QObject *parent)
        : QObject(parent),
          m_src(src),
          m_address(src.address()),
          m_coordinate(src.coordinate()),
          m_boundingBox(src.viewport())
{
    for (int i = 0; i < src.navigationPositions().count(); ++i) {
        m_navigationPositions.append(new QDeclarativeCoordinate(src.navigationPositions().at(i)));
    }
}

QDeclarativeGeoLocation::~QDeclarativeGeoLocation()
{
}

void QDeclarativeGeoLocation::setLocation(const QGeoLocation &src)
{
    QGeoLocation previous = m_src;
    m_src = src;

    if (previous.additionalData() != m_src.additionalData()) {
        emit additionalDataChanged();
    }
    if (previous.alternativeLabels() != m_src.alternativeLabels()) {
        synchronizeAlternativeValues();
        emit alternativeLabelsChanged();
    }
    if (previous.address() != m_src.address()) {
        m_address.setAddress(m_src.address());
        emit addressChanged();
    }
    if (previous.coordinate() != m_src.coordinate()) {
        m_coordinate.setCoordinate(m_src.coordinate());
        emit coordinateChanged();
    }
    if (previous.navigationPositions() != m_src.navigationPositions()) {
        synchronizeNavigationPositions();
        emit navigationPositionsChanged();
    }
    if (previous.label() != m_src.label()) {
        emit labelChanged();
    }
    if (previous.locationId() != m_src.locationId()) {
        emit locationIdChanged();
    }
    if (previous.viewport() != m_src.viewport()) {
        emit viewport();
    }
}

QGeoLocation QDeclarativeGeoLocation::location()
{
    QList<QPlaceAlternativeValue> list;
    foreach (QDeclarativeAlternativeValue *value, m_alternativeValues) {
        list.append(value->valueObject());
    }
    m_src.setAlternativeLabels(list);
    QList<QGeoCoordinate> navigationList;
    foreach (QDeclarativeCoordinate *value, m_navigationPositions) {
        navigationList.append(value->coordinate());
    }
    m_src.setNavigationPositions(navigationList);
    m_src.setAddress(m_address.address());
    m_src.setCoordinate(m_coordinate.coordinate());
    m_src.setViewport(m_boundingBox.box());
    return m_src;
}

/*!
    \qmlproperty QVariantHash Location::additionalData

    This property holds additional data for location. Those are pairs of strings (key/value).
*/

void QDeclarativeGeoLocation::setAdditionalData(const QVariantHash &additionalData)
{
    if (m_src.additionalData() != additionalData) {
        m_src.setAdditionalData(additionalData);
        emit additionalDataChanged();
    }
}

QVariantHash QDeclarativeGeoLocation::additionalData() const
{
    return m_src.additionalData();
}

/*!
    \qmlproperty string Location::address

    This property holds address of the location.
*/
void QDeclarativeGeoLocation::setAddress(QDeclarativeGeoAddress *address)
{
    if (m_src.address() != address->address()) {
        m_address.setAddress(address->address());
        m_src.setAddress(address->address());
        emit addressChanged();
    }
}

QDeclarativeGeoAddress *QDeclarativeGeoLocation::address()
{
    return &m_address;
}

/*!
    \qmlproperty string Location::coordinate

    This property holds display coordinates of the location.

   Note: this property's changed() signal is currently emitted only if the
   whole element changes, not if only the contents of the element change.
*/
void QDeclarativeGeoLocation::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (m_src.coordinate() != coordinate->coordinate()) {
        m_coordinate.setCoordinate(coordinate->coordinate());
        m_src.setCoordinate(coordinate->coordinate());
        emit coordinateChanged();
    }
}

QDeclarativeCoordinate *QDeclarativeGeoLocation::coordinate()
{
    return &m_coordinate;
}

/*!
    \qmlproperty string Location::label

    This property holds label.
*/
void QDeclarativeGeoLocation::setLabel(const QString &label)
{
    if (m_src.label() != label) {
        m_src.setLabel(label);
        emit labelChanged();
    }
}

QString QDeclarativeGeoLocation::label() const
{
    return m_src.label();
}

/*!
    \qmlproperty string Location::locationId

    This property holds location id.
*/
void QDeclarativeGeoLocation::setLocationId(const QString &locationId)
{
    if (m_src.locationId() != locationId) {
        m_src.setLocationId(locationId);
        emit locationIdChanged();
    }
}

QString QDeclarativeGeoLocation::locationId() const
{
    return m_src.locationId();
}

/*!
    \qmlproperty BoundingBox Location::viewport

    This property holds bouding box of area on map ocupied by location.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativeGeoLocation::setViewport(QDeclarativeGeoBoundingBox *viewport)
{
    if (m_src.viewport() != viewport->box()) {
        m_boundingBox.setBox(viewport->box());
        m_src.setViewport(viewport->box());
        emit viewportChanged();
    }
}

QDeclarativeGeoBoundingBox *QDeclarativeGeoLocation::viewport()
{
    return &m_boundingBox;
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeAlternativeValue> Location::alternativeLabels

    This property alternative values for label property.

     Note: this property's changed() signal is currently emitted only if the
     whole element changes, not if only the contents of the element change.
*/
QDeclarativeListProperty<QDeclarativeAlternativeValue> QDeclarativeGeoLocation::alternativeLabels()
{
    return QDeclarativeListProperty<QDeclarativeAlternativeValue>(this,
                                                          0, // opaque data parameter
                                                          alternativeValue_append,
                                                          alternativeValue_count,
                                                          alternativeValue_at,
                                                          alternativeValue_clear);
}

void QDeclarativeGeoLocation::alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                  QDeclarativeAlternativeValue *value)
{
    QDeclarativeGeoLocation* object = static_cast<QDeclarativeGeoLocation*>(prop->object);
    QDeclarativeAlternativeValue *altValue = new QDeclarativeAlternativeValue(object);
    altValue->setValueObject(value->valueObject());
    object->m_alternativeValues.append(altValue);
    QList<QPlaceAlternativeValue> list = object->m_src.alternativeLabels();
    list.append(value->valueObject());
    object->m_src.setAlternativeLabels(list);
    emit object->alternativeLabelsChanged();
}

int QDeclarativeGeoLocation::alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    return static_cast<QDeclarativeGeoLocation*>(prop->object)->m_alternativeValues.count();
}

QDeclarativeAlternativeValue* QDeclarativeGeoLocation::alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                                          int index)
{
    QDeclarativeGeoLocation* object = static_cast<QDeclarativeGeoLocation*>(prop->object);
    QDeclarativeAlternativeValue *res = NULL;
    if (object->m_alternativeValues.count() > index && index > -1) {
        res = object->m_alternativeValues[index];
    }
    return res;
}

void QDeclarativeGeoLocation::alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    QDeclarativeGeoLocation* object = static_cast<QDeclarativeGeoLocation*>(prop->object);
    qDeleteAll(object->m_alternativeValues);
    object->m_alternativeValues.clear();
    object->m_src.setAlternativeLabels(QList<QPlaceAlternativeValue>());
    emit object->alternativeLabelsChanged();
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeCoordinate> Location::navigationPositions

    This property navigation coordinates for location.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoLocation::navigationPositions()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
                                                          0, // opaque data parameter
                                                          navigationPosition_append,
                                                          navigationPosition_count,
                                                          navigationPosition_at,
                                                          navigationPosition_clear);
}

void QDeclarativeGeoLocation::navigationPosition_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop,
                                                  QDeclarativeCoordinate *value)
{
    QDeclarativeGeoLocation* object = static_cast<QDeclarativeGeoLocation*>(prop->object);
    QDeclarativeCoordinate *altValue = new QDeclarativeCoordinate(object);
    altValue->setCoordinate(value->coordinate());
    object->m_navigationPositions.append(altValue);
    QList<QGeoCoordinate> list = object->m_src.navigationPositions();
    list.append(value->coordinate());
    object->m_src.setNavigationPositions(list);
    emit object->navigationPositionsChanged();
}

int QDeclarativeGeoLocation::navigationPosition_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoLocation*>(prop->object)->m_navigationPositions.count();
}

QDeclarativeCoordinate* QDeclarativeGeoLocation::navigationPosition_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop,
                                                                          int index)
{
    QDeclarativeGeoLocation* object = static_cast<QDeclarativeGeoLocation*>(prop->object);
    QDeclarativeCoordinate *res = NULL;
    if (object->m_navigationPositions.count() > index && index > -1) {
        res = object->m_navigationPositions[index];
    }
    return res;
}

void QDeclarativeGeoLocation::navigationPosition_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoLocation* object = static_cast<QDeclarativeGeoLocation*>(prop->object);
    qDeleteAll(object->m_navigationPositions);
    object->m_navigationPositions.clear();
    object->m_src.setNavigationPositions(QList<QGeoCoordinate>());
    emit object->navigationPositionsChanged();
}

void QDeclarativeGeoLocation::synchronizeAlternativeValues()
{
    qDeleteAll(m_alternativeValues);
    m_alternativeValues.clear();
    foreach (QPlaceAlternativeValue value, m_src.alternativeLabels()) {
        QDeclarativeAlternativeValue* declarativeValue = new QDeclarativeAlternativeValue(value, this);
        m_alternativeValues.append(declarativeValue);
    }
}

void QDeclarativeGeoLocation::synchronizeNavigationPositions()
{
    qDeleteAll(m_navigationPositions);
    m_navigationPositions.clear();
    foreach (QGeoCoordinate value, m_src.navigationPositions()) {
        QDeclarativeCoordinate* declarativeValue = new QDeclarativeCoordinate(value, this);
        m_navigationPositions.append(declarativeValue);
    }
}
