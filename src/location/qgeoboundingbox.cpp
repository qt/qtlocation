/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeoboundingbox.h"
#include "qgeoboundingbox_p.h"

#include "qgeocoordinate.h"
#include "qnumeric.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingBox
    \brief The QGeoBoundingBox class defines a rectangular geographic area.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps

    The rectangle is defined in terms of a QGeoCoordinate which specifies the
    top left coordinate of the rectangle and a QGeoCoordinate which specifies
    the bottom right coordinate of the rectangle.

    A bounding box is considered invalid if the top left or bottom right
    coordinates are invalid or if the top left coordinate is south of the
    bottom right coordinate.

    Bounding boxes can never cross the poles.

    Several methods behave as though the bounding box is defined in terms of a
    center coordinate, the width of the bounding box in degrees and the height
    of the bounding box in degrees.

    If the height or center of a bounding box is adjusted such that it would
    cross one of the poles the height is modified such that the bounding box
    touches but does not cross the pole and that the center coordinate is still
    in the center of the bounding box.
*/

/*!
    Constructs a new, invalid bounding box.
*/
QGeoBoundingBox::QGeoBoundingBox()
        : d_ptr(new QGeoBoundingBoxPrivate()) {}

/*!
    Constructs a new bounding box centered at \a center with a
    width in degrees of \a degreesWidth and a height in degrees of \a degreesHeight.

    If \a degreesHeight would take the bounding box beyond one of the poles,
    the height of the bounding box will be truncated such that the bounding box
    only extends up to the pole. The center of the bounding box will be
    unchanged, and the height will be adjusted such that the center point is at
    the center of the truncated bounding box.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &center, double degreesWidth, double degreesHeight)
        : d_ptr(new QGeoBoundingBoxPrivate(center, center))
{
        setWidth(degreesWidth);
        setHeight(degreesHeight);
}

/*!
    Constructs a new bounding box with a top left coordinate \a
    topLeft and a bottom right coordinate \a bottomRight.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
        : d_ptr(new QGeoBoundingBoxPrivate(topLeft, bottomRight)) {}

/*!
    Constructs a bounding box from the contents of \a other.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoBoundingBox &other)
        : QGeoBoundingArea(other),
          d_ptr(other.d_ptr) {}

/*!
    Destroys this bounding box.
*/
QGeoBoundingBox::~QGeoBoundingBox() {}

