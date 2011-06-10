#include "qplacelocation.h"
#include "qplacelocation_p.h"

QTM_USE_NAMESPACE

QPlaceLocationPrivate::QPlaceLocationPrivate()
    : QSharedData(),
      locationScore(0)
{
}

QPlaceLocationPrivate::QPlaceLocationPrivate(const QPlaceLocationPrivate &other)
    : QSharedData()
{
    this->additionalData = other.additionalData;
    this->address = other.address;
    this->alternativeLabels = other.alternativeLabels;
    this->displayPosition = other.displayPosition;
    this->navigationPositions = other.navigationPositions;
    this->label = other.label;
    this->locationId = other.locationId;
    this->locationScore = other.locationScore;
    this->mapView = other.mapView;
}

QPlaceLocationPrivate::~QPlaceLocationPrivate()
{
}

bool QPlaceLocationPrivate::operator==(const QPlaceLocationPrivate &other) const
{
    return (
            this->additionalData == other.additionalData
            && this->address == other.address
            && this->alternativeLabels == other.alternativeLabels
            && this->displayPosition == other.displayPosition
            && this->navigationPositions == other.navigationPositions
            && this->label == other.label
            && this->locationId == other.locationId
            && this->locationScore == other.locationScore
            && this->mapView == other.mapView
    );
}

/*!
    \class QPlaceLocation

    \inmodule QPlaces

    \brief The QPlaceLocation class represents a location object.

    Each QPlaceLocation represents a location object with a number of attributes
    such as label, display position etc. Each QPlaceLocation is associated with place.

    Location objects are read-only, e.g. user of API might get location object
    associated to specific place but can not edit its content. User might also create new
    location object and add it to place.

    QPlaceLocation is an in memory representation of a location object.
*/

/*!
    Default constructor. Constructs an new location object.
*/
QPlaceLocation::QPlaceLocation()
    : d(new QPlaceLocationPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceLocation::QPlaceLocation(const QPlaceLocation &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceLocation::~QPlaceLocation()
{
}

QPlaceLocation &QPlaceLocation::operator =(const QPlaceLocation &other) {
    d = other.d;
    return *this;
}

bool QPlaceLocation::operator==(const QPlaceLocation &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns additional data.
*/
QVariantHash QPlaceLocation::additionalData() const
{
    return d->additionalData;
}

/*!
    Sets additional data.
*/
void QPlaceLocation::setAdditionalData(const QVariantHash &data)
{
    d->additionalData = data;
}

/*!
    Returns address.
*/
QPlaceAddress QPlaceLocation::address() const
{
    return d->address;
}

/*!
    Sets address.
*/
void QPlaceLocation::setAddress(const QPlaceAddress &address)
{
    d->address = address;
}

/*!
    Returns alternative labels.
*/
QList<QPlaceAlternativeValue> QPlaceLocation::alternativeLabels() const
{
    return d->alternativeLabels;
}

/*!
    Sets alternative labels.
*/
void QPlaceLocation::setAlternativeLabels(const QList<QPlaceAlternativeValue> &labels)
{
    d->alternativeLabels = labels;
}

/*!
    Returns display position.
*/
QPlaceGeoCoordinate QPlaceLocation::displayPosition() const
{
    return d->displayPosition;
}

/*!
    Sets display position.
*/
void QPlaceLocation::setDisplayPosition(const QPlaceGeoCoordinate &position)
{
    d->displayPosition = position;
}

/*!
    Returns navigation positions.
*/
QList<QPlaceGeoCoordinate> QPlaceLocation::navigationPositions() const
{
    return d->navigationPositions;
}

/*!
    Sets navigation positions.
*/
void QPlaceLocation::setNavigationPositions(const QList<QPlaceGeoCoordinate> &positions)
{
    d->navigationPositions = positions;
}

/*!
    Returns label.
*/
QString QPlaceLocation::label() const
{
    return d->label;
}

/*!
    Sets label.
*/
void QPlaceLocation::setLabel(const QString &label)
{
    d->label = label;
}

/*!
    Returns location id.
*/
QString QPlaceLocation::locationId() const
{
    return d->locationId;
}

/*!
    Sets location id.
*/
void QPlaceLocation::setLocationId(const QString &locationId)
{
    d->locationId = locationId;
}

/*!
    Returns location score.
*/
int QPlaceLocation::locationScore() const
{
    return d->locationScore;
}

/*!
    Sets location score.
*/
void QPlaceLocation::setLocationScore(const int &score)
{
    d->locationScore = score;
}

/*!
    Returns view port.
*/
QPlaceGeoBoundingBox QPlaceLocation::mapView() const
{
    return d->mapView;
}

/*!
    Sets view port.
*/
void QPlaceLocation::setMapView(const QPlaceGeoBoundingBox &coordinate)
{
    d->mapView = coordinate;
}
