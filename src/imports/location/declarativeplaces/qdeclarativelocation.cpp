#include "qdeclarativelocation_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass Location

    \brief The Location element holds location data.
    \inherits QObject

    Location cointains many properties holding data of the location like geo coordinates,
    address, etc.

    \ingroup qml-places
*/

QDeclarativeLocation::QDeclarativeLocation(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeLocation::QDeclarativeLocation(const QPlaceLocation &src,
        QObject *parent)
        : QObject(parent),
          m_src(src)
{
}

QDeclarativeLocation::~QDeclarativeLocation()
{
}

void QDeclarativeLocation::setLocation(const QPlaceLocation &src)
{
    QPlaceLocation previous = m_src;
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
    if (previous.displayPosition() != m_src.displayPosition()) {
        m_displayPosition.setCoordinate(m_src.displayPosition());
        emit displayPositionChanged();
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
    if (previous.locationScore() != m_src.locationScore()) {
        emit locationScoreChanged();
    }
    if (previous.mapView() != m_src.mapView()) {
        emit mapViewChanged();
    }
}

QPlaceLocation QDeclarativeLocation::location() const
{
    return m_src;
}

/*!
    \qmlproperty QVariantHash Location::additionalData

    This property holds additional data for location. Those are pairs of strings (key/value).
*/

void QDeclarativeLocation::setAdditionalData(const QVariantHash &additionalData)
{
    if (m_src.additionalData() != additionalData) {
        m_src.setAdditionalData(additionalData);
        emit additionalDataChanged();
    }
}

QVariantHash QDeclarativeLocation::additionalData() const
{
    return m_src.additionalData();
}

/*!
    \qmlproperty string Location::address

    This property holds address of the location.
*/
void QDeclarativeLocation::setAddress(QDeclarativeGeoAddress *address)
{
    if (m_src.address() != address->address()) {
        m_address.setAddress(address->address());
        m_src.setAddress(address->address());
        emit addressChanged();
    }
}

QDeclarativeGeoAddress *QDeclarativeLocation::address()
{
    return &m_address;
}

/*!
    \qmlproperty string Location::displayPosition

    This property holds display coordinates of the location.
*/
void QDeclarativeLocation::setDisplayPosition(QDeclarativeCoordinate *displayPosition)
{
    if (m_src.displayPosition() != displayPosition->coordinate()) {
        m_displayPosition.setCoordinate(displayPosition->coordinate());
        m_src.setDisplayPosition(displayPosition->coordinate());
        emit displayPositionChanged();
    }
}

QDeclarativeCoordinate *QDeclarativeLocation::displayPosition()
{
    return &m_displayPosition;
}

/*!
    \qmlproperty string Location::label

    This property holds label.
*/
void QDeclarativeLocation::setLabel(const QString &label)
{
    if (m_src.label() != label) {
        m_src.setLabel(label);
        emit labelChanged();
    }
}

QString QDeclarativeLocation::label() const
{
    return m_src.label();
}

/*!
    \qmlproperty string Location::locationId

    This property holds location id.
*/
void QDeclarativeLocation::setLocationId(const QString &locationId)
{
    if (m_src.locationId() != locationId) {
        m_src.setLocationId(locationId);
        emit locationIdChanged();
    }
}

QString QDeclarativeLocation::locationId() const
{
    return m_src.locationId();
}

/*!
    \qmlproperty string Location::locationScore

    This property holds location score.
*/
void QDeclarativeLocation::setLocationScore(const int &locationScore)
{
    if (m_src.locationScore() != locationScore) {
        m_src.setLocationScore(locationScore);
        emit locationScoreChanged();
    }
}

int QDeclarativeLocation::locationScore() const
{
    return m_src.locationScore();
}

/*!
    \qmlproperty string Location::mapView

    This property holds bouding box of area on map ocupied by location.
*/
void QDeclarativeLocation::setMapView(QDeclarativeGeoBoundingBox *mapView)
{
    if (m_src.mapView() != mapView->box()) {
        m_boundingBox.setBox(mapView->box());
        m_src.setMapView(mapView->box());
        emit mapViewChanged();
    }
}

QDeclarativeGeoBoundingBox *QDeclarativeLocation::mapView()
{
    return &m_boundingBox;
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeAlternativeValue> Location::alternativeLabels

    This property alternative values for label property.
*/
QDeclarativeListProperty<QDeclarativeAlternativeValue> QDeclarativeLocation::alternativeLabels()
{
    return QDeclarativeListProperty<QDeclarativeAlternativeValue>(this,
                                                          0, // opaque data parameter
                                                          alternativeValue_append,
                                                          alternativeValue_count,
                                                          alternativeValue_at,
                                                          alternativeValue_clear);
}

void QDeclarativeLocation::alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                  QDeclarativeAlternativeValue *value)
{
    QDeclarativeLocation* object = static_cast<QDeclarativeLocation*>(prop->object);
    QDeclarativeAlternativeValue *altValue = new QDeclarativeAlternativeValue(object);
    altValue->setValueObject(value->valueObject());
    object->m_alternativeValues.append(altValue);
    QList<QPlaceAlternativeValue> list = object->m_src.alternativeLabels();
    list.append(value->valueObject());
    object->m_src.setAlternativeLabels(list);
    emit object->alternativeLabelsChanged();
}

int QDeclarativeLocation::alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    return static_cast<QDeclarativeLocation*>(prop->object)->m_alternativeValues.count();
}

QDeclarativeAlternativeValue* QDeclarativeLocation::alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                                          int index)
{
    QDeclarativeLocation* object = static_cast<QDeclarativeLocation*>(prop->object);
    QDeclarativeAlternativeValue *res = NULL;
    if (object->m_alternativeValues.count() > index && index > -1) {
        res = object->m_alternativeValues[index];
    }
    return res;
}

