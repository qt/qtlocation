#include "qplacegeoboundingbox.h"
#include "qplacegeoboundingbox_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceGeoBoundingBoxPrivate::QPlaceGeoBoundingBoxPrivate()
    : QSharedData()
{
}

QPlaceGeoBoundingBoxPrivate::QPlaceGeoBoundingBoxPrivate(const QPlaceGeoBoundingBoxPrivate &other)
    : QSharedData()
{
    this->topLeft = other.topLeft;
    this->bottomRight = other.bottomRight;
}

QPlaceGeoBoundingBoxPrivate::~QPlaceGeoBoundingBoxPrivate()
{
}

bool QPlaceGeoBoundingBoxPrivate::operator==(const QPlaceGeoBoundingBoxPrivate &other) const
{
    return (
            this->topLeft == other.topLeft
            && this->bottomRight == other.bottomRight
    );
}

/*!
    \class QPlaceGeoBoundingBox

    \inmodule QPlaces

    \brief The QPlaceGeoBoundingBox class represents a bounding box object.

    Each QPlaceGeoBoundingBox represents a bounding box with a top/left and bottom/right point.
*/

/*!
    Constructs a new position object.
*/
QPlaceGeoBoundingBox::QPlaceGeoBoundingBox()
    : d(new QPlaceGeoBoundingBoxPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceGeoBoundingBox::QPlaceGeoBoundingBox(const QPlaceGeoBoundingBox &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceGeoBoundingBox::~QPlaceGeoBoundingBox()
{
}

QPlaceGeoBoundingBox &QPlaceGeoBoundingBox::operator =(const QPlaceGeoBoundingBox &other) {
    d = other.d;
    return *this;
}

bool QPlaceGeoBoundingBox::operator==(const QPlaceGeoBoundingBox &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}


/*!
    Returns coordinate of top left edge of bounding box.
*/
QPlaceGeoCoordinate QPlaceGeoBoundingBox::topLeft() const
{
    return d->topLeft;
}

/*!
    Sets \a coordinate of top left edge of bounding box.
*/
void QPlaceGeoBoundingBox::setTopLeft(const QPlaceGeoCoordinate &coordinate)
{
    d->topLeft = coordinate;
}

/*!
    Returns coordinate of bottom right edge of bounding box.
*/
QPlaceGeoCoordinate QPlaceGeoBoundingBox::bottomRight() const
{
    return d->bottomRight;
}

/*!
    Sets \a coordinate of bottom right edge of bounding box.
*/
void QPlaceGeoBoundingBox::setBottomRight(const QPlaceGeoCoordinate &coordinate)
{
    d->bottomRight = coordinate;
}

/*!
    Returns true if bounding box is valid.
*/
bool QPlaceGeoBoundingBox::isValid()
{
    return (d->bottomRight.isValid() && d->topLeft.isValid());
}
