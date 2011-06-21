#ifndef QDECLARATIVEPERIOD_P_H
#define QDECLARATIVEPERIOD_P_H

#include <qplaceperiod.h>
#include <QtDeclarative/qdeclarative.h>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativePeriod : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDate startDate READ startDate WRITE setStartDate NOTIFY startDateChanged)
    Q_PROPERTY(QTime startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate NOTIFY endDateChanged)
    Q_PROPERTY(QTime endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged)

public:
    explicit QDeclarativePeriod(QObject* parent = 0);
    explicit QDeclarativePeriod(const QPlacePeriod &period, QObject* parent = 0);
    ~QDeclarativePeriod();

    QPlacePeriod period() const;
    void setPeriod(const QPlacePeriod &period);

    QDate startDate() const;
    void setStartDate(const QDate &data);
    QTime startTime() const;
    void setStartTime(const QTime &data);
    QDate endDate() const;
    void setEndDate(const QDate &data);
    QTime endTime() const;
    void setEndTime(const QTime &data);

signals:
    void startDateChanged();
    void startTimeChanged();
    void endDateChanged();
    void endTimeChanged();

private:
    QPlacePeriod m_period;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativePeriod));

#endif // QDECLARATIVEPERIOD_P_H
