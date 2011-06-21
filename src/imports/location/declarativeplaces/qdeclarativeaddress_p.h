#ifndef QDECLARATIVEADDRESS_P_H
#define QDECLARATIVEADDRESS_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplaceaddress.h>
#include "qdeclarativealternativevalue_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeAddress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantHash additionalData READ additionalData WRITE setAdditionalData NOTIFY additionalDataChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeAttributes READ alternativeAttributes NOTIFY alternativeAttributesChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged);
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged);
    Q_PROPERTY(QString countryName READ countryName WRITE setCountryName NOTIFY countryNameChanged);
    Q_PROPERTY(QString county READ county WRITE setCounty NOTIFY countyChanged);
    Q_PROPERTY(QString district READ district WRITE setDistrict NOTIFY districtChanged);
    Q_PROPERTY(QString floor READ floor WRITE setFloor NOTIFY floorChanged);
    Q_PROPERTY(QString houseNumber READ houseNumber WRITE setHouseNumber NOTIFY houseNumberChanged);
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged);
    Q_PROPERTY(QString postalCode READ postalCode WRITE setPostalCode NOTIFY postalCodeChanged);
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged);
    Q_PROPERTY(QString street READ street WRITE setStreet NOTIFY streetChanged);
    Q_PROPERTY(QString suite READ suite WRITE setSuite NOTIFY suiteChanged);

public:
    explicit QDeclarativeAddress(QObject* parent = 0);
    explicit QDeclarativeAddress(const QPlaceAddress &src, QObject* parent = 0);
    ~QDeclarativeAddress();

    QPlaceAddress address() const;
    void setAddress(const QPlaceAddress &src);

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeAttributes();
    static void alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                  QDeclarativeAlternativeValue* value);
    static int alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);
    static QDeclarativeAlternativeValue* alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop, int index);
    static void alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);

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

signals:
    void additionalDataChanged();
    void alternativeAttributesChanged();
    void cityChanged();
    void countryChanged();
    void countryNameChanged();
    void countyChanged();
    void districtChanged();
    void floorChanged();
    void houseNumberChanged();
    void labelChanged();
    void postalCodeChanged();
    void stateChanged();
    void streetChanged();
    void suiteChanged();

private:
    void synchronizeAlternativeValues();

private:
    QList<QDeclarativeAlternativeValue*> m_alternativeValues;
    QPlaceAddress m_src;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeAddress));

#endif // QDECLARATIVEADDRESS_P_H
