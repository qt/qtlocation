#ifndef QPLACEGEOBOUNDINGBOX_H
#define QPLACEGEOBOUNDINGBOX_H

#include <QSharedDataPointer>
#include "qmobilityglobal.h"
#include "qplacegeocoordinate.h"

QTM_BEGIN_NAMESPACE

class QPlaceGeoBoundingBoxPrivate;

class Q_LOCATION_EXPORT QPlaceGeoBoundingBox
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

    bool isValid() const;

private:
    QSharedDataPointer<QPlaceGeoBoundingBoxPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEGEOBOUNDINGBOX_H
