/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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

QT_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingBox
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \since Qt Location 5.0

    \brief The QGeoBoundingBox class defines a rectangular geographic area.

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

inline QGeoBoundingBoxPrivate *QGeoBoundingBox::d_func()
{
    return static_cast<QGeoBoundingBoxPrivate *>(d_ptr.data());
}

inline const QGeoBoundingBoxPrivate *QGeoBoundingBox::d_func() const
{
    return static_cast<const QGeoBoundingBoxPrivate *>(d_ptr.constData());
}

/*!
    Constructs a new, invalid bounding box.
*/
QGeoBoundingBox::QGeoBoundingBox()
:   QGeoBoundingArea(new QGeoBoundingBoxPrivate)
{
}

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
{
    d_ptr = new QGeoBoundingBoxPrivate(center, center);
    setWidth(degreesWidth);
    setHeight(degreesHeight);
}

/*!
    Constructs a new bounding box with a top left coordinate \a
    topLeft and a bottom right coordinate \a bottomRight.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
{
    d_ptr = new QGeoBoundingBoxPrivate(topLeft, bottomRight);
}

/*!
    Constructs a bounding box from the contents of \a other.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoBoundingBox &other)
:   QGeoBoundingArea(other)
{
}

/*!
    Constructs a bounding box from the contents of \a other.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoBoundingArea &other)
:   QGeoBoundingArea(other)
{
    if (type() != QGeoBoundingArea::BoxType)
        d_ptr = new QGeoBoundingBoxPrivate;
}

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
    if (this == &other)
        return *this;

    QGeoBoundingArea::operator=(other);
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this bounding box is equal to \a other.
*/
bool QGeoBoundingBox::operator == (const QGeoBoundingBox &other) const
{
    Q_D(const QGeoBoundingBox);

    return *d == *other.d_func();
}

/*!
    Returns whether this bounding box is not equal to \a other.
*/
bool QGeoBoundingBox::operator != (const QGeoBoundingBox &other) const
{
    Q_D(const QGeoBoundingBox);

    return !(*d == *other.d_func());
}

bool QGeoBoundingBoxPrivate::isValid() const
{
    return topLeft.isValid() && bottomRight.isValid() &&
           topLeft.latitude() >= bottomRight.latitude();
}

bool QGeoBoundingBoxPrivate::isEmpty() const
{
    if (!isValid())
        return true;

    return topLeft.latitude() == bottomRight.latitude() ||
           topLeft.longitude() == bottomRight.longitude();
}

/*!
    Sets the top left coordinate of this bounding box to \a topLeft.
*/
void QGeoBoundingBox::setTopLeft(const QGeoCoordinate &topLeft)
{
    Q_D(QGeoBoundingBox);

    d->topLeft = topLeft;
}

/*!
    Returns the top left coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::topLeft() const
{
    Q_D(const QGeoBoundingBox);

    return d->topLeft;
}

/*!
    Sets the top right coordinate of this bounding box to \a topRight.
*/
void QGeoBoundingBox::setTopRight(const QGeoCoordinate &topRight)
{
    Q_D(QGeoBoundingBox);

    d->topLeft.setLatitude(topRight.latitude());
    d->bottomRight.setLongitude(topRight.longitude());
}

/*!
    Returns the top right coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::topRight() const
{
    // TODO remove?
    if (!isValid())
        return QGeoCoordinate();

    Q_D(const QGeoBoundingBox);

    return QGeoCoordinate(d->topLeft.latitude(), d->bottomRight.longitude());
}

/*!
    Sets the bottom left coordinate of this bounding box to \a bottomLeft.
*/
void QGeoBoundingBox::setBottomLeft(const QGeoCoordinate &bottomLeft)
{
    Q_D(QGeoBoundingBox);

    d->bottomRight.setLatitude(bottomLeft.latitude());
    d->topLeft.setLongitude(bottomLeft.longitude());
}

