#ifndef QPLACEBUSINESSINFORMATION_P_H
#define QPLACEBUSINESSINFORMATION_P_H

#include <QSharedData>

#include "qplacebusinessinformation.h"

QTM_BEGIN_NAMESPACE

class QPlaceBusinessInformationPrivate : public QSharedData
{
public:
    QPlaceBusinessInformationPrivate();
    QPlaceBusinessInformationPrivate(const QPlaceBusinessInformationPrivate &other);

    ~QPlaceBusinessInformationPrivate();

    bool operator==(const QPlaceBusinessInformationPrivate &other) const;

    QVariantHash additionalData;
    QList<QPlacePeriod> annualClosings;
    QList<QPlaceWeekdayHours> openingHours;
    QList<QPlaceBusinessFeature> features;
    QString annualClosingNote;
    QString openingNote;
    QStringList paymentMethods;
};

QTM_END_NAMESPACE

#endif // QPLACEBUSINESSINFORMATION_P_H