void QDeclarativeLocation::alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    QDeclarativeLocation* object = static_cast<QDeclarativeLocation*>(prop->object);
    qDeleteAll(object->m_alternativeValues);
    object->m_alternativeValues.clear();
    object->m_src.setAlternativeLabels(QList<QPlaceAlternativeValue>());
    emit object->alternativeLabelsChanged();
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeCoordinate> Location::navigationPositions

    This property navigation coordinates for location.
*/
QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeLocation::navigationPositions()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
                                                          0, // opaque data parameter
                                                          navigationPosition_append,
                                                          navigationPosition_count,
                                                          navigationPosition_at,
                                                          navigationPosition_clear);
}

void QDeclarativeLocation::navigationPosition_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop,
                                                  QDeclarativeCoordinate *value)
{
    QDeclarativeLocation* object = static_cast<QDeclarativeLocation*>(prop->object);
    QDeclarativeCoordinate *altValue = new QDeclarativeCoordinate(object);
    altValue->setCoordinate(value->coordinate());
    object->m_navigationPositions.append(altValue);
    QList<QGeoCoordinate> list = object->m_src.navigationPositions();
    list.append(value->coordinate());
    object->m_src.setNavigationPositions(list);
    emit object->navigationPositionsChanged();
}

int QDeclarativeLocation::navigationPosition_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeLocation*>(prop->object)->m_navigationPositions.count();
}

QDeclarativeCoordinate* QDeclarativeLocation::navigationPosition_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop,
                                                                          int index)
{
    QDeclarativeLocation* object = static_cast<QDeclarativeLocation*>(prop->object);
    QDeclarativeCoordinate *res = NULL;
    if (object->m_navigationPositions.count() > index && index > -1) {
        res = object->m_navigationPositions[index];
    }
    return res;
}

void QDeclarativeLocation::navigationPosition_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeLocation* object = static_cast<QDeclarativeLocation*>(prop->object);
    qDeleteAll(object->m_navigationPositions);
    object->m_navigationPositions.clear();
    object->m_src.setNavigationPositions(QList<QGeoCoordinate>());
    emit object->navigationPositionsChanged();
}

void QDeclarativeLocation::synchronizeAlternativeValues()
{
    qDeleteAll(m_alternativeValues);
    m_alternativeValues.clear();
    foreach (QPlaceAlternativeValue value, m_src.alternativeLabels()) {
        QDeclarativeAlternativeValue* declarativeValue = new QDeclarativeAlternativeValue(value, this);
        m_alternativeValues.append(declarativeValue);
    }
}

void QDeclarativeLocation::synchronizeNavigationPositions()
{
    qDeleteAll(m_navigationPositions);
    m_navigationPositions.clear();
    foreach (QGeoCoordinate value, m_src.navigationPositions()) {
        QDeclarativeCoordinate* declarativeValue = new QDeclarativeCoordinate(value, this);
        m_navigationPositions.append(declarativeValue);
    }
}
