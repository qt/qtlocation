#include "qplaceperiod.h"
#include "qplaceperiod_p.h"

QTM_USE_NAMESPACE

QPlacePeriodPrivate::QPlacePeriodPrivate()
    : QSharedData()
{
    QDate date;
    QTime time;
    begin.setDate(date);
    begin.setTime(time);
    end.setDate(date);
    end.setTime(time);
}

QPlacePeriodPrivate::QPlacePeriodPrivate(const QPlacePeriodPrivate &other)
    : QSharedData()
{
    this->begin = other.begin;
    this->end = other.end;
}

QPlacePeriodPrivate::~QPlacePeriodPrivate()
{
}

bool QPlacePeriodPrivate::operator==(const QPlacePeriodPrivate &other) const
{
    return (
            this->begin == other.begin
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
}

QPlacePeriod::QPlacePeriod(int startMonth, int startDay, int startHour, int startMinute,
            int endMonth, int endDay, int endHour, int endMinute)
    : d(new QPlacePeriodPrivate)
{
    QTime startTime(startHour, startMinute);
    QDate startDate(0, startMonth, startDay);
    d->begin.setTime(startTime);
    d->begin.setDate(startDate);
    QTime endTime(endHour, endMinute);
    QDate endDate(0, endMonth, endDay);
    d->end.setTime(endTime);
    d->end.setDate(endDate);
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
    Returns sart date.  Null returned if not set.
    If year was not set, year value will be default(4713 BC)
*/
QDate QPlacePeriod::startDate() const
{
    return d->begin.date();
}

/*!
    Sets th start \a date
*/
void QPlacePeriod::setStartDate(const QDate &date)
{
    d->begin.setDate(date);
}

/*!
    Returns the start time.  Null returned if not set.
*/
QTime QPlacePeriod::startTime() const
{
    return d->begin.time();
}

/*!
    Sets the start time.
*/
void QPlacePeriod::setStartTime(const QTime &time)
{
    d->begin.setTime(time);
}

/*!
    Returns the end date.  Null is returned if not set.
    If the year was not set, year value will be default (4713 BC)
*/
QDate QPlacePeriod::endDate() const
{
    return d->end.date();
}

/*!
    Sets the end \a date.
*/
void QPlacePeriod::setEndDate(const QDate &date)
{
    d->end.setDate(date);
}

/*!
    Returns end time.  Null returned if not set.
*/
QTime QPlacePeriod::endTime() const
{
    return d->end.time();
}

/*!
    Sets the end \a time
*/
void QPlacePeriod::setEndTime(const QTime &time)
{
    d->end.setTime(time);
}
