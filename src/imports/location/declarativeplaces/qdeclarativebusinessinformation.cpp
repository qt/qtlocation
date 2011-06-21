#include "qdeclarativebusinessinformation_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass BusinessInformation

    \brief The BusinessInformation element holds business information data.
    \inherits QObject

    BusinessInformation cointains many properties holding data of the place like
    business features, openning hours, etc.

    \ingroup qml-places
*/

QDeclarativeBusinessInformation::QDeclarativeBusinessInformation(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeBusinessInformation::QDeclarativeBusinessInformation(const QPlaceBusinessInformation &src,
        QObject *parent)
        : QObject(parent),
          m_src(src)
{
    synchronizeFeatures();
    synchronizeOpeningHours();
    synchronizeAnnualClosings();
}

QDeclarativeBusinessInformation::~QDeclarativeBusinessInformation()
{
}

void QDeclarativeBusinessInformation::setBusinessInformation(const QPlaceBusinessInformation &src)
{
    QPlaceBusinessInformation previous = m_src;
    m_src = src;

    if (previous.additionalData() != m_src.additionalData()) {
        emit additionalDataChanged();
    }
    if (previous.features() != m_src.features()) {
        synchronizeFeatures();
        emit featuresChanged();
    }
    if (previous.openingHours() != m_src.openingHours()) {
        synchronizeOpeningHours();
        emit openingHoursChanged();
    }
    if (previous.annualClosings() != m_src.annualClosings()) {
        synchronizeAnnualClosings();
        emit annualClosingsChanged();
    }
    if (previous.annualClosingNote() != m_src.annualClosingNote()) {
        emit annualClosingNoteChanged();
    }
    if (previous.openingNote() != m_src.openingNote()) {
        emit openingNoteChanged();
    }
    if (previous.paymentMethods() != m_src.paymentMethods()) {
        emit paymentMethodsChanged();
    }
}

QPlaceBusinessInformation QDeclarativeBusinessInformation::businessInformation() const
{
    return m_src;
}

/*!
    \qmlproperty QVariantHash BusinessInformation::additionalData

    This property holds additional data for business information.
    Those are pairs of strings (key/value).
*/

void QDeclarativeBusinessInformation::setAdditionalData(const QVariantHash &additionalData)
{
    if (m_src.additionalData() != additionalData) {
        m_src.setAdditionalData(additionalData);
        emit additionalDataChanged();
    }
}

QVariantHash QDeclarativeBusinessInformation::additionalData() const
{
    return m_src.additionalData();
}

/*!
    \qmlproperty string BusinessInformation::annualClosingNote

    This property holds annual closing note.
*/
void QDeclarativeBusinessInformation::setAnnualClosingNote(const QString &data)
{
    if (m_src.annualClosingNote() != data) {
        m_src.setAnnualClosingNote(data);
        emit annualClosingNoteChanged();
    }
}

QString QDeclarativeBusinessInformation::annualClosingNote() const
{
    return m_src.annualClosingNote();
}

/*!
    \qmlproperty string BusinessInformation::openingNote

    This property holds oppening note.
*/
void QDeclarativeBusinessInformation::setOpeningNote(const QString &data)
{
    if (m_src.openingNote() != data) {
        m_src.setOpeningNote(data);
        emit annualClosingNoteChanged();
    }
}

QString QDeclarativeBusinessInformation::openingNote() const
{
    return m_src.openingNote();
}

/*!
    \qmlproperty stringlist BusinessInformation::paymentMethods

    This property holds payment methods list.
*/
void QDeclarativeBusinessInformation::setPaymentMethods(const QStringList &data)
{
    if (m_src.paymentMethods() != data) {
        m_src.setPaymentMethods(data);
        emit paymentMethodsChanged();
    }
}

QStringList QDeclarativeBusinessInformation::paymentMethods() const
{
    return m_src.paymentMethods();
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeBusinessFeature> BusinessInformation::features

    This property holds business featyres list.
*/
QDeclarativeListProperty<QDeclarativeBusinessFeature> QDeclarativeBusinessInformation::features()
{
    return QDeclarativeListProperty<QDeclarativeBusinessFeature>(this,
                                                          0, // opaque data parameter
                                                          features_append,
                                                          features_count,
                                                          features_at,
                                                          features_clear);
}

void QDeclarativeBusinessInformation::features_append(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop,
                                                  QDeclarativeBusinessFeature *value)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    QDeclarativeBusinessFeature *altValue = new QDeclarativeBusinessFeature(object);
    altValue->setBusinessFeature(value->businessFeature());
    object->m_features.append(altValue);
    QList<QPlaceBusinessFeature> list = object->m_src.features();
    list.append(value->businessFeature());
    object->m_src.setFeatures(list);
    emit object->featuresChanged();
}

int QDeclarativeBusinessInformation::features_count(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop)
{
    return static_cast<QDeclarativeBusinessInformation*>(prop->object)->m_features.count();
}

QDeclarativeBusinessFeature* QDeclarativeBusinessInformation::features_at(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop,
                                                                          int index)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    QDeclarativeBusinessFeature *res = NULL;
    if (object->m_features.count() > index && index > -1) {
        res = object->m_features[index];
    }
    return res;
}

