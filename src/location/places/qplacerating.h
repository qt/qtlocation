#ifndef QPLACERATING_H
#define QPLACERATING_H

#include <QSharedDataPointer>
#include "qplaceglobal.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceRatingPrivate;

class Q_PLACES_EXPORT QPlaceRating
{
public:
    QPlaceRating();
    QPlaceRating(const QPlaceRating &other);

    virtual ~QPlaceRating();

    QPlaceRating &operator=(const QPlaceRating &other);

    bool operator==(const QPlaceRating &other) const;
    bool operator!=(const QPlaceRating &other) const {
        return !(other == *this);
    }

    double value() const;
    void setValue(const double &data);
    uint count() const;
    void setCount(const uint &data);

private:
    QSharedDataPointer<QPlaceRatingPrivate> d;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACERATING_H
