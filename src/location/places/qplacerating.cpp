#include "qplacerating.h"
#include "qplacerating_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceRatingPrivate::QPlaceRatingPrivate() : QSharedData()
{
}

QPlaceRatingPrivate::QPlaceRatingPrivate(const QPlaceRatingPrivate &other)
    : QSharedData()
{
    this->value = other.value;
    this->count = other.count;
}

QPlaceRatingPrivate::~QPlaceRatingPrivate()
{
}

bool QPlaceRatingPrivate::operator==(const QPlaceRatingPrivate &other) const
{
    return (
            this->value == other.value
            && this->count == other.count
    );
}

/*!
    \class QPlaceRating

    \inmodule QPlaces

    \brief The QPlaceRating class represents a rating object.

    Each QPlaceRating represents a rating object with a count and value.

    QPlaceRating is an in memory representation of a rating object.
*/

/*!
    Constructs a new rating object.
*/
QPlaceRating::QPlaceRating()
    : d(new QPlaceRatingPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceRating::QPlaceRating(const QPlaceRating &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceRating::~QPlaceRating()
{
}

QPlaceRating &QPlaceRating::operator =(const QPlaceRating &other) {
    d = other.d;
    return *this;
}

bool QPlaceRating::operator==(const QPlaceRating &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns value.
*/
double QPlaceRating::value() const
{
    return d->value;
}

/*!
    Sets value.
*/
void QPlaceRating::setValue(const double &src)
{
    d->value = src;
}

/*!
    Returns count.
*/
uint QPlaceRating::count() const
{
    return d->count;
}

/*!
    Sets count.
*/
void QPlaceRating::setCount(const uint &src)
{
    d->count = src;
}
