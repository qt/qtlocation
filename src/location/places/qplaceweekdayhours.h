#ifndef QPLACEWEEKDAYHOURS_H
#define QPLACEWEEKDAYHOURS_H

#include <QSharedDataPointer>
#include "qmobilityglobal.h"
#include "qplaceperiod.h"

QTM_BEGIN_NAMESPACE

class QPlaceWeekdayHoursPrivate;

class Q_LOCATION_EXPORT QPlaceWeekdayHours
{
public:
    QPlaceWeekdayHours();
    QPlaceWeekdayHours(const QPlaceWeekdayHours &other);

    virtual ~QPlaceWeekdayHours();

    QPlaceWeekdayHours &operator=(const QPlaceWeekdayHours &other);

    bool operator==(const QPlaceWeekdayHours &other) const;
    bool operator!=(const QPlaceWeekdayHours &other) const {
        return !(other == *this);
    }

    QPlacePeriod period() const;
    void setPeriod(const QPlacePeriod &period);
    Qt::DayOfWeek weekday() const;
    void setWeekday(const Qt::DayOfWeek &weekday);

private:
    QSharedDataPointer<QPlaceWeekdayHoursPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEWEEKDAYHOURS_H
