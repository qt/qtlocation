#ifndef QPLACELOCATION_P_H
#define QPLACELOCATION_P_H

#include <QSharedData>

#include "qplacelocation.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceLocationPrivate : public QSharedData
{
public:
    QPlaceLocationPrivate();
    QPlaceLocationPrivate(const QPlaceLocationPrivate &other);

    ~QPlaceLocationPrivate();

    bool operator==(const QPlaceLocationPrivate &other) const;

    QVariantHash additionalData;
    QPlaceAddress address;
    QList<QPlaceAlternativeValue> alternativeLabels;
    QPlaceGeoCoordinate displayPosition;
    QList<QPlaceGeoCoordinate> navigationPositions;
    QString label;
    QString locationId;
    int locationScore;
    QPlaceGeoBoundingBox mapView;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACELOCATION_P_H
