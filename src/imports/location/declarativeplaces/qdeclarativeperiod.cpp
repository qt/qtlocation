#include "qdeclarativeperiod_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Period

    \brief The Period element holds various time slot data, such as
    begining and end dates.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativePeriod::QDeclarativePeriod(QObject* parent)
        : QObject(parent) {}

QDeclarativePeriod::QDeclarativePeriod(const QPlacePeriod &period,
        QObject *parent)
        : QObject(parent),
        m_period(period) {}

QDeclarativePeriod::~QDeclarativePeriod() {}

void QDeclarativePeriod::setPeriod(const QPlacePeriod &period)
{
    QPlacePeriod previousPeriod = m_period;
    m_period = period;

    if (period.startDate() != previousPeriod.startDate()) {
        emit startDateChanged();
    }
    if (period.startTime() != previousPeriod.startTime()) {
        emit startTimeChanged();
    }
    if (period.endDate() != previousPeriod.endDate()) {
        emit endDateChanged();
    }
    if (period.endTime() != previousPeriod.endTime()) {
        emit endTimeChanged();
    }
}

QPlacePeriod QDeclarativePeriod::period() const
{
    return m_period;
}

/*!
    \qmlproperty QDate Period::startDate

    This property holds the begining date.
*/
void QDeclarativePeriod::setStartDate(const QDate &startDate)
{
    if (m_period.startDate() != startDate) {
        m_period.setStartDate(startDate);
        emit startDateChanged();
    }
}

QDate QDeclarativePeriod::startDate() const
{
    return m_period.startDate();
}

/*!
    \qmlproperty QTime Period::startTime

    This property holds the begining time.
*/
void QDeclarativePeriod::setStartTime(const QTime &startTime)
{
    if (m_period.startTime() != startTime) {
        m_period.setStartTime(startTime);
        emit startTimeChanged();
    }
}

QTime QDeclarativePeriod::startTime() const
{
    return m_period.startTime();
}

/*!
    \qmlproperty QDate Period::endDate

    This property holds the ending date.
*/
void QDeclarativePeriod::setEndDate(const QDate &endDate)
{
    if (m_period.endDate() != endDate) {
        m_period.setEndDate(endDate);
        emit endDateChanged();
    }
}

QDate QDeclarativePeriod::endDate() const
{
    return m_period.endDate();
}

/*!
    \qmlproperty QTime Period::endTime

    This property holds the ending time.
*/
void QDeclarativePeriod::setEndTime(const QTime &endTime)
{
    if (m_period.endTime() != endTime) {
        m_period.setEndTime(endTime);
        emit endTimeChanged();
    }
}

QTime QDeclarativePeriod::endTime() const
{
    return m_period.endTime();
}