/*!
    Returns the bottom left coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::bottomLeft() const
{
    // TODO remove?
    if (!isValid())
        return QGeoCoordinate();

    Q_D(const QGeoBoundingBox);

    return QGeoCoordinate(d->bottomRight.latitude(), d->topLeft.longitude());
}

/*!
    Sets the bottom right coordinate of this bounding box to \a bottomRight.
*/
void QGeoBoundingBox::setBottomRight(const QGeoCoordinate &bottomRight)
{
    Q_D(QGeoBoundingBox);

    d->bottomRight = bottomRight;
}

/*!
    Returns the bottom right coordinate of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::bottomRight() const
{
    Q_D(const QGeoBoundingBox);

    return d->bottomRight;
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
    Q_D(QGeoBoundingBox);

    if (!isValid()) {
        d->topLeft = center;
        d->bottomRight = center;
        return;
    }
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
        brLat = 2 * center.latitude() - 90.0;
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

    d->topLeft = QGeoCoordinate(tlLat, tlLon);
    d->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
    Returns the center of this bounding box.
*/
QGeoCoordinate QGeoBoundingBox::center() const
{
    if (!isValid())
        return QGeoCoordinate();

    Q_D(const QGeoBoundingBox);

    double cLat = (d->topLeft.latitude() + d->bottomRight.latitude()) / 2.0;

    double cLon = (d->bottomRight.longitude() + d->topLeft.longitude()) / 2.0;
    if (d->topLeft.longitude() > d->bottomRight.longitude()) {
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

    Q_D(QGeoBoundingBox);

    if (degreesWidth >= 360.0) {
        d->topLeft.setLongitude(-180.0);
        d->bottomRight.setLongitude(180.0);
        return;
    }

    double tlLat = d->topLeft.latitude();
    double brLat = d->bottomRight.latitude();

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

    d->topLeft = QGeoCoordinate(tlLat, tlLon);
    d->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
    Returns the width of this bounding box in degrees.

    The return value is undefined if this bounding box is invalid.
*/
double QGeoBoundingBox::width() const
{
    if (!isValid())
        return qQNaN();

    Q_D(const QGeoBoundingBox);

    double result = d->bottomRight.longitude() - d->topLeft.longitude();
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

    This change is done such that the center coordinate is still at the
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

    Q_D(QGeoBoundingBox);

    double tlLon = d->topLeft.longitude();
    double brLon = d->bottomRight.longitude();

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

    d->topLeft = QGeoCoordinate(tlLat, tlLon);
    d->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
    Returns the height of this bounding box in degrees.

    The return value is undefined if this bounding box is invalid.
*/
double QGeoBoundingBox::height() const
{
    if (!isValid())
        return qQNaN();

    Q_D(const QGeoBoundingBox);

    double result = d->topLeft.latitude() - d->bottomRight.latitude();
    if (result < 0.0)
        result = qQNaN();
    return result;
}

bool QGeoBoundingBoxPrivate::contains(const QGeoCoordinate &coordinate) const
{
    if (!isValid() || !coordinate.isValid())
        return false;

    double left = topLeft.longitude();
    double right = bottomRight.longitude();
    double top = topLeft.latitude();
    double bottom = bottomRight.latitude();

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
    Q_D(const QGeoBoundingBox);

    return (d->contains(boundingBox.topLeft())
            && d->contains(boundingBox.topRight())
            && d->contains(boundingBox.bottomLeft())
            && d->contains(boundingBox.bottomRight()));
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
    Q_D(const QGeoBoundingBox);

    double left1 = d->topLeft.longitude();
    double right1 = d->bottomRight.longitude();
    double top1 = d->topLeft.latitude();
    double bottom1 = d->bottomRight.latitude();

    double left2 = boundingBox.d_func()->topLeft.longitude();
    double right2 = boundingBox.d_func()->bottomRight.longitude();
    double top2 = boundingBox.d_func()->topLeft.latitude();
    double bottom2 = boundingBox.d_func()->bottomRight.latitude();

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

    Q_D(QGeoBoundingBox);

    double tlLat = d->topLeft.latitude();
    double tlLon = d->topLeft.longitude();
    double brLat = d->bottomRight.latitude();
    double brLon = d->bottomRight.longitude();

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

    d->topLeft = QGeoCoordinate(tlLat, tlLon);
    d->bottomRight = QGeoCoordinate(brLat, brLon);
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

    If the centers of the two bounding boxes are separated by exactly 180.0 degrees then the
    width is set to 360.0 degrees with the leftmost longitude set to -180.0 degrees and the
    rightmost longitude set to 180.0 degrees.  This is done to ensure that the result is
    independent of the order of the operands.

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

    If the centers of the two bounding boxes are separated by exactly 180.0 degrees then the
    width is set to 360.0 degrees with the leftmost longitude set to -180.0 degrees and the
    rightmost longitude set to 180.0 degrees.  This is done to ensure that the result is
    independent of the order of the operands.

*/

/*!
    Returns the smallest bounding box which contains both this bounding box and \a boundingBox.

    If the centers of the two bounding boxes are separated by exactly 180.0 degrees then the
    width is set to 360.0 degrees with the leftmost longitude set to -180.0 degrees and the
    rightmost longitude set to 180.0 degrees.  This is done to ensure that the result is
    independent of the order of the operands.

*/
QGeoBoundingBox& QGeoBoundingBox::operator |= (const QGeoBoundingBox & boundingBox)
{
    // If non-intersecting goes for most narrow box

    Q_D(QGeoBoundingBox);

    double left1 = d->topLeft.longitude();
    double right1 = d->bottomRight.longitude();
    double top1 = d->topLeft.latitude();
    double bottom1 = d->bottomRight.latitude();

    double left2 = boundingBox.d_func()->topLeft.longitude();
    double right2 = boundingBox.d_func()->bottomRight.longitude();
    double top2 = boundingBox.d_func()->topLeft.latitude();
    double bottom2 = boundingBox.d_func()->bottomRight.latitude();

    double top = qMax(top1, top2);
    double bottom = qMin(bottom1, bottom2);

    double left = 0.0;
    double right = 0.0;

    bool wrap1 = (left1 > right1);
    bool wrap2 = (left2 > right2);

    if ((wrap1 && wrap2) || (!wrap1 && !wrap2)) {

        double w = qAbs((left1 + right1 - left2 - right2) / 2.0);

        if (w < 180.0) {
            left = qMin(left1, left2);
            right = qMax(right1, right2);
        } else if (w > 180.0) {
            left = qMax(left1, left2);
            right = qMin(right1, right2);
        } else {
            left = -180.0;
            right = 180.0;
        }

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

    d->topLeft = QGeoCoordinate(top, left);
    d->bottomRight = QGeoCoordinate(bottom, right);

    return *this;
}

/*******************************************************************************
*******************************************************************************/

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate()
:   QGeoBoundingAreaPrivate(QGeoBoundingArea::BoxType)
{
}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoCoordinate &topLeft,
                                               const QGeoCoordinate &bottomRight)
:   QGeoBoundingAreaPrivate(QGeoBoundingArea::BoxType), topLeft(topLeft), bottomRight(bottomRight)
{
}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoBoundingBoxPrivate &other)
:   QGeoBoundingAreaPrivate(QGeoBoundingArea::BoxType), topLeft(other.topLeft),
    bottomRight(other.bottomRight)
{
}

QGeoBoundingBoxPrivate::~QGeoBoundingBoxPrivate() {}

QGeoBoundingAreaPrivate *QGeoBoundingBoxPrivate::clone() const
{
    return new QGeoBoundingBoxPrivate(*this);
}

bool QGeoBoundingBoxPrivate::operator==(const QGeoBoundingAreaPrivate &other) const
{
    if (!QGeoBoundingAreaPrivate::operator==(other))
        return false;

    const QGeoBoundingBoxPrivate &otherBox = static_cast<const QGeoBoundingBoxPrivate &>(other);

    return topLeft == otherBox.topLeft && bottomRight == otherBox.bottomRight;
}

QT_END_NAMESPACE

