#include "qplacegeocoordinate.h"
#include "qplacegeocoordinate_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceGeoCoordinatePrivate::QPlaceGeoCoordinatePrivate()
    : QSharedData(),
      latitude(360),
      longitude(180)
{
}

QPlaceGeoCoordinatePrivate::QPlaceGeoCoordinatePrivate(const QPlaceGeoCoordinatePrivate &other)
    : QSharedData()
{
    this->latitude = other.latitude;
    this->longitude = other.longitude;
}

QPlaceGeoCoordinatePrivate::~QPlaceGeoCoordinatePrivate()
{
}

bool QPlaceGeoCoordinatePrivate::operator==(const QPlaceGeoCoordinatePrivate &other) const
{
    return (
            this->latitude == other.latitude
            && this->longitude == other.longitude
    );
}

bool QPlaceGeoCoordinatePrivate::isValid() const
{
    if ((this->longitude >= -180 && this->longitude <= 180 )
            && (this->latitude >=-90 && this->latitude <= 90 )) {
        return true;
    }
    return false;
}

/*!
    \class QPlaceGeoCoordinate

    \inmodule QPlaces

    \brief The QPlaceGeoCoordinate class represents a position object.

    Each QPlaceGeoCoordinate represents a position object with a longitude and latitude.

    QPlaceGeoCoordinate is an in memory representation of a position object.
*/

/*!
    Constructs a new position object.
*/
QPlaceGeoCoordinate::QPlaceGeoCoordinate()
    : d(new QPlaceGeoCoordinatePrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceGeoCoordinate::QPlaceGeoCoordinate(const QPlaceGeoCoordinate &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceGeoCoordinate::~QPlaceGeoCoordinate()
{
}

QPlaceGeoCoordinate &QPlaceGeoCoordinate::operator =(const QPlaceGeoCoordinate &other) {
    d = other.d;
    return *this;
}

bool QPlaceGeoCoordinate::operator==(const QPlaceGeoCoordinate &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}


/*!
    Returns latitude.
*/
double QPlaceGeoCoordinate::latitude() const
{
    return d->latitude;
}

/*!
    Sets latitude.
*/
void QPlaceGeoCoordinate::setLatitude(const double &src)
{
    d->latitude = src;
}

/*!
    Returns longitude.
*/
double QPlaceGeoCoordinate::longitude() const
{
    return d->longitude;
}

/*!
    Sets longitude.
*/
void QPlaceGeoCoordinate::setLongitude(const double &src)
{
    d->longitude = src;
}

/*!
    Check if position is valid.
*/
bool QPlaceGeoCoordinate::isValid() const
{
    return d->isValid();
}
