#ifndef QPLACEADDRESS_H
#define QPLACEADDRESS_H

#include "qmobilityglobal.h"
#include <QSharedDataPointer>
#include <QString>
#include <QVariant>
#include <QList>
#include "qplacealternativevalue.h"

QTM_BEGIN_NAMESPACE

class QPlaceAddressPrivate;

class Q_LOCATION_EXPORT QPlaceAddress
{
public:
    QPlaceAddress();
    QPlaceAddress(const QPlaceAddress &other);

    virtual ~QPlaceAddress();

    QPlaceAddress &operator=(const QPlaceAddress &other);

    bool operator==(const QPlaceAddress &other) const;
    bool operator!=(const QPlaceAddress &other) const {
        return !(other == *this);
    }

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QList<QPlaceAlternativeValue> alternativeAttributes() const;
    void setAlternativeAttributes(const QList<QPlaceAlternativeValue> &alternativeAttributes);
    QString city() const;
    void setCity(const QString &city);
    QString country() const;
    void setCountry(const QString &country);
    QString countryName() const;
    void setCountryName(const QString &countryName);
    QString county() const;
    void setCounty(const QString &county);
    QString district() const;
    void setDistrict(const QString &district);
    QString floor() const;
    void setFloor(const QString &floor);
    QString houseNumber() const;
    void setHouseNumber(const QString &houseNumber);
    QString label() const;
    void setLabel(const QString &label);
    QString postalCode() const;
    void setPostalCode(const QString &postalCode);
    QString state() const;
    void setState(const QString &state);
    QString street() const;
    void setStreet(const QString &street);
    QString suite() const;
    void setSuite(const QString &suite);

private:
    QSharedDataPointer<QPlaceAddressPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEADDRESS_H
