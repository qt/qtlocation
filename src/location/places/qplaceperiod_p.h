#ifndef QPLACEPERIOD_P_H
#define QPLACEPERIOD_P_H

#include <QSharedData>

#include "qplaceperiod.h"

QTM_BEGIN_NAMESPACE

class QPlacePeriodPrivate : public QSharedData
{
public:
    QPlacePeriodPrivate();
    QPlacePeriodPrivate(const QPlacePeriodPrivate &other);

    ~QPlacePeriodPrivate();

    bool operator==(const QPlacePeriodPrivate &other) const;

    enum Value {
        None = 0,
        Date = 1,
        Time = 2,
        Year = 4
    };

    Q_DECLARE_FLAGS(Values, Value);
    Values values;
    QDateTime begin;
    QDateTime end;
};

QTM_END_NAMESPACE

#endif // QPLACEPERIOD_P_H
