#ifndef QPLACEADDRESS_P_H
#define QPLACEADDRESS_P_H

#include <QSharedData>

#include "qplaceaddress.h"

QTM_BEGIN_NAMESPACE

class QPlaceAddressPrivate : public QSharedData
{
public:
    QPlaceAddressPrivate();
    QPlaceAddressPrivate(const QPlaceAddressPrivate &other);

    ~QPlaceAddressPrivate();

    bool operator==(const QPlaceAddressPrivate &other) const;

    QVariantHash additionalData;
    QList<QPlaceAlternativeValue> alternativeAttributes;
    QString city;
    QString country;
    QString countryName;
    QString county;
    QString district;
    QString floor;
    QString houseNumber;
    QString label;
    QString postalCode;
    QString state;
    QString street;
    QString suite;
};

QTM_END_NAMESPACE

#endif // QPLACEADDRESS_P_H
