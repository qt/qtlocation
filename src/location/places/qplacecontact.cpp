#include "qplacecontact.h"
#include "qplacecontact_p.h"

QTM_USE_NAMESPACE

QPlaceContactPrivate::QPlaceContactPrivate() : QSharedData()
{
    this->type = QPlaceContact::Undefined;
}

QPlaceContactPrivate::QPlaceContactPrivate(const QPlaceContactPrivate &other)
    : QSharedData()
{
    this->description = other.description;
    this->type = other.type;
    this->value = other.value;
}

QPlaceContactPrivate::~QPlaceContactPrivate()
{
}

bool QPlaceContactPrivate::operator==(const QPlaceContactPrivate &other) const
{
    return (
            this->description == other.description
            && this->type == other.type
            && this->value == other.value
    );
}

/*!
    \class QPlaceContact

    \inmodule QPlaces

    \brief The QPlaceContact class represents a contact object.

    Each QPlaceContact represents a contact object with a number of attributes
    such as type, data etc. Each QPlaceContact is associated with place.

    Contact objects are read-only, e.g. user of API might get contact object
    associated to specific place but can not edit its content. User might also create new
    contact object and add it to place.

    QPlaceContact is an in memory representation of a contact object.
*/

/*!
    Constructs an new contact object.
*/
QPlaceContact::QPlaceContact()
    : d(new QPlaceContactPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceContact::QPlaceContact(const QPlaceContact &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceContact::~QPlaceContact()
{
}

QPlaceContact &QPlaceContact::operator =(const QPlaceContact &other) {
    d = other.d;
    return *this;
}

bool QPlaceContact::operator==(const QPlaceContact &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns description.
*/
QString QPlaceContact::description() const
{
    return d->description;
}

/*!
    Sets description.
*/
void QPlaceContact::setDescription(const QString &data)
{
    d->description = data;
}

/*!
    Returns type.
*/
QPlaceContact::ContactType QPlaceContact::type() const
{
    return d->type;
}

/*!
    Sets type.
*/
void QPlaceContact::setType(const QPlaceContact::ContactType &data)
{
    d->type = data;
}

/*!
    Returns value.
*/
QString QPlaceContact::value() const
{
    return d->value;
}

/*!
    Sets value.
*/
void QPlaceContact::setValue(const QString &data)
{
    d->value = data;
}
