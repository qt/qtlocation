#ifndef QPLACEGEOCOORDINATE_P_H
#define QPLACEGEOCOORDINATE_P_H

#include <QSharedData>

#include "qplacegeoboundingbox.h"
#include "qplacegeocoordinate.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceGeoBoundingBoxPrivate : public QSharedData
{
public:
    QPlaceGeoBoundingBoxPrivate();
    QPlaceGeoBoundingBoxPrivate(const QPlaceGeoBoundingBoxPrivate &other);

    ~QPlaceGeoBoundingBoxPrivate();

    bool operator==(const QPlaceGeoBoundingBoxPrivate &other) const;

    QPlaceGeoCoordinate topLeft;
    QPlaceGeoCoordinate bottomRight;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEGEOCOORDINATE_P_H
