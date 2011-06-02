#include "qplacecategory.h"
#include "qplacecategory_p.h"

#include "qplacemanagerengine.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceCategoryPrivate::QPlaceCategoryPrivate()
    : QSharedData()
{
}

QPlaceCategoryPrivate::QPlaceCategoryPrivate(const QPlaceCategoryPrivate &other)
    : QSharedData()
{
    this->alternativeNames = other.alternativeNames;
    this->categoryId = other.categoryId;
    this->categorySystemId = other.categorySystemId;
    this->description = other.description;
    this->name = other.name;
}

QPlaceCategoryPrivate::~QPlaceCategoryPrivate()
{
}

bool QPlaceCategoryPrivate::operator==(const QPlaceCategoryPrivate &other) const
{
    return (
            this->alternativeNames == other.alternativeNames
            && this->categoryId == other.categoryId
            && this->categorySystemId == other.categorySystemId
            && this->description == other.description
            && this->name == other.name
    );
}

/*!
    \class QPlaceCategory

    \inmodule QPlaces

    \brief The QPlaceCategory class represents a category object.

    Each QPlaceCategory represents a category object with a number of attributes
    such as description, name, id etc. Each QPlaceCategory is associated with place.

    Category objects are read-only, e.g. user of API might get business info object
    associated to specific place but can not edit its content. User might also create new
    contact object and add it to place.

    QPlaceCategory is an in memory representation of a contact object.
*/


/*
    Default constructor. Constructs a new category object.
*/
QPlaceCategory::QPlaceCategory()
    : d(new QPlaceCategoryPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceCategory::QPlaceCategory(const QPlaceCategory &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceCategory::~QPlaceCategory()
{
}

QPlaceCategory &QPlaceCategory::operator =(const QPlaceCategory &other) {
    d = other.d;
    return *this;
}

bool QPlaceCategory::operator==(const QPlaceCategory &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns alternative names.
*/
QList<QPlaceAlternativeValue> QPlaceCategory::alternativeNames() const
{
    return d->alternativeNames;
}

/*!
    Sets alternative names.
*/
void QPlaceCategory::setAlternativeNames(const QList<QPlaceAlternativeValue> &names)
{
    d->alternativeNames = names;
}

/*!
    Returns category id.
*/
QString QPlaceCategory::categoryId() const
{
    return d->categoryId;
}

/*!
    Sets description.
*/
void QPlaceCategory::setCategoryId(const QString &catID)
{
    d->categoryId = catID;
}

/*!
    Returns category system id.
*/
QString QPlaceCategory::categorySystemId() const
{
    return d->categorySystemId;
}

/*!
    Sets category system id.
*/
void QPlaceCategory::setCategorySystemId(const QString &catID)
{
    d->categorySystemId = catID;
}

/*!
    Returns description.
*/
QString QPlaceCategory::description() const
{
    return d->description;
}

/*!
    Sets category description.
*/
void QPlaceCategory::setDescription(const QString &description)
{
    d->description = description;
}

/*!
    Returns name of category.
*/
QString QPlaceCategory::name() const
{
    return d->name;
}

/*!
    Sets category name.
*/
void QPlaceCategory::setName(const QString &name)
{
    d->name = name;
}

/*!
    Returns true if category is empty.
*/
bool QPlaceCategory::isEmpty() const
{
    return !d->categoryId.isEmpty();
}
