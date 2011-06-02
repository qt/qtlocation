#ifndef QPLACEREVIEW_P_H
#define QPLACEREVIEW_P_H

#include <QSharedData>

#include "qplacereview.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceReviewPrivate : public QSharedData
{
public:
    QPlaceReviewPrivate();
    QPlaceReviewPrivate(const QPlaceReviewPrivate &other);

    ~QPlaceReviewPrivate();

    bool operator==(const QPlaceReviewPrivate &other) const;

    QString date;
    QString description;
    QString language;
    uint helpfulVotings;
    QStringList mediaIds;
    uint notHelpfulVotings;
    double rating;
    QString reviewId;
    QPlaceSupplier supplier;
    QString title;
    QString userId;
    QString userName;
    QString originatorURL;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEREVIEW_P_H
