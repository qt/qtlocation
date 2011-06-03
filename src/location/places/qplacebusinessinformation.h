#ifndef QPLACEBUSINESSINFORMATION_H
#define QPLACEBUSINESSINFORMATION_H

#include <QSharedDataPointer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVariant>

#include "qplacebusinessfeature.h"
#include "qplaceperiod.h"
#include "qplaceweekdayhours.h"

QTM_BEGIN_NAMESPACE

class QPlaceBusinessInformationPrivate;

class Q_LOCATION_EXPORT QPlaceBusinessInformation
{
public:
    QPlaceBusinessInformation();
    QPlaceBusinessInformation(const QPlaceBusinessInformation &other);

    virtual ~QPlaceBusinessInformation();

    QPlaceBusinessInformation &operator=(const QPlaceBusinessInformation &other);

    bool operator==(const QPlaceBusinessInformation &other) const;
    bool operator!=(const QPlaceBusinessInformation &other) const {
        return !(other == *this);
    }

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QList<QPlacePeriod> annualClosings() const;
    void setAnnualClosings(const QList<QPlacePeriod> &data);
    QList<QPlaceWeekdayHours> openingHours() const;
    void setOpeningHours(const QList<QPlaceWeekdayHours> &data);
    QString annualClosingNote() const;
    void setAnnualClosingNote(const QString &data);
    QList<QPlaceBusinessFeature> features() const;
    void setFeatures(const QList<QPlaceBusinessFeature> &data);
    QString openingNote() const;
    void setOpeningNote(const QString &data);
    QStringList paymentMethods() const;
    void setPaymentMethods(const QStringList &data);

private:
    QSharedDataPointer<QPlaceBusinessInformationPrivate> d;
};

QTM_END_NAMESPACE


#endif // QPLACEBUSINESSINFORMATION_H