/*!
    Assigns \a other to this bounding box and returns a reference to this
    bounding box.
*/
QGeoBoundingBox& QGeoBoundingBox::operator = (const QGeoBoundingBox & other)
{
    QGeoBoundingArea::operator=(other);
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this bounding box is equal to \a other.
*/
bool QGeoBoundingBox::operator == (const QGeoBoundingBox &other) const
{
    return ((*(d_ptr.constData()) == *(other.d_ptr.constData())));
}

/*!
    Returns whether this bounding box is not equal to \a other.
*/
bool QGeoBoundingBox::operator != (const QGeoBoundingBox &other) const
{
    return !(this->operator==(other));
}

/*!
    Returns QGeoBoundingArea::BoxType to identify this as
    a QGeoBoundingBox instance.

    This function is provided to help find the specific type of
    aQGeoBoundingArea instance.
*/
QGeoBoundingArea::AreaType QGeoBoundingBox::type() const
{
    return QGeoBoundingArea::BoxType;
}

/*!
    Returns whether this bounding box is valid.

    A valid bounding box has valid top left and bottom right coordinates and
    has a top left coordinate with latitude greater than or equal to the
    latitude of the bottom right coordinate.
*/
bool QGeoBoundingBox::isValid() const
{
    return (d_ptr->topLeft.isValid()
            && d_ptr->bottomRight.isValid()
            && d_ptr->topLeft.latitude() >= d_ptr->bottomRight.latitude());
}

/*!
    Returns whether this bounding box has a geometrical area of 0.

    Returns true if this bounding box is invalid.
*/
bool QGeoBoundingBox::isEmpty() const
{
    return (!isValid()
            || (d_ptr->topLeft.latitude() == d_ptr->bottomRight.latitude())
            || (d_ptr->topLeft.longitude() == d_ptr->bottomRight.longitude()));
}

/*!
    Sets the top left coordinate of this bounding box to \a topLeft.
*/
void QGeoBoundingBox::setTopLeft(const QGeoCoordinate &topLeft)
{
    d_ptr->topLeft = topLeft;
}

/*!
    Returns the top left coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::topLeft() const
{
    return d_ptr->topLeft;
}

/*!
    Sets the top right coordinate of this bounding box to \a topRight.
*/
void QGeoBoundingBox::setTopRight(const QGeoCoordinate &topRight)
{
    d_ptr->topLeft.setLatitude(topRight.latitude());
    d_ptr->bottomRight.setLongitude(topRight.longitude());
}

/*!
    Returns the top right coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::topRight() const
{
    // TODO remove?
    if (!isValid())
        return QGeoCoordinate();

    return QGeoCoordinate(d_ptr->topLeft.latitude(), d_ptr->bottomRight.longitude());
}

/*!
    Sets the bottom left coordinate of this bounding box to \a bottomLeft.
*/
void QGeoBoundingBox::setBottomLeft(const QGeoCoordinate &bottomLeft)
{
    d_ptr->bottomRight.setLatitude(bottomLeft.latitude());
    d_ptr->topLeft.setLongitude(bottomLeft.longitude());
}

/*!
    Returns the bottom left coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::bottomLeft() const
{
    // TODO remove?
    if (!isValid())
        return QGeoCoordinate();

    return QGeoCoordinate(d_ptr->bottomRight.latitude(), d_ptr->topLeft.longitude());
}

/*!
    Sets the bottom right coordinate of this bounding box to \a bottomRight.
*/
void QGeoBoundingBox::setBottomRight(const QGeoCoordinate &bottomRight)
{
    d_ptr->bottomRight = bottomRight;
}

/*!
    Returns the bottom right coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::bottomRight() const
{
    return d_ptr->bottomRight;
}

/*!
    Sets the center of this bounding box to \a center.

    If this causes the bounding box to cross on of the poles the height of the
    bounding box will be truncated such that the bounding box only extends up
    to the pole. The center of the bounding box will be unchanged, and the
    height will be adjusted such that the center point is at the center of the
    truncated bounding box.
*/
void QGeoBoundingBox::setCenter(const QGeoCoordinate &center)
{
    double width = this->width();
    double height = this->height();

    double tlLat = center.latitude() + height / 2.0;
    double tlLon = center.longitude() - width / 2.0;
    double brLat = center.latitude() - height / 2.0;
    double brLon = center.longitude() + width / 2.0;

    if (tlLon < -180.0)
        tlLon += 360.0;
    if (tlLon > 180.0)
        tlLon -= 360.0;

    if (brLon < -180.0)
        brLon += 360.0;
    if (brLon > 180.0)
        brLon -= 360.0;

    if (tlLat > 90.0) {
        brLat = 2* center.latitude() - 90.0;
        tlLat = 90.0;
    }

    if (tlLat < -90.0) {
        brLat = -90.0;
        tlLat = -90.0;
    }

    if (brLat > 90.0) {
        tlLat = 90.0;
        brLat = 90.0;
    }

    if (brLat < -90.0) {
        tlLat = 2 * center.latitude() + 90.0;
        brLat = -90.0;
    }

    if (width == 360.0) {
        tlLon = -180.0;
        brLon = 180.0;
    }

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
    Returns the center of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::center() const
{
    if (!isValid())
        return QGeoCoordinate();

    double cLat = (d_ptr->topLeft.latitude() + d_ptr->bottomRight.latitude()) / 2.0;

    double cLon = (d_ptr->bottomRight.longitude() + d_ptr->topLeft.longitude()) / 2.0;
    if (d_ptr->topLeft.longitude() > d_ptr->bottomRight.longitude()) {
        cLon = cLon - 180.0;
    }

    if (cLon < -180.0)
        cLon += 360.0;
    if (cLon > 180.0)
        cLon -= 360.0;

    return QGeoCoordinate(cLat, cLon);
}

/*!
    Sets the width of this bounding box in degrees to \a degreesWidth.

    If \a degreesWidth is less than 0.0 or if this bounding box is invalid this
    function does nothing.  To set up the values of an invalid
    QGeoBoundingBox based on the center, width and height you should use
    setCenter() first in order to make the QGeoBoundingBox valid.

    If \a degreesWidth is greater than 360.0 then 360.0 is used as the width,
    the leftmost longitude of the bounding box is set to -180.0 degrees and the
    rightmost longitude of the bounding box is set to 180.0 degrees.
*/
void QGeoBoundingBox::setWidth(double degreesWidth)
{
    if (!isValid())
        return;

    if (degreesWidth < 0.0)
        return;

    if (degreesWidth >= 360.0) {
        d_ptr->topLeft.setLongitude(-180.0);
        d_ptr->bottomRight.setLongitude(180.0);
        return;
    }

    double tlLat = d_ptr->topLeft.latitude();
    double brLat = d_ptr->bottomRight.latitude();

    QGeoCoordinate c = center();

    double tlLon = c.longitude() - degreesWidth / 2.0;

    if (tlLon < -180.0)
        tlLon += 360.0;
    if (tlLon > 180.0)
        tlLon -= 360.0;

    double brLon = c.longitude() + degreesWidth / 2.0;

    if (brLon < -180.0)
        brLon += 360.0;
    if (brLon > 180.0)
        brLon -= 360.0;

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
    Returns the width of this bounding box in degrees.

    The return value is undefined if this bounding box is invalid.
*/
double QGeoBoundingBox::width() const
{
    if (!isValid())
        return qQNaN();

    double result = d_ptr->bottomRight.longitude() - d_ptr->topLeft.longitude();
    if (result < 0.0)
        result += 360.0;
    if (result > 360.0)
        result -= 360.0;

    return result;
}

/*!
    Sets the height of this bounding box in degrees to \a degreesHeight.

    If \a degreesHeight is less than 0.0 or if this bounding box is invalid
    this function does nothing. To set up the values of an invalid
    QGeoBoundingBox based on the center, width and height you should use
    setCenter() first in order to make the QGeoBoundingBox valid.

    If the change in height would cause the bounding box to cross a pole
    the height is adjusted such that the bounding box only touches the pole.

    This changes is done such that the center coordinate is still at the
    center of the bounding box, which may result in a bounding box with
    a smaller height than might otherwise be expected.

    If \a degreesHeight is greater than 180.0 then 180.0 is used as the height.
*/
void QGeoBoundingBox::setHeight(double degreesHeight)
{
    if (!isValid())
        return;

    if (degreesHeight < 0.0)
        return;

    if (degreesHeight >= 180.0) {
        degreesHeight = 180.0;
    }

    double tlLon = d_ptr->topLeft.longitude();
    double brLon = d_ptr->bottomRight.longitude();

    QGeoCoordinate c = center();

    double tlLat = c.latitude() + degreesHeight / 2.0;
    double brLat = c.latitude() - degreesHeight / 2.0;

    if (tlLat > 90.0) {
        brLat = 2* c.latitude() - 90.0;
        tlLat = 90.0;
    }

    if (tlLat < -90.0) {
        brLat = -90.0;
        tlLat = -90.0;
    }

    if (brLat > 90.0) {
        tlLat = 90.0;
        brLat = 90.0;
    }

    if (brLat < -90.0) {
        tlLat = 2 * c.latitude() + 90.0;
        brLat = -90.0;
    }

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
    Returns the height of this bounding box in degrees.

    The return value is undefined if this bounding box is invalid.
*/
double QGeoBoundingBox::height() const
{
    if (!isValid())
        return qQNaN();

    double result = d_ptr->topLeft.latitude() - d_ptr->bottomRight.latitude();
    if (result < 0.0)
        result = qQNaN();
    return result;
}

/*!
    Returns whether the coordinate \a coordinate is contained within this
    bounding box.
*/
bool QGeoBoundingBox::contains(const QGeoCoordinate &coordinate) const
{

    if (!isValid() || !coordinate.isValid())
        return false;

    double left = d_ptr->topLeft.longitude();
    double right = d_ptr->bottomRight.longitude();
    double top = d_ptr->topLeft.latitude();
    double bottom = d_ptr->bottomRight.latitude();

    double lon = coordinate.longitude();
    double lat = coordinate.latitude();

    if (lat > top)
        return false;
    if (lat < bottom)
        return false;

    if ((lat == 90.0) && (top == 90.0))
        return true;

    if ((lat == -90.0) && (bottom == -90.0))
        return true;

    if (left <= right) {
        if ((lon < left) || (lon > right))
            return false;
    } else {
        if ((lon < left) && (lon > right))
            return false;
    }

    return true;
}

/*!
    Returns whether the bounding box \a boundingBox is contained within this
    bounding box.
*/
bool QGeoBoundingBox::contains(const QGeoBoundingBox &boundingBox) const
{
    return (contains(boundingBox.topLeft())
            && contains(boundingBox.topRight())
            && contains(boundingBox.bottomLeft())
            && contains(boundingBox.bottomRight()));
}

/*!
    Returns whether the bounding box \a boundingBox intersects this bounding
    box.

    If the top or bottom edges of both bounding boxes are at one of the poles
    the bounding boxes are considered to be intersecting, since the longitude
    is irrelevant when the edges are at the pole.
*/
bool QGeoBoundingBox::intersects(const QGeoBoundingBox &boundingBox) const
{
    double left1 = d_ptr->topLeft.longitude();
    double right1 = d_ptr->bottomRight.longitude();
    double top1 = d_ptr->topLeft.latitude();
    double bottom1 = d_ptr->bottomRight.latitude();

    double left2 = boundingBox.d_ptr->topLeft.longitude();
    double right2 = boundingBox.d_ptr->bottomRight.longitude();
    double top2 = boundingBox.d_ptr->topLeft.latitude();
    double bottom2 = boundingBox.d_ptr->bottomRight.latitude();

    if (top1 < bottom2)
        return false;

    if (bottom1 > top2)
        return false;

    if ((top1 == 90.0) && (top1 == top2))
        return true;

    if ((bottom1 == -90.0) && (bottom1 == bottom2))
        return true;

    if (left1 < right1) {
        if (left2 < right2) {
            if ((left1 > right2) || (right1 < left2))
                return false;
        } else {
            if ((left1 > right2) && (right1 < left2))
                return false;
        }
    } else {
        if (left2 < right2) {
            if ((left2 > right1) && (right2 < left1))
                return false;
        } else {
            // if both wrap then they have to intersect
        }
    }

    return true;
}

/*!
    Translates this bounding box by \a degreesLatitude northwards and \a
    degreesLongitude eastwards.

    Negative values of \a degreesLatitude and \a degreesLongitude correspond to
    southward and westward translation respectively.

    If the translation would have caused the bounding box to cross a pole the
    bounding box will be translated until the top or bottom edge of bounding
    box touches the pole but not further.
*/
void QGeoBoundingBox::translate(double degreesLatitude, double degreesLongitude)
{
    // TODO handle dlat, dlon larger than 360 degrees

    double tlLat = d_ptr->topLeft.latitude();
    double tlLon = d_ptr->topLeft.longitude();
    double brLat = d_ptr->bottomRight.latitude();
    double brLon = d_ptr->bottomRight.longitude();

    if ((tlLat != 90.0) || (brLat != -90.0)) {
        tlLat += degreesLatitude;
        brLat += degreesLatitude;
    }

    if ( (tlLon != -180.0) || (brLon != 180.0) ) {
        tlLon += degreesLongitude;
        brLon += degreesLongitude;
    }

    if (tlLon < -180.0)
        tlLon += 360.0;
    if (tlLon > 180.0)
        tlLon -= 360.0;

    if (brLon < -180.0)
        brLon += 360.0;
    if (brLon > 180.0)
        brLon -= 360.0;

    if (tlLat > 90.0)
        tlLat = 90.0;

    if (tlLat < -90.0)
        tlLat = -90.0;

    if (brLat > 90.0)
        brLat = 90.0;

    if (brLat < -90.0)
        brLat = -90.0;

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
    Returns a copy of this bounding box translated by \a degreesLatitude northwards and \a
    degreesLongitude eastwards.

    Negative values of \a degreesLatitude and \a degreesLongitude correspond to
    southward and westward translation respectively.

    \sa translate()
*/
QGeoBoundingBox QGeoBoundingBox::translated(double degreesLatitude, double degreesLongitude) const
{
    QGeoBoundingBox result(*this);
    result.translate(degreesLatitude, degreesLongitude);
    return result;
}

/*!
    Returns the smallest bounding box which contains both this bounding box and \a boundingBox.
*/
QGeoBoundingBox QGeoBoundingBox::united(const QGeoBoundingBox &boundingBox) const
{
    QGeoBoundingBox result(*this);
    result |= boundingBox;
    return result;
}

/*!
    \fn QGeoBoundingBox QGeoBoundingBox::operator | (const QGeoBoundingBox &boundingBox) const

    Returns the smallest bounding box which contains both this bounding box and \a boundingBox.
*/

/*!
    Returns the smallest bounding box which contains both this bounding box and \a boundingBox.
*/
QGeoBoundingBox& QGeoBoundingBox::operator |= (const QGeoBoundingBox & boundingBox)
{
    // If non-intersecting goes for most narrow box

    double left1 = d_ptr->topLeft.longitude();
    double right1 = d_ptr->bottomRight.longitude();
    double top1 = d_ptr->topLeft.latitude();
    double bottom1 = d_ptr->bottomRight.latitude();

    double left2 = boundingBox.d_ptr->topLeft.longitude();
    double right2 = boundingBox.d_ptr->bottomRight.longitude();
    double top2 = boundingBox.d_ptr->topLeft.latitude();
    double bottom2 = boundingBox.d_ptr->bottomRight.latitude();

    double top = qMax(top1, top2);
    double bottom = qMin(bottom1, bottom2);

    double left = 0.0;
    double right = 0.0;

    bool wrap1 = (left1 > right1);
    bool wrap2 = (left2 > right2);

    if ((wrap1 && wrap2) || (!wrap1 && !wrap2)) {
        left = qMin(left1, left2);
        right = qMax(right1, right2);
    } else {
        double wrapLeft = 0.0;
        double wrapRight = 0.0;
        double nonWrapLeft = 0.0;
        double nonWrapRight = 0.0;

        if (wrap1) {
            wrapLeft = left1;
            wrapRight = right1;
            nonWrapLeft = left2;
            nonWrapRight = right2;
        } else {
            wrapLeft = left2;
            wrapRight = right2;
            nonWrapLeft = left1;
            nonWrapRight = right1;
        }

        bool joinWrapLeft = (nonWrapRight >= wrapLeft);
        bool joinWrapRight = (nonWrapLeft <= wrapRight);

        if (joinWrapLeft) {
            if (joinWrapRight) {
                left = -180.0;
                right = 180.0;
            } else {
                left = nonWrapLeft;
                right = wrapRight;
            }
        } else {
            if (joinWrapRight) {
                left = wrapLeft;
                right = nonWrapRight;
            } else {
                double wrapRightDistance = nonWrapLeft - wrapRight;
                double wrapLeftDistance = wrapLeft - nonWrapRight;

                if (wrapLeftDistance == wrapRightDistance) {
                    left = -180.0;
                    right = 180.0;
                } else if (wrapLeftDistance < wrapRightDistance) {
                    left = nonWrapLeft;
                    right = wrapRight;
                } else {
                    left = wrapLeft;
                    right = nonWrapRight;
                }
            }
        }
    }

    if (((left1 == -180) && (right1 == 180.0))
            || ((left2 == -180) && (right2 == 180.0))) {
        left = -180;
        right = 180;
    }

    d_ptr->topLeft = QGeoCoordinate(top, left);
    d_ptr->bottomRight = QGeoCoordinate(bottom, right);

    return *this;
}

/*******************************************************************************
*******************************************************************************/

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate()
    : QSharedData() {}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
        : QSharedData(),
        topLeft(topLeft),
        bottomRight(bottomRight) {}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoBoundingBoxPrivate &other)
        : QSharedData(),
        topLeft(other.topLeft),
        bottomRight(other.bottomRight) {}

QGeoBoundingBoxPrivate::~QGeoBoundingBoxPrivate() {}

QGeoBoundingBoxPrivate& QGeoBoundingBoxPrivate::operator= (const QGeoBoundingBoxPrivate & other)
{
    topLeft = other.topLeft;
    bottomRight = other.bottomRight;

    return *this;
}

bool QGeoBoundingBoxPrivate::operator== (const QGeoBoundingBoxPrivate &other) const
{
    return ((topLeft == other.topLeft) && (bottomRight == other.bottomRight));
}

QTM_END_NAMESPACE

