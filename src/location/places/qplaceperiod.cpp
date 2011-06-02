#include "qplaceperiod.h"
#include "qplaceperiod_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlacePeriodPrivate::QPlacePeriodPrivate() : QSharedData()
{
    this->values = QPlacePeriodPrivate::None;
}

QPlacePeriodPrivate::QPlacePeriodPrivate(const QPlacePeriodPrivate &other)
    : QSharedData()
{
    this->values = other.values;
    this->begin = other.begin;
    this->end = other.end;
}

QPlacePeriodPrivate::~QPlacePeriodPrivate()
{
}

bool QPlacePeriodPrivate::operator==(const QPlacePeriodPrivate &other) const
{
    return (
            this->values == other.values
            && this->begin == other.begin
            && this->end == other.end
    );
}

/*!
    \class QPlacePeriod

    \inmodule QPlaces

    \brief The QPlacePeriod class represents a time period object.
    It might contains just hours information but it might contains also
    date. If some field is unasaignd it will return -1.
*/

/*!
    Constructs an new period object.
*/
QPlacePeriod::QPlacePeriod()
    : d(new QPlacePeriodPrivate)
{
}


QPlacePeriod::QPlacePeriod(int startHour, int startMinute, int endHour, int endMinute)
    : d(new QPlacePeriodPrivate)
{
    QTime startTime(startHour, startMinute);
    d->begin.setTime(startTime);
    QTime endTime(endHour, endMinute);
    d->end.setTime(endTime);
    d->values = QPlacePeriodPrivate::Time;
}

QPlacePeriod::QPlacePeriod(int startMonth, int startDay, int startHour, int startMinute,
            int endMonth, int endDay, int endHour, int endMinute)
    : d(new QPlacePeriodPrivate)
{
    QTime startTime(startHour, startMinute);
    QDate startDate(2008, startMonth, startDay);
    d->begin.setTime(startTime);
    d->begin.setDate(startDate);
    QTime endTime(endHour, endMinute);
    QDate endDate(2008, endMonth, endDay);
    d->end.setTime(endTime);
    d->end.setDate(endDate);
    d->values = QPlacePeriodPrivate::Time & QPlacePeriodPrivate::Date;
}

QPlacePeriod::QPlacePeriod(int startYear, int startMonth, int startDay, int startHour, int startMinute,
            int endYear, int endMonth, int endDay, int endHour, int endMinute)
    : d(new QPlacePeriodPrivate)
{
    QTime startTime(startHour, startMinute);
    QDate startDate(startYear, startMonth, startDay);
    d->begin.setTime(startTime);
    d->begin.setDate(startDate);
    QTime endTime(endHour, endMinute);
    QDate endDate(endYear, endMonth, endDay);
    d->end.setTime(endTime);
    d->end.setDate(endDate);
    d->values = QPlacePeriodPrivate::Time & QPlacePeriodPrivate::Date & QPlacePeriodPrivate::Year;
}

/*!
    Constructs a copy of \a other
*/
QPlacePeriod::QPlacePeriod(const QPlacePeriod &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlacePeriod::~QPlacePeriod()
{
}

QPlacePeriod &QPlacePeriod::operator =(const QPlacePeriod &other) {
    d = other.d;
    return *this;
}

bool QPlacePeriod::operator==(const QPlacePeriod &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns start year. -1 returned if not set.
*/
int QPlacePeriod::startYear() const
{
    if (d->values & QPlacePeriodPrivate::Year) {
        return d->begin.date().year();
    }
    return -1;
}

/*!
    Returns start month. -1 returned if not set.
*/
int QPlacePeriod::startMonth() const
{
    if (d->values & QPlacePeriodPrivate::Date) {
        return d->begin.date().month();
    }
    return -1;
}

/*!
    Returns start day. -1 returned if not set.
*/
int QPlacePeriod::startDay() const
{
    if (d->values & QPlacePeriodPrivate::Date) {
        return d->begin.date().day();
    }
    return -1;
}

/*!
    Returns start hour. -1 returned if not set.
*/
int QPlacePeriod::startHour() const
{
    if (d->values & QPlacePeriodPrivate::Time) {
        return d->begin.time().hour();
    }
    return -1;
}

/*!
    Returns start minute. -1 returned if not set.
*/
int QPlacePeriod::startMinute() const
{
    if (d->values & QPlacePeriodPrivate::Time) {
        return d->begin.time().minute();
    }
    return -1;
}

/*!
    Returns end year. -1 returned if not set.
*/
int QPlacePeriod::endYear() const
{
    if (d->values & QPlacePeriodPrivate::Year) {
        return d->end.date().year();
    }
    return -1;
}

/*!
    Returns end month. -1 returned if not set.
*/
int QPlacePeriod::endMonth() const
{
    if (d->values & QPlacePeriodPrivate::Date) {
        return d->end.date().month();
    }
    return -1;
}

/*!
    Returns end day. -1 returned if not set.
*/
int QPlacePeriod::endDay() const
{
    if (d->values & QPlacePeriodPrivate::Date) {
        return d->end.date().day();
    }
    return -1;
}

/*!
    Returns end year. -1 returned if not set.
*/
int QPlacePeriod::endHour() const
{
    if (d->values & QPlacePeriodPrivate::Time) {
        return d->end.time().hour();
    }
    return -1;
}

/*!
    Returns end year. -1 returned if not set.
*/
int QPlacePeriod::endMinute() const
{
    if (d->values & QPlacePeriodPrivate::Time) {
        return d->end.time().minute();
    }
    return -1;
}
