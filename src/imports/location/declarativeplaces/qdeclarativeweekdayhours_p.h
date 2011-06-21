#ifndef QDECLARATIVEWEEKDAYHOURS_P_H
#define QDECLARATIVEWEEKDAYHOURS_P_H

#include <qplaceweekdayhours.h>
#include <QtDeclarative/qdeclarative.h>
#include <QObject>

#include "qdeclarativeperiod_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeWeekdayHours : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativePeriod* period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(Qt::DayOfWeek weekday READ weekday WRITE setWeekday NOTIFY weekdayChanged)

public:
    explicit QDeclarativeWeekdayHours(QObject* parent = 0);
    explicit QDeclarativeWeekdayHours(const QPlaceWeekdayHours &weekdayHours, QObject* parent = 0);
    ~QDeclarativeWeekdayHours();

    QPlaceWeekdayHours weekdayHours() const;
    void setWeekdayHours(const QPlaceWeekdayHours &weekdayHours);

    QDeclarativePeriod *period();
    void setPeriod(QDeclarativePeriod *period);
    Qt::DayOfWeek weekday() const;
    void setWeekday(const Qt::DayOfWeek &weekday);

signals:
    void periodChanged();
    void weekdayChanged();

private:
    QDeclarativePeriod m_period;
    QPlaceWeekdayHours m_weekdayHours;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeWeekdayHours));

#endif // QDECLARATIVEWEEKDAYHOURS_P_H
