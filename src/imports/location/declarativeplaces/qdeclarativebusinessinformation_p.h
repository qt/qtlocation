#ifndef QDECLARATIVEBUSINESSINFORMATION_P_H
#define QDECLARATIVEBUSINESSINFORMATION_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplacebusinessinformation.h>
#include "qdeclarativebusinessfeature_p.h"
#include "qdeclarativeperiod_p.h"
#include "qdeclarativeweekdayhours_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeBusinessInformation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantHash additionalData READ additionalData WRITE setAdditionalData NOTIFY additionalDataChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeBusinessFeature> features READ features NOTIFY featuresChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeWeekdayHours> openingHours READ openingHours NOTIFY openingHoursChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativePeriod> annualClosings READ annualClosings NOTIFY annualClosingsChanged)
    Q_PROPERTY(QString annualClosingNote READ annualClosingNote WRITE setAnnualClosingNote NOTIFY annualClosingNoteChanged);
    Q_PROPERTY(QString openingNote READ openingNote WRITE setOpeningNote NOTIFY openingNoteChanged);
    Q_PROPERTY(QStringList paymentMethods READ paymentMethods WRITE setPaymentMethods NOTIFY paymentMethodsChanged);

public:
    explicit QDeclarativeBusinessInformation(QObject* parent = 0);
    explicit QDeclarativeBusinessInformation(const QPlaceBusinessInformation &src,
                                             QObject* parent = 0);
    ~QDeclarativeBusinessInformation();

    QPlaceBusinessInformation businessInformation() const;
    void setBusinessInformation(const QPlaceBusinessInformation &src);

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QDeclarativeListProperty<QDeclarativeBusinessFeature> features();
    static void features_append(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop,
                                  QDeclarativeBusinessFeature* value);
    static int features_count(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop);
    static QDeclarativeBusinessFeature* features_at(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop, int index);
    static void features_clear(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop);
    QDeclarativeListProperty<QDeclarativePeriod> annualClosings();
    static void annualClosings_append(QDeclarativeListProperty<QDeclarativePeriod> *prop,
                                  QDeclarativePeriod* value);
    static int annualClosings_count(QDeclarativeListProperty<QDeclarativePeriod> *prop);
    static QDeclarativePeriod* annualClosings_at(QDeclarativeListProperty<QDeclarativePeriod> *prop, int index);
    static void annualClosings_clear(QDeclarativeListProperty<QDeclarativePeriod> *prop);
    QDeclarativeListProperty<QDeclarativeWeekdayHours> openingHours();
    static void openingHours_append(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop,
                                  QDeclarativeWeekdayHours* value);
    static int openingHours_count(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop);
    static QDeclarativeWeekdayHours* openingHours_at(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop, int index);
    static void openingHours_clear(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop);
    QString annualClosingNote() const;
    void setAnnualClosingNote(const QString &data);
    QString openingNote() const;
    void setOpeningNote(const QString &data);
    QStringList paymentMethods() const;
    void setPaymentMethods(const QStringList &data);

signals:
    void additionalDataChanged();
    void featuresChanged();
    void openingHoursChanged();
    void annualClosingsChanged();
    void annualClosingNoteChanged();
    void openingNoteChanged();
    void paymentMethodsChanged();

private:
    void synchronizeFeatures();
    void synchronizeOpeningHours();
    void synchronizeAnnualClosings();

private:
    QList<QDeclarativeBusinessFeature*> m_features;
    QList<QDeclarativePeriod*> m_annualClosings;
    QList<QDeclarativeWeekdayHours*> m_openingHours;

    QPlaceBusinessInformation m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeBusinessInformation));

#endif // QDECLARATIVEBUSINESSINFORMATION_P_H
