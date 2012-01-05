/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapquickitem_p.h"
#include "qdeclarativecoordinate_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>

#include <QDebug>
#include <cmath>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapQuickItem QDeclarativeGeoMapQuickItem
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

    \brief The MapQuickItem element displays an arbitrary Qt Quick element
           on a Map.

    The MapQuickItem element is used to place an arbitrary Qt Quick element
    on a Map at a specified location and size. Compared to floating an item
    above the Map, a MapQuickItem will follow the panning (and optionally, the
    zooming) of the Map as if it is on the Map surface.

    \section2 Positioning and Sizing

    The positioning of the MapQuickItem on the Map is controlled by two
    properties: \l coordinate and \l anchorPoint. If only \l coordinate is set,
    it specifies a longitude/latitude coordinate for the item to be placed at.
    The set coordinate will line up with the top-left corner of the contained
    item when shown on the screen.

    The \l anchorPoint property provides a way to line up the coordinate with
    other parts of the item than just the top-left corner, by setting a number
    of pixels the item will be offset by. A simple way to think about it is
    to note that the point given by \l anchorPoint on the item itself is the
    point that will line up with the given \l coordinate when displayed.

    In addition to being anchored to the map, the MapQuickItem can optionally
    follow the scale of the map, and change size when the Map is zoomed in or
    zoomed out. This behaviour is controlled by the \l zoomLevel property. The
    default behaviour if \l zoomLevel is not set is for the item to be drawn
    "on the screen" rather than "on the map", so that its size remains the same
    regardless of the zoom level of the Map.

    \section2 Performance

    Performance of a MapQuickItem is normally in the same ballpark as the
    contained Qt Quick item alone. Overheads added amount to a translation
    and (possibly) scaling of the original item, as well as a transformation
    from longitude and latitude to screen position.

    \section2 Example Usage

    The following snippet shows a MapQuickItem containing an Image element,
    to display a Marker on the Map. This strategy is used to show the map
    markers in the MapViewer example.

    \snippet examples/declarative/mapviewer/content/map/Marker.qml mqi-top
    \snippet examples/declarative/mapviewer/content/map/Marker.qml mqi-anchor
    \snippet examples/declarative/mapviewer/content/map/Marker.qml mqi-closeimage
    \snippet examples/declarative/mapviewer/content/map/Marker.qml mqi-close

    \image ../../../doc/src/images/api-mapquickitem.png
*/

QDeclarativeGeoMapQuickItem::QDeclarativeGeoMapQuickItem(QQuickItem *parent)
    : QDeclarativeGeoMapItemBase(parent),
      coordinate_(&internalCoordinate_),
      sourceItem_(0),
      zoomLevel_(0.0),
      inUpdate_(false),
      mapAndSourceItemSet_(false)
{
    setFlag(ItemHasContents, true);
}

QDeclarativeGeoMapQuickItem::~QDeclarativeGeoMapQuickItem() {}

/*!
    \qmlproperty Coordinate MapQuickItem::coordinate

    This property holds the anchor coordinate of the MapQuickItem. The point
    on the sourceItem that is specified by anchorPoint is kept aligned with
    this coordinate when drawn on the map.

    In the image below, there are 3 MapQuickItems that are identical except
    for the value of their anchorPoint properties. The values of anchorPoint
    for each are written on top of the item.

    \image ../../../doc/src/images/api-mapquickitem-anchor.png
*/
void QDeclarativeGeoMapQuickItem::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (coordinate_ == coordinate)
        return;
    if (coordinate_)
        coordinate_->disconnect(this);
    coordinate_ = coordinate;

    if (coordinate_) {
        connect(coordinate_,
                SIGNAL(latitudeChanged(double)),
                this,
                SLOT(updateMapItem()));
        connect(coordinate_,
                SIGNAL(longitudeChanged(double)),
                this,
                SLOT(updateMapItem()));
        connect(coordinate_,
                SIGNAL(altitudeChanged(double)),
                this,
                SLOT(updateMapItem()));
    }

    updateMapItem();

    emit coordinateChanged();
}

void QDeclarativeGeoMapQuickItem::setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map && quickMap) {
        QObject::connect(quickMap, SIGNAL(heightChanged()), this, SLOT(updateMapItem()));
        QObject::connect(quickMap, SIGNAL(widthChanged()), this, SLOT(updateMapItem()));
        QObject::connect(map, SIGNAL(cameraDataChanged(QGeoCameraData)), this, SLOT(updateMapItem()));
        updateMapItem();
    }
}

