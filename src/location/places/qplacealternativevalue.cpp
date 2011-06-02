#include "qplacealternativevalue.h"
#include "qplacealternativevalue_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceAlternativeValuePrivate::QPlaceAlternativeValuePrivate() : QSharedData()
{
}

QPlaceAlternativeValuePrivate::QPlaceAlternativeValuePrivate(const QPlaceAlternativeValuePrivate &other)
    : QSharedData()
{
    this->key = other.key;
    this->language = other.language;
    this->semantics = other.semantics;
    this->type = other.type;
    this->value = other.value;
}

QPlaceAlternativeValuePrivate::~QPlaceAlternativeValuePrivate()
{
}

bool QPlaceAlternativeValuePrivate::operator==(const QPlaceAlternativeValuePrivate &other) const
{
    return (
            this->key == other.key
            && this->language == other.language
            && this->semantics == other.semantics
            && this->type == other.type
            && this->value == other.value
    );
}

/*!
    \class QPlaceAlternativeValue

    \inmodule QPlaces

    \brief The QPlaceAlternativeValue class represents a alternative value object.

    Each QPlaceAlternativeValue represents a alternative value object with a number of attributes
    such as value, language id etc. Each QPlaceAlternativeValue is associated with place.

    Alternative value objects are read-only, e.g. user of API might get alternative value object
    associated to specific place but can not edit its content. User might also create new
    alternative value object and add it to place.

    QPlaceAlternativeValue is an in memory representation of a alternative value object.
*/

/*!
    Constructs an new alternative value object.
*/
QPlaceAlternativeValue::QPlaceAlternativeValue()
    : d(new QPlaceAlternativeValuePrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceAlternativeValue::QPlaceAlternativeValue(const QPlaceAlternativeValue &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceAlternativeValue::~QPlaceAlternativeValue()
{
}

QPlaceAlternativeValue &QPlaceAlternativeValue::operator =(const QPlaceAlternativeValue &other) {
    d = other.d;
    return *this;
}

bool QPlaceAlternativeValue::operator==(const QPlaceAlternativeValue &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns key.
*/
QString QPlaceAlternativeValue::key() const
{
    return d->key;
}

/*!
    Sets key.
*/
void QPlaceAlternativeValue::setKey(const QString &data)
{
    d->key = data;
}

/*!
    Returns language.
*/
QString QPlaceAlternativeValue::language() const
{
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceAlternativeValue::setLanguage(const QString &data)
{
    d->language = data;
}

/*!
    Returns semantics.
*/
QPlaceAlternativeValue::TextSemantics QPlaceAlternativeValue::semantics() const
{
    return d->semantics;
}

/*!
    Sets semantics.
*/
void QPlaceAlternativeValue::setSemantics(const QPlaceAlternativeValue::TextSemantics &data)
{
    d->semantics = data;
}

/*!
    Returns type.
*/
QPlaceAlternativeValue::NameType QPlaceAlternativeValue::type() const
{
    return d->type;
}

/*!
    Sets type.
*/
void QPlaceAlternativeValue::setType(const QPlaceAlternativeValue::NameType &data)
{
    d->type = data;
}

/*!
    Returns value.
*/
QString QPlaceAlternativeValue::value() const
{
    return d->value;
}

/*!
    Sets value.
*/
void QPlaceAlternativeValue::setValue(const QString &data)
{
    d->value = data;
}
