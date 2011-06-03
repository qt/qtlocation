#ifndef QPLACEGEOCOORDINATE_H
#define QPLACEGEOCOORDINATE_H

#include <QSharedDataPointer>
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QPlaceGeoCoordinatePrivate;

class Q_LOCATION_EXPORT QPlaceGeoCoordinate
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

QTM_END_NAMESPACE

#endif // QPLACEGEOCOORDINATE_H
