/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapcircleobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapCircle

    \brief The MapCircle element displays a circle on a map.
    \inherits QGeoMapCircleObject

    \ingroup qml-location-maps

    The circle is specified in terms of a central coordinate and
    a radius in metres.

    If \l center and \l radius are not specified and valid the
    circle will not be displayed.

    An example, a MapCircle positioned at your current position (assuming there
    is a PositionSource):
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic map position marker definition
    The element is used with a Map element.

    The MapCircle element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoMapCircleObject::QDeclarativeGeoMapCircleObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent), circle_(0), center_(0)
{
    circle_ = new QGeoMapCircleObject();
    setMapObject(circle_);

    connect(circle_,
            SIGNAL(radiusChanged(qreal)),
            this,
            SIGNAL(radiusChanged(qreal)));
    connect(&border_,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&border_,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapCircleObject::~QDeclarativeGeoMapCircleObject()
{
    delete circle_;
}

/*!
    \qmlproperty Coordinate MapCircle::center

    This property holds the coordinate at the center of the circle.

    The default value is an invalid coordinate.
*/

void QDeclarativeGeoMapCircleObject::setCenter(QDeclarativeCoordinate *center)
{
    if (!center || center == center_)
        return;
    center_ = center;
    connect(center_,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(centerLatitudeChanged(double)));
    connect(center_,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(centerLongitudeChanged(double)));
    connect(center_,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(centerAltitudeChanged(double)));

    circle_->setCenter(center->coordinate());
    emit centerChanged(center_);
}

QDeclarativeCoordinate* QDeclarativeGeoMapCircleObject::center()
{
    return center_;
}

void QDeclarativeGeoMapCircleObject::centerLatitudeChanged(double /*latitude*/)
{
    circle_->setCenter(center_->coordinate());
}

void QDeclarativeGeoMapCircleObject::centerLongitudeChanged(double /*longitude*/)
{
    circle_->setCenter(center_->coordinate());
}

void QDeclarativeGeoMapCircleObject::centerAltitudeChanged(double /*altitude*/)
{
    circle_->setCenter(center_->coordinate());
}

void QDeclarativeGeoMapCircleObject::setRadius(qreal radius)
{
    circle_->setRadius(radius);
}

qreal QDeclarativeGeoMapCircleObject::radius() const
{
    return circle_->radius();
}

/*!
    \qmlproperty qreal MapCircle::radius

    This property holds the radius of the circle in metres.

    A negative value is used to indicate that the radius is invalid and
    the default value is a radius of -1.0.
*/

/*!
    \qmlproperty color MapCircle::color

    This property holds the color used to fill the circle.

    The default value corresponds to a transparent color.
*/

void QDeclarativeGeoMapCircleObject::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    QBrush m_brush(color);
    circle_->setBrush(m_brush);
    emit colorChanged(color_);
}

QColor QDeclarativeGeoMapCircleObject::color() const
{
    return color_;
}

/*!
    \qmlproperty int MapCircle::border.width
    \qmlproperty color MapCircle::border.color

    These properties hold the width and color used to draw the border of the circle.

    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/
QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapCircleObject::border()
{
    return &border_;
}

void QDeclarativeGeoMapCircleObject::borderColorChanged(const QColor &color)
{
    QPen p = circle_->pen();
    p.setColor(color);
    circle_->setPen(p);
}

void QDeclarativeGeoMapCircleObject::borderWidthChanged(int width)
{
    QPen p = circle_->pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    circle_->setPen(p);
}

/*!
    \qmlproperty int MapCircle::z

    This property holds the z-value of the circle.

    Map objects are drawn in z-value order, and objects with the
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapCircle::visible

    This property holds a boolean corresponding to whether or not the
    circle is visible.
*/

#include "moc_qdeclarativegeomapcircleobject_p.cpp"

QTM_END_NAMESPACE

