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
          m_address(src.address()),
          m_coordinate(src.coordinate()),
          m_boundingBox(src.viewport()),
          m_src(src)
{
}

QDeclarativeGeoLocation::~QDeclarativeGeoLocation()
{
}

void QDeclarativeGeoLocation::setLocation(const QGeoLocation &src)
{
    QGeoLocation previous = m_src;
    m_src = src;

    if (previous.address() != m_src.address()) {
        m_address.setAddress(m_src.address());
        emit addressChanged();
    }
    if (previous.coordinate() != m_src.coordinate()) {
        m_coordinate.setCoordinate(m_src.coordinate());
        emit coordinateChanged();
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
    m_src.setAddress(m_address.address());
    m_src.setCoordinate(m_coordinate.coordinate());
    m_src.setViewport(m_boundingBox.box());
    return m_src;
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
