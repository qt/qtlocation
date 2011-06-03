#ifndef QPLACEPERIOD_H
#define QPLACEPERIOD_H

#include <QDateTime>
#include <QSharedDataPointer>
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QPlacePeriodPrivate;

class Q_LOCATION_EXPORT QPlacePeriod
{
public:
    QPlacePeriod();
    QPlacePeriod(int startHour, int startMinute, int endHour, int endMinute);
    QPlacePeriod(int startMonth, int startDay, int startHour, int startMinute,
                int endMonth, int endDay, int endHour, int endMinute);
    QPlacePeriod(int startYear, int startMonth, int startDay, int startHour, int startMinute,
                int endYear, int endMonth, int endDay, int endHour, int endMinute);

    QPlacePeriod(const QPlacePeriod &other);

    virtual ~QPlacePeriod();

    QPlacePeriod &operator=(const QPlacePeriod &other);

    bool operator==(const QPlacePeriod &other) const;
    bool operator!=(const QPlacePeriod &other) const {
        return !(other == *this);
    }

    int startYear() const;
    int startMonth() const;
    int startDay() const;
    int startHour() const;
    int startMinute() const;
    int endYear() const;
    int endMonth() const;
    int endDay() const;
    int endHour() const;
    int endMinute() const;

private:
    QSharedDataPointer<QPlacePeriodPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEPERIOD_H
