#ifndef QPLACEWEEKDAYHOURS_P_H
#define QPLACEWEEKDAYHOURS_P_H

#include <QSharedData>

#include "qplaceweekdayhours.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceWeekdayHoursPrivate : public QSharedData
{
public:
    QPlaceWeekdayHoursPrivate();
    QPlaceWeekdayHoursPrivate(const QPlaceWeekdayHoursPrivate &other);

    ~QPlaceWeekdayHoursPrivate();

    bool operator==(const QPlaceWeekdayHoursPrivate &other) const;

    QPlacePeriod period;
    Qt::DayOfWeek weekday;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEWEEKDAYHOURS_P_H