void QDeclarativeGeoMapQuickItem::dragEnded()
{
    if (!mapAndSourceItemSet_)
        return;
    QGeoCoordinate newCoordinate = map()->screenPositionToCoordinate(QPointF(x(), y()) + (scaleFactor() * anchorPoint_), false);
    if (newCoordinate.isValid()) {
        internalCoordinate_.setCoordinate(newCoordinate);
        setCoordinate(&internalCoordinate_);
    }
}

QDeclarativeCoordinate* QDeclarativeGeoMapQuickItem::coordinate()
{
    return coordinate_;
}

/*!
    \qmlproperty object MapQuickItem::sourceItem

    This property holds the source item that will be drawn on the map.
*/
void QDeclarativeGeoMapQuickItem::setSourceItem(QQuickItem* sourceItem)
{
    if (sourceItem == sourceItem_)
        return;
    sourceItem_ = sourceItem;

    updateMapItem();

    emit sourceItemChanged();
}

QQuickItem* QDeclarativeGeoMapQuickItem::sourceItem()
{
    return sourceItem_;
}

/*!
    \qmlproperty QPointF MapQuickItem::anchorPoint

    This property determines which point on the sourceItem that will be lined
    up with the coordinate on the map.
*/
void QDeclarativeGeoMapQuickItem::setAnchorPoint(const QPointF &anchorPoint)
{
    if (anchorPoint == anchorPoint_)
        return;
    anchorPoint_ = anchorPoint;
    updateMapItem();
    emit anchorPointChanged();
}

QPointF QDeclarativeGeoMapQuickItem::anchorPoint() const
{
    return anchorPoint_;
}

/*!
    \qmlproperty qreal MapQuickItem::zoomLevel

    This property controls the scaling behaviour of the contents of the
    MapQuickItem. In particular, by setting this property it is possible
    to choose between objects that are drawn on the screen (and sized in
    screen pixels), and those drawn on the map surface (which change size
    with the zoom level of the map).

    The default value for this property is 0.0, which corresponds to drawing
    the object on the screen surface. If set to another value, the object will
    be drawn on the map surface instead. The value (if not zero) specifies the
    zoomLevel at which the object will be visible at a scale of 1:1 (ie, where
    object pixels and screen pixels are the same). At zoom levels lower than
    this, the object will appear smaller, and at higher zoom levels, appear
    larger. This is in contrast to when this property is set to zero, where
    the object remains the same size on the screen at all zoom levels.
*/
void QDeclarativeGeoMapQuickItem::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel == zoomLevel_)
        return;
    zoomLevel_ = zoomLevel;
    updateMapItem();
    emit zoomLevelChanged();
}

qreal QDeclarativeGeoMapQuickItem::zoomLevel() const
{
    return zoomLevel_;
}

void QDeclarativeGeoMapQuickItem::updateMapItem()
{
    if (!quickMap() && sourceItem_) {
        mapAndSourceItemSet_ = false;
        sourceItem_->setParentItem(0);
        return;
    }

    if (!quickMap() || !map() || !sourceItem_) {
        mapAndSourceItemSet_ = false;
        return;
    }

    if (!mapAndSourceItemSet_ && quickMap() && map() && sourceItem_) {
        mapAndSourceItemSet_ = true;
        sourceItem_->setParentItem(this);
        sourceItem_->setTransformOrigin(QQuickItem::TopLeft);
        connect(sourceItem_, SIGNAL(xChanged()), this, SLOT(updateMapItem()));
        connect(sourceItem_, SIGNAL(yChanged()), this, SLOT(updateMapItem()));
        connect(sourceItem_, SIGNAL(widthChanged()), this, SLOT(updateMapItem()));
        connect(sourceItem_, SIGNAL(heightChanged()), this, SLOT(updateMapItem()));
    }

    sourceItem_->setScale(scaleFactor());
    sourceItem_->setPos(QPointF(0,0));
    setWidth(sourceItem_->width());
    setHeight(sourceItem_->height());
    setPositionOnMap(coordinate()->coordinate(), scaleFactor() * anchorPoint_);
    update();
}

qreal QDeclarativeGeoMapQuickItem::scaleFactor()
{
    qreal scale = 1.0;
    // use 1+x to avoid fuzzy compare against zero
    if (!qFuzzyCompare(1.0 + zoomLevel_, 1.0))
        scale = pow(0.5, zoomLevel_ - map()->cameraData().zoomFactor());
    return scale;
}

#include "moc_qdeclarativegeomapquickitem_p.cpp"

QT_END_NAMESPACE
