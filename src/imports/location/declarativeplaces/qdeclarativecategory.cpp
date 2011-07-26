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
}

QPlaceCategory QDeclarativeCategory::category()
{
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
