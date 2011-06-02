#ifndef QPLACEGEOBOUNDINGBOX_H
#define QPLACEGEOBOUNDINGBOX_H

#include <QSharedDataPointer>
#include "qplaceglobal.h"
#include "qplacegeocoordinate.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceGeoBoundingBoxPrivate;

class Q_PLACES_EXPORT QPlaceGeoBoundingBox
{
public:
    QPlaceGeoBoundingBox();
    QPlaceGeoBoundingBox(const QPlaceGeoBoundingBox &other);

    virtual ~QPlaceGeoBoundingBox();

    QPlaceGeoBoundingBox &operator=(const QPlaceGeoBoundingBox &other);

    bool operator==(const QPlaceGeoBoundingBox &other) const;
    bool operator!=(const QPlaceGeoBoundingBox &other) const {
        return !(other == *this);
    }

    QPlaceGeoCoordinate topLeft() const;
    void setTopLeft(const QPlaceGeoCoordinate &coordinate);
    QPlaceGeoCoordinate bottomRight() const;
    void setBottomRight(const QPlaceGeoCoordinate &coordinate);

    bool isValid();

private:
    QSharedDataPointer<QPlaceGeoBoundingBoxPrivate> d;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEGEOBOUNDINGBOX_H
