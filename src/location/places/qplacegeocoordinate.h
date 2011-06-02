#ifndef QPLACEGEOCOORDINATE_H
#define QPLACEGEOCOORDINATE_H

#include <QSharedDataPointer>
#include "qplaceglobal.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceGeoCoordinatePrivate;

class Q_PLACES_EXPORT QPlaceGeoCoordinate
{
public:
    QPlaceGeoCoordinate();
    QPlaceGeoCoordinate(const QPlaceGeoCoordinate &other);

    virtual ~QPlaceGeoCoordinate();

    QPlaceGeoCoordinate &operator=(const QPlaceGeoCoordinate &other);

    bool operator==(const QPlaceGeoCoordinate &other) const;
    bool operator!=(const QPlaceGeoCoordinate &other) const {
        return !(other == *this);
    }

    double latitude() const;
    void setLatitude(const double &lat);
    double longitude() const;
    void setLongitude(const double &lon);

    bool isValid() const;

private:
    QSharedDataPointer<QPlaceGeoCoordinatePrivate> d;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEGEOCOORDINATE_H
