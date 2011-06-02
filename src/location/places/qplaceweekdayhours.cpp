#include "qplaceweekdayhours.h"
#include "qplaceweekdayhours_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceWeekdayHoursPrivate::QPlaceWeekdayHoursPrivate() : QSharedData()
{
}

QPlaceWeekdayHoursPrivate::QPlaceWeekdayHoursPrivate(const QPlaceWeekdayHoursPrivate &other)
    : QSharedData()
{
    this->period = other.period;
    this->weekday = other.weekday;
}

QPlaceWeekdayHoursPrivate::~QPlaceWeekdayHoursPrivate()
{
}

bool QPlaceWeekdayHoursPrivate::operator==(const QPlaceWeekdayHoursPrivate &other) const
{
    return (
            this->period == other.period
            && this->weekday == other.weekday
    );
}

/*!
    \class QPlaces::QWeekdayHours

    \inmodule QPlaces

    \brief The QWeekdayHours class represents a day of week object.
    It might contains hours information and day of the week that place is open.
*/

/*!
    Constructs an new weekday object.
*/
QPlaceWeekdayHours::QPlaceWeekdayHours()
    : d(new QPlaceWeekdayHoursPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceWeekdayHours::QPlaceWeekdayHours(const QPlaceWeekdayHours &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceWeekdayHours::~QPlaceWeekdayHours()
{
}

QPlaceWeekdayHours &QPlaceWeekdayHours::operator =(const QPlaceWeekdayHours &other) {
    d = other.d;
    return *this;
}

bool QPlaceWeekdayHours::operator==(const QPlaceWeekdayHours &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns period. Do not delete it.
*/
QPlacePeriod QPlaceWeekdayHours::period() const
{
    return d->period;
}
/*!
    Sets period.
*/
void QPlaceWeekdayHours::setPeriod(const QPlacePeriod &period)
{
    d->period = period;
}

/*!
    Returns day of the week.
*/
Qt::DayOfWeek QPlaceWeekdayHours::weekday() const
{
    return d->weekday;
}

/*!
    Sets weekday.
*/
void QPlaceWeekdayHours::setWeekday(const Qt::DayOfWeek &weekday)
{
    d->weekday = weekday;
}

