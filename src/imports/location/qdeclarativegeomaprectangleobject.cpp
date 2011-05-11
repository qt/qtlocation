


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

#include "qdeclarativegeomaprectangleobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapRectangle

    \brief The MapRectangle element displays a rectangle on a map.
    \inherits QGeoMapRectangleObject

    \ingroup qml-location-maps

    The rectangle is specified in terms of the top left and bottom
    right coordinates.

    If \l topLeft and \l bottomRight are not specified and valid the
    rectangle will not be displayed.

    An example of one big yellow rectangle:
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml MapRectangle

    The MapRectangle element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoMapRectangleObject::QDeclarativeGeoMapRectangleObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent), rectangle_(0), topLeft_(0), bottomRight_(0)
{
    rectangle_ = new QGeoMapRectangleObject();
    setMapObject(rectangle_);

    connect(&border_,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&border_,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapRectangleObject::~QDeclarativeGeoMapRectangleObject()
{
    delete rectangle_;
}

/*!
    \qmlproperty Coordinate MapRectangle::topLeft

    This property holds the coordinate corresponding to the top left
    corner of the rectangle.

    The default value is an invalid coordinate.
*/

void QDeclarativeGeoMapRectangleObject::setTopLeft(QDeclarativeCoordinate *topLeft)
{
    if (!topLeft || topLeft == topLeft_)
        return;

    topLeft_ = topLeft;

    connect(topLeft_,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(topLeftLatitudeChanged(double)));
    connect(topLeft_,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(topLeftLongitudeChanged(double)));
    connect(topLeft_,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(topLeftAltitudeChanged(double)));

    rectangle_->setTopLeft(topLeft->coordinate());
    emit topLeftChanged(topLeft_);
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::topLeft()
{
    return topLeft_;
}

void QDeclarativeGeoMapRectangleObject::topLeftLatitudeChanged(double /*latitude*/)
{
    rectangle_->setTopLeft(topLeft_->coordinate());
}

void QDeclarativeGeoMapRectangleObject::topLeftLongitudeChanged(double /*longitude*/)
{
    rectangle_->setTopLeft(topLeft_->coordinate());
}

void QDeclarativeGeoMapRectangleObject::topLeftAltitudeChanged(double /*altitude*/)
{
    rectangle_->setTopLeft(topLeft_->coordinate());
}

/*!
    \qmlproperty Coordinate MapRectangle::bottomRight

    This property holds the coordinate corresponding to the bottom right
    corner of the rectangle.

    The default value is an invalid coordinate.
*/

void QDeclarativeGeoMapRectangleObject::setBottomRight(QDeclarativeCoordinate *bottomRight)
{
    if (!bottomRight || bottomRight == bottomRight_)
        return;

    bottomRight_ = bottomRight;
    connect(bottomRight_,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(bottomRightLatitudeChanged(double)));
    connect(bottomRight_,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(bottomRightLongitudeChanged(double)));
    connect(bottomRight_,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(bottomRightAltitudeChanged(double)));

    rectangle_->setBottomRight(bottomRight->coordinate());

    emit bottomRightChanged(bottomRight_);
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::bottomRight()
{
    return bottomRight_;
}

void QDeclarativeGeoMapRectangleObject::bottomRightLatitudeChanged(double /*latitude*/)
{
    rectangle_->setBottomRight(bottomRight_->coordinate());
}

void QDeclarativeGeoMapRectangleObject::bottomRightLongitudeChanged(double /*longitude*/)
{
    rectangle_->setBottomRight(bottomRight_->coordinate());
}

void QDeclarativeGeoMapRectangleObject::bottomRightAltitudeChanged(double /*altitude*/)
{
    rectangle_->setBottomRight(bottomRight_->coordinate());
}

/*!
    \qmlproperty color MapRectangle::color

    This property holds the color used to fill the circle.

    The default value corresponds to a transparent color.
*/

void QDeclarativeGeoMapRectangleObject::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    QBrush m_brush(color);
    rectangle_->setBrush(m_brush);
    emit colorChanged(color_);
}

QColor QDeclarativeGeoMapRectangleObject::color() const
{
    return color_;
}

/*!
    \qmlproperty int MapRectangle::border.width
    \qmlproperty color MapRectangle::border.color

    These properties hold the width and color used to draw the border of the circle.

    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapRectangleObject::border()
{
    return &border_;
}

void QDeclarativeGeoMapRectangleObject::borderColorChanged(const QColor &color)
{
    QPen p = rectangle_->pen();
    p.setColor(color);
    rectangle_->setPen(p);
}

void QDeclarativeGeoMapRectangleObject::borderWidthChanged(int width)
{
    QPen p = rectangle_->pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    rectangle_->setPen(p);
}

/*!
    \qmlproperty int MapRectangle::z

    This property holds the z-value of the rectangle.

    Map objects are drawn in z-value order, and objects with the
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapRectangle::visible

    This property holds a boolean corresponding to whether or not the
    rectangle is visible.
*/

#include "moc_qdeclarativegeomaprectangleobject_p.cpp"

QTM_END_NAMESPACE

