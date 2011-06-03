#ifndef QPLACERATING_P_H
#define QPLACERATING_P_H

#include <QSharedData>

#include "qplacerating.h"

QTM_BEGIN_NAMESPACE

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

QTM_END_NAMESPACE

#endif // QPLACERATING_P_H
