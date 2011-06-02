#ifndef QPLACERATING_P_H
#define QPLACERATING_P_H

#include <QSharedData>

#include "qplacerating.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceRatingPrivate : public QSharedData
{
public:
    QPlaceRatingPrivate();
    QPlaceRatingPrivate(const QPlaceRatingPrivate &other);

    ~QPlaceRatingPrivate();

    bool operator==(const QPlaceRatingPrivate &other) const;

    double value;
    uint count;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACERATING_P_H
