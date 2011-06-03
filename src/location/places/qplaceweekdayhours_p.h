#ifndef QPLACEWEEKDAYHOURS_P_H
#define QPLACEWEEKDAYHOURS_P_H

#include <QSharedData>

#include "qplaceweekdayhours.h"

QTM_BEGIN_NAMESPACE

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

QTM_END_NAMESPACE

#endif // QPLACEWEEKDAYHOURS_P_H
