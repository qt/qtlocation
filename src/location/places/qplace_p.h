#ifndef QPLACE_P_H
#define QPLACE_P_H

#include <QSharedData>

#include "qplace.h"

namespace QT_PLACES_NAMESPACE {

class QPlacePrivate : public QSharedData
{
public:
    QPlacePrivate();
    QPlacePrivate(const QPlacePrivate &other);

    ~QPlacePrivate();

    bool operator==(const QPlacePrivate &other) const;

    QVariantHash additionalData;
    QList<QPlaceAlternativeValue> alternativeNames;
    double placeScore;
    QPlaceBusinessInformation businessInfo;
    QList<QPlaceCategory> categories;
    QList<QPlaceContact> contacts;
    QList<QPlaceDescription> descriptions;
    QPlaceLocation location;
    QList<QPlaceLocation> alternativeLocations;
    QPlaceRating rating;
    QList<QPlaceSupplier> suppliers;
    QStringList feeds;
    QPlacePaginationList<QPlaceMediaObject> media;
    uint mediaCount;
    QString name;
    QString placeId;
    QPlacePaginationList<QPlaceReview> reviews;
    uint reviewCount;
    QString shortDescription;
    QStringList tags;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACE_P_H
