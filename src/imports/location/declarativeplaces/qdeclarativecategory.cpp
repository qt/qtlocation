#include "qdeclarativecategory_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Category

    \brief The Category element holds various positional data, such as \l
    latitude and \l longitude and.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeCategory::QDeclarativeCategory(QObject* parent)
        : QObject(parent) {}

QDeclarativeCategory::QDeclarativeCategory(const QPlaceCategory &category,
        QObject *parent)
        : QObject(parent),
        m_category(category)
{
    synchronizeAlternativeValues();
}

QDeclarativeCategory::~QDeclarativeCategory() {}

void QDeclarativeCategory::setCategory(const QPlaceCategory &category)
{
    QPlaceCategory previous = m_category;
    m_category = category;

    if (category.name() != previous.name()) {
        emit nameChanged();
    }
    if (category.categoryId() != previous.categoryId()) {
        emit categoryIdChanged();
    }
    if (category.description() != previous.description()) {
        emit descriptionChanged();
    }
    if (category.alternativeNames() != previous.alternativeNames()) {
        synchronizeAlternativeValues();
        emit alternativeNamesChanged();
    }
}

QPlaceCategory QDeclarativeCategory::category()
{
    QList<QPlaceAlternativeValue> list;
    foreach (QDeclarativeAlternativeValue *value, m_alternativeValues) {
        list.append(value->valueObject());
    }
    m_category.setAlternativeNames(list);

    return m_category;
}

/*!
    \qmlproperty string Category::categoryId

    This property holds the id of the category
*/

void QDeclarativeCategory::setCategoryId(const QString &id)
{
    if (m_category.categoryId() != id) {
        m_category.setCategoryId(id);
        emit categoryIdChanged();
    }
}

QString QDeclarativeCategory::categoryId() const
{
    return m_category.categoryId();
}

/*!
    \qmlproperty string Category::description

    This property holds the description of the category
*/

void QDeclarativeCategory::setDescription(const QString &description)
{
    if (m_category.description() != description) {
        m_category.setDescription(description);
        emit descriptionChanged();
    }
}

QString QDeclarativeCategory::description() const
{
    return m_category.description();
}

/*!
    \qmlproperty string Category::name

    This property holds name of the category
*/

void QDeclarativeCategory::setName(const QString &name)
{
    if (m_category.name() != name) {
        m_category.setName(name);
        emit nameChanged();
    }
}

QString QDeclarativeCategory::name() const
{
    return m_category.name();
}

/*!
    \qmlproperty QVariantHash Address::alternativeAttributes

    This property alternative values for label property.
    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeListProperty<QDeclarativeAlternativeValue> QDeclarativeCategory::alternativeNames()
{
    return QDeclarativeListProperty<QDeclarativeAlternativeValue>(this,
                                                          0, // opaque data parameter
                                                          alternativeValue_append,
                                                          alternativeValue_count,
                                                          alternativeValue_at,
                                                          alternativeValue_clear);
}

void QDeclarativeCategory::alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                  QDeclarativeAlternativeValue *value)
{
    QDeclarativeCategory* object = static_cast<QDeclarativeCategory*>(prop->object);
    QDeclarativeAlternativeValue *altValue = new QDeclarativeAlternativeValue(object);
    altValue->setValueObject(value->valueObject());
    object->m_alternativeValues.append(altValue);
    QList<QPlaceAlternativeValue> list = object->m_category.alternativeNames();
    list.append(value->valueObject());
    object->m_category.setAlternativeNames(list);
    emit object->alternativeNamesChanged();
}

int QDeclarativeCategory::alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    return static_cast<QDeclarativeCategory*>(prop->object)->m_alternativeValues.count();
}

QDeclarativeAlternativeValue* QDeclarativeCategory::alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                                                          int index)
{
    QDeclarativeCategory* object = static_cast<QDeclarativeCategory*>(prop->object);
    QDeclarativeAlternativeValue *res = NULL;
    if (object->m_alternativeValues.count() > index && index > -1) {
        res = object->m_alternativeValues[index];
    }
    return res;
}

void QDeclarativeCategory::alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop)
{
    QDeclarativeCategory* object = static_cast<QDeclarativeCategory*>(prop->object);
    qDeleteAll(object->m_alternativeValues);
    object->m_alternativeValues.clear();
    object->m_category.setAlternativeNames(QList<QPlaceAlternativeValue>());
    emit object->alternativeNamesChanged();
}

void QDeclarativeCategory::synchronizeAlternativeValues()
{
    qDeleteAll(m_alternativeValues);
    m_alternativeValues.clear();
    foreach (QPlaceAlternativeValue value, m_category.alternativeNames()) {
        QDeclarativeAlternativeValue* declarativeValue = new QDeclarativeAlternativeValue(value, this);
        m_alternativeValues.append(declarativeValue);
    }
}
