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

#include "qgeomapcustomobject.h"
#include "qgeomapcustomobject_p.h"
#include "qgeoboundingbox.h"

#include <QGraphicsItem>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapCustomObject
    \brief The QGeoMapCustomObject class is a QGeoMapObject used to draw
    a pixmap on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects
    \since 1.2

    Any arbitrary QGraphicsItem can be associated with a QGeoMapCustomObject, and to
    this end it contains support for interpreting the coordinates of the
    QGraphicsItem in a variety of different ways.

    For example, the following code creates a QGraphicsEllipseItem and a
    QGeoMapCustomObject to display it. The EllipseItem extends from the origin point,
    out 20 meters to the east and 30 metres south.

    \code
    QGraphicsEllipseItem *ellipseItem = new QGraphicsEllipseItem;
    ellipseItem->setRect(0, 0, 20, 30);

    QGeoMapCustomObject *mapObject = new QGeoMapCustomObject;
    mapObject->setGraphicsItem(ellipseItem);
    mapObject->setUnits(QGeoMapObject::MeterUnit);
    mapObject->setOrigin(QGeoCoordinate(-27.5796, 153.1));
    \endcode

    \section2 Units and coordinates

    The local units and coordinates of the QGraphicsItem are transformed
    onto the map based on the \a units, \a origin, \a transformType and
    \a transform properties. Several systems are available, including
    pixels, meters and seconds of arc.

    It should be noted that both pixel and meter coordinate systems are south-
    oriented (ie, positive Y axis faces south on the map). However, the
    RelativeArcSeconds unit system faces north to align with the standard
    latitude grid. The Y axis can be flipped if necessary by making use of the
    GraphicsItem's \a transform property

    \code
    QTransform northFlip;
    northFlip.scale(0, -1);

    ellipseItem->setTransform(northFlip);
    \endcode

    \section2 Transform methods

    Normally, the GraphicsItem will be transformed into map coordinates using
    a bilinear interpolation. Another option is the ExactTransform, which
    converts the GraphicsItem exactly into map coordinates, but is only available
    for certain subclasses of QGraphicsItem. Other interpolation methods may
    be provided in future for greater accuracy near poles and in different
    map projections, without the limitations of ExactTransform.

    Calling setUnits() or setting the units property will result in the
    default value of transformType being restored. See QGeoMapObject::transformType
    for more details.

    \section2 Caveats

    Other than the coordinate system features, there are a few differences
    with using QGraphicsItems on a map compared to using them on a standard
    QGraphicsScene. One of the most important of these is the use of the
    \a update() function. When an application changes anything that has an
    effect upon the appearance, size, shape etc of the QGraphicsItem, it
    must call \a QGeoMapCustomObject::update() to ensure that the map is updated.

    Another is the use of child items of a QGraphicsItem. These are supported
    in more or less the same manner as in QGraphicsScene, with the exception
    of use in concert with \a ExactTransform -- any object with transformType
    set to \a ExactTransform will not have children of its QGraphicsItem drawn
    on the map.
*/

/*!
    Constructs a new custom object.
*/
QGeoMapCustomObject::QGeoMapCustomObject()
    : d_ptr(new QGeoMapCustomObjectPrivate())
{
}

/*!
    Constructs a new custom object which will draw a QGraphicsItem at an
    offset of \a offset pixels from the coordinate \a coordinate.
*/
QGeoMapCustomObject::QGeoMapCustomObject(const QGeoCoordinate &coordinate, const QPoint &offset)
    : d_ptr(new QGeoMapCustomObjectPrivate())
{
    setOrigin(coordinate);
    d_ptr->offset = offset;
}

/*!
    Destroys this custom object.
*/
QGeoMapCustomObject::~QGeoMapCustomObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapCustomObject::type() const
{
    return QGeoMapObject::CustomType;
}

/*!
*/
void QGeoMapCustomObject::update()
{
    emit triggerUpdate();
}

/*!
\fn void QGeoMapCustomObject::triggerUpdate()
*/

/*!
    \property QGeoMapCustomObject::graphicsItem
    \brief This property holds the graphics item which will
    be drawn by this custom object.

    If the graphics item is 0 then nothing will be drawn.
*/

QGraphicsItem* QGeoMapCustomObject::graphicsItem() const
{
    return d_ptr->graphicsItem;
}

void QGeoMapCustomObject::setGraphicsItem(QGraphicsItem *graphicsItem)
{
    if (d_ptr->graphicsItem == graphicsItem)
        return;

    d_ptr->graphicsItem = graphicsItem;

    emit graphicsItemChanged(graphicsItem);
}

/*!
\fn void QGeoMapCustomObject::graphicsItemChanged(QGraphicsItem *graphicsItem)

    This signal is emitted when the graphics item which this custom object
    draws is changed.

    The new value will be \a graphicsItem.
*/

/*!
    \property QGeoMapCustomObject::offset
    \brief This property holds the offset in pixels at which the graphics
    item will be drawn by this custom object.

    The default value of this property is QPoint(0, 0). If this value is not
    changed the upper left coordinate of the graphics item will be drawn at the
    coordinate specified by QGeoMapCustomObject::coordinate.

    The offset is in pixels and is independent of the zoom level of the map.
*/
QPoint QGeoMapCustomObject::offset() const
{
    return d_ptr->offset;
//    QPointF pt = d_ptr->offset;
//
//    QPoint rounded;
//    rounded.setX(int(pt.x() > 0 ? pt.x() + 0.5 : pt.x() - 0.5));
//    rounded.setY(int(pt.y() > 0 ? pt.y() + 0.5 : pt.y() - 0.5));
//
//    return rounded;
}

void QGeoMapCustomObject::setOffset(const QPoint &offset)
{
    if (d_ptr->offset != offset) {
        d_ptr->offset = offset;
        emit offsetChanged(offset);
    }
}

/*!
\fn void QGeoMapCustomObject::offsetChanged(const QPoint &offset)
    
    This signal is emitted when the on-screen offset from the coordinate 
    at which this custom object should be drawn has changed.

    The new value will be \a offset.
*/

/*!
    Sets the origin of the object to \a origin.
*/
void QGeoMapCustomObject::setOrigin(const QGeoCoordinate &origin)
{
    QGeoMapObject::setOrigin(origin);
}

/*!
    Sets the coordinate units of the object to \a unit.

    Note that setting this property will reset the transformType property to
    the default for the units given. For PixelUnit, this is ExactTransform,
    and for all others, BilinearTransform.
*/
void QGeoMapCustomObject::setUnits(const CoordinateUnit &unit)
{
    QGeoMapObject::setUnits(unit);
}

/*!
    Sets the transform type of the object to \a type.
*/
void QGeoMapCustomObject::setTransformType(const TransformType &type)
{
    QGeoMapObject::setTransformType(type);
}

/*******************************************************************************
*******************************************************************************/

QGeoMapCustomObjectPrivate::QGeoMapCustomObjectPrivate()
    : graphicsItem(0) {}

QGeoMapCustomObjectPrivate::~QGeoMapCustomObjectPrivate()
{
}

#include "moc_qgeomapcustomobject.cpp"

QTM_END_NAMESPACE

