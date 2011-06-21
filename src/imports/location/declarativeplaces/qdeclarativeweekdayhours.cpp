#include "qdeclarativeweekdayhours_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass WeekdayHours

    \brief The WeekdayHours element holds various positional data, such as \l
    latitude and \l longitude and.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeWeekdayHours::QDeclarativeWeekdayHours(QObject* parent)
        : QObject(parent) {}

QDeclarativeWeekdayHours::QDeclarativeWeekdayHours(const QPlaceWeekdayHours &weekdayHours,
        QObject *parent)
        : QObject(parent),
          m_period(weekdayHours.period()),
          m_weekdayHours(weekdayHours) {}

QDeclarativeWeekdayHours::~QDeclarativeWeekdayHours() {}

void QDeclarativeWeekdayHours::setWeekdayHours(const QPlaceWeekdayHours &weekdayHours)
{
    QPlaceWeekdayHours previousWeekdayHours = m_weekdayHours;
    m_weekdayHours = weekdayHours;

    if (m_weekdayHours.weekday() != previousWeekdayHours.weekday()) {
        emit weekdayChanged();
    }
    if (m_weekdayHours.period() != previousWeekdayHours.period()) {
        m_period.setPeriod(m_weekdayHours.period());
        emit periodChanged();
    }
}

QPlaceWeekdayHours QDeclarativeWeekdayHours::weekdayHours() const
{
    return m_weekdayHours;
}

/*!
    \qmlproperty Qt::DayOfWeek WeekdayHours::weekday

    This property holds the day of the week.
*/

void QDeclarativeWeekdayHours::setWeekday(const Qt::DayOfWeek &weekday)
{
    if (m_weekdayHours.weekday() != weekday) {
        m_weekdayHours.setWeekday(weekday);
        emit weekdayChanged();
    }
}

Qt::DayOfWeek QDeclarativeWeekdayHours::weekday() const
{
    return m_weekdayHours.weekday();
}

/*!
    \qmlproperty double WeekdayHours::period

    This property holds period time.
*/

void QDeclarativeWeekdayHours::setPeriod(QDeclarativePeriod *period)
{
    if (m_period.period() != period->period()) {
        m_period.setPeriod(period->period());
        emit periodChanged();
    }
}

QDeclarativePeriod* QDeclarativeWeekdayHours::period()
{
    return &m_period;
}
