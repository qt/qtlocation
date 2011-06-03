#ifndef QPLACEGEOCOORDINATE_P_H
#define QPLACEGEOCOORDINATE_P_H

#include <QSharedData>

#include "qplacegeocoordinate.h"

QTM_BEGIN_NAMESPACE

class QPlaceGeoCoordinatePrivate : public QSharedData
{
public:
    QPlaceGeoCoordinatePrivate();
    QPlaceGeoCoordinatePrivate(const QPlaceGeoCoordinatePrivate &other);

    ~QPlaceGeoCoordinatePrivate();

    bool operator==(const QPlaceGeoCoordinatePrivate &other) const;

    bool isValid() const;

    double latitude;
    double longitude;
};

QTM_END_NAMESPACE

#endif // QPLACEGEOCOORDINATE_P_H
