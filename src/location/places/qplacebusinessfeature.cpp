#include "qplacebusinessfeature.h"
#include "qplacebusinessfeature_p.h"

QTM_USE_NAMESPACE

QPlaceBusinessFeaturePrivate::QPlaceBusinessFeaturePrivate() : QSharedData()
{
}

QPlaceBusinessFeaturePrivate::QPlaceBusinessFeaturePrivate(const QPlaceBusinessFeaturePrivate &other)
    : QSharedData()
{
    this->value = other.value;
    this->key = other.key;
    this->label = other.label;
}

QPlaceBusinessFeaturePrivate::~QPlaceBusinessFeaturePrivate()
{
}

bool QPlaceBusinessFeaturePrivate::operator==(const QPlaceBusinessFeaturePrivate &other) const
{
    return (
            this->value == other.value
            && this->key == other.key
            && this->label == other.label
    );
}

/*!
    \class QPlaceBusinessFeature

    \inmodule QPlaces

    \brief The QPlaceBusinessFeature class represents a business services object.

    Each QPlaceBusinessFeature represents a business service object with a number of attributes
    such as key etc.
*/


/*!
    Constructs an new "empty" business feature object.
*/
QPlaceBusinessFeature::QPlaceBusinessFeature()
    : d(new QPlaceBusinessFeaturePrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceBusinessFeature::QPlaceBusinessFeature(const QPlaceBusinessFeature &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceBusinessFeature::~QPlaceBusinessFeature()
{
}

QPlaceBusinessFeature &QPlaceBusinessFeature::operator =(const QPlaceBusinessFeature &other) {
    d = other.d;
    return *this;
}

bool QPlaceBusinessFeature::operator==(const QPlaceBusinessFeature &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns key.
*/
QString QPlaceBusinessFeature::key() const
{
    return d->key;
}

/*!
    Sets key.
*/
void QPlaceBusinessFeature::setKey(const QString &data)
{
    d->key = data;
}

/*!
    Returns label.
*/
QString QPlaceBusinessFeature::label() const
{
    return d->label;
}

/*!
    Sets label.
*/
void QPlaceBusinessFeature::setLabel(const QString &data)
{
    d->label = data;
}

/*!
    Returns value.
*/
QString QPlaceBusinessFeature::value() const
{
    return d->value;
}

/*!
    Sets value.
*/
void QPlaceBusinessFeature::setValue(const QString &data)
{
    d->value = data;
}