void QDeclarativeBusinessInformation::features_clear(QDeclarativeListProperty<QDeclarativeBusinessFeature> *prop)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    qDeleteAll(object->m_features);
    object->m_features.clear();
    object->m_src.setFeatures(QList<QPlaceBusinessFeature>());
    emit object->featuresChanged();
}

void QDeclarativeBusinessInformation::synchronizeFeatures()
{
    qDeleteAll(m_features);
    m_features.clear();
    foreach (QPlaceBusinessFeature value, m_src.features()) {
        QDeclarativeBusinessFeature* declarativeValue = new QDeclarativeBusinessFeature(value, this);
        m_features.append(declarativeValue);
    }
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeWeekdayHours> BusinessInformation::openingHours

    This property opening hours list.
*/
QDeclarativeListProperty<QDeclarativeWeekdayHours> QDeclarativeBusinessInformation::openingHours()
{
    return QDeclarativeListProperty<QDeclarativeWeekdayHours>(this,
                                                          0, // opaque data parameter
                                                          openingHours_append,
                                                          openingHours_count,
                                                          openingHours_at,
                                                          openingHours_clear);
}

void QDeclarativeBusinessInformation::openingHours_append(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop,
                                                  QDeclarativeWeekdayHours *value)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    QDeclarativeWeekdayHours *altValue = new QDeclarativeWeekdayHours(object);
    altValue->setWeekdayHours(value->weekdayHours());
    object->m_openingHours.append(altValue);
    QList<QPlaceWeekdayHours> list = object->m_src.openingHours();
    list.append(value->weekdayHours());
    object->m_src.setOpeningHours(list);
    emit object->openingHoursChanged();
}

int QDeclarativeBusinessInformation::openingHours_count(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop)
{
    return static_cast<QDeclarativeBusinessInformation*>(prop->object)->m_openingHours.count();
}

QDeclarativeWeekdayHours* QDeclarativeBusinessInformation::openingHours_at(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop,
                                                                          int index)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    QDeclarativeWeekdayHours *res = NULL;
    if (object->m_openingHours.count() > index && index > -1) {
        res = object->m_openingHours[index];
    }
    return res;
}

void QDeclarativeBusinessInformation::openingHours_clear(QDeclarativeListProperty<QDeclarativeWeekdayHours> *prop)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    qDeleteAll(object->m_openingHours);
    object->m_openingHours.clear();
    object->m_src.setOpeningHours(QList<QPlaceWeekdayHours>());
    emit object->openingHoursChanged();
}

void QDeclarativeBusinessInformation::synchronizeOpeningHours()
{
    qDeleteAll(m_openingHours);
    m_openingHours.clear();
    foreach (QPlaceWeekdayHours value, m_src.openingHours()) {
        QDeclarativeWeekdayHours* declarativeValue = new QDeclarativeWeekdayHours(value, this);
        m_openingHours.append(declarativeValue);
    }
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativePeriod> BusinessInformation::annualClosings

    This property annual closings list.
*/
QDeclarativeListProperty<QDeclarativePeriod> QDeclarativeBusinessInformation::annualClosings()
{
    return QDeclarativeListProperty<QDeclarativePeriod>(this,
                                                          0, // opaque data parameter
                                                          annualClosings_append,
                                                          annualClosings_count,
                                                          annualClosings_at,
                                                          annualClosings_clear);
}

void QDeclarativeBusinessInformation::annualClosings_append(QDeclarativeListProperty<QDeclarativePeriod> *prop,
                                                  QDeclarativePeriod *value)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    QDeclarativePeriod *altValue = new QDeclarativePeriod(object);
    altValue->setPeriod(value->period());
    object->m_annualClosings.append(altValue);
    QList<QPlacePeriod> list = object->m_src.annualClosings();
    list.append(value->period());
    object->m_src.setAnnualClosings(list);
    emit object->annualClosingNoteChanged();
}

int QDeclarativeBusinessInformation::annualClosings_count(QDeclarativeListProperty<QDeclarativePeriod> *prop)
{
    return static_cast<QDeclarativeBusinessInformation*>(prop->object)->m_annualClosings.count();
}

QDeclarativePeriod* QDeclarativeBusinessInformation::annualClosings_at(QDeclarativeListProperty<QDeclarativePeriod> *prop,
                                                                          int index)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    QDeclarativePeriod *res = NULL;
    if (object->m_annualClosings.count() > index && index > -1) {
        res = object->m_annualClosings[index];
    }
    return res;
}

void QDeclarativeBusinessInformation::annualClosings_clear(QDeclarativeListProperty<QDeclarativePeriod> *prop)
{
    QDeclarativeBusinessInformation* object = static_cast<QDeclarativeBusinessInformation*>(prop->object);
    qDeleteAll(object->m_annualClosings);
    object->m_annualClosings.clear();
    object->m_src.setAnnualClosings(QList<QPlacePeriod>());
    emit object->annualClosingNoteChanged();
}

void QDeclarativeBusinessInformation::synchronizeAnnualClosings()
{
    qDeleteAll(m_annualClosings);
    m_annualClosings.clear();
    foreach (QPlacePeriod value, m_src.annualClosings()) {
        QDeclarativePeriod* declarativeValue = new QDeclarativePeriod(value, this);
        m_annualClosings.append(declarativeValue);
    }
}
