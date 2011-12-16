/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativecirclemapitem_p.h"
#include "qdeclarativegeomapquickitem_p.h"
#include "projection_p.h"
#include <cmath>
#include <QPen>
#include <QPainter>

QT_BEGIN_NAMESPACE

//TODO: make export ??

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vertex
{
    QVector2D position;
};

static const qreal qgeocoordinate_EARTH_MEAN_RADIUS = 6371.0072;

inline static qreal qgeocoordinate_degToRad(qreal deg)
{
    return deg * M_PI / 180;
}
inline static qreal qgeocoordinate_radToDeg(qreal rad)
{
    return rad * 180 / M_PI;
}

static void updatePolygon(QPolygonF& points,const Map& map, const QList<QGeoCoordinate> &path, qreal& w, qreal& h)
{

    qreal minX, maxX, minY, maxY;
    //TODO: dateline handling

    for (int i = 0; i < path.size(); ++i) {

        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        QPointF point = map.coordinateToScreenPosition(coord, false);

        if (i == 0) {
            minX = point.x();
            maxX = point.x();
            minY = point.y();
            maxY = point.y();
        } else {
            minX = qMin(point.x(), minX);
            maxX = qMax(point.x(), maxX);
            minY = qMin(point.y(), minY);
            maxY = qMax(point.y(), maxY);
        }
        points.append(point);
    }

    points.translate(-minX, -minY);

    w = maxX - minX;
    h = maxY - minY;
}

static void calcualtePeripheralPoints(QList<QGeoCoordinate>& path, const QGeoCoordinate& center, qreal distance, int steps)
{
    // get angular distance in radians
    distance = distance / (qgeocoordinate_EARTH_MEAN_RADIUS * 1000);

    // We are using horizontal system, we have radius (distance)
    // projected onto Celestial sphere.
    // This way we know the altitude in horizontal system => h = 90 - r;
    // We can now "spin" around with azimuth as a step to get all the points from
    // peripheral of the given circle.
    // To get geographical position we need to change from horizontal system
    // to equatorial system.

    // get location
    qreal lat = qgeocoordinate_degToRad(center.latitude());
    qreal lon = qgeocoordinate_degToRad(center.longitude());

    // precalculate
    qreal cos_h = sin(distance);
    qreal sin_h = cos(distance);
    qreal cos_phi = cos(lat), sin_phi = sin(lat);
    qreal sin_phi_x_sin_h = sin_phi * sin_h;
    qreal cos_phi_x_cos_h = cos_phi * cos_h;
    qreal sin_phi_x_cos_h = sin_phi * cos_h;
    qreal cos_phi_x_sin_h = cos_phi * sin_h;

    for (int i = 0; i < steps; ++i) {

        qreal a = 2 * M_PI * i / steps;
        qreal sin_delta = sin_phi_x_sin_h - cos_phi_x_cos_h * cos(a);
        qreal cos_delta_x_cos_tau = cos_phi_x_sin_h + sin_phi_x_cos_h * cos(a);
        qreal cos_delta_x_sin_tau = -sin(a) * cos_h;
        // get the hour angle (use Cartesian to polar conversion)
        qreal tau = atan2(cos_delta_x_sin_tau, cos_delta_x_cos_tau);
        qreal cos_delta = sqrt(cos_delta_x_sin_tau
                               * cos_delta_x_sin_tau + cos_delta_x_cos_tau
                               * cos_delta_x_cos_tau);
        // get declination ( use Cartesian to polar conversion )
        qreal delta = atan2(sin_delta, cos_delta);
        // get right ascension from tau , use a greenwich star time of 0
        qreal alpha = lon - tau;
        qreal lat2 = qgeocoordinate_radToDeg(delta);
        qreal lon2 = qgeocoordinate_radToDeg(alpha);
        if (lon2 < -180.0) {
            lon2 += 360.0;
        } else if (lon2 > 180.0) {
            lon2 -= 360.0;
        }
        path << QGeoCoordinate(lat2, lon2, 0.0f);
    }
}

QDeclarativeCircleMapItem::QDeclarativeCircleMapItem(QQuickItem *parent):
    QDeclarativeGeoMapItemBase(parent),
    center_(0),
    mapCircleNode_(0),
    radius_(0),
    zoomLevel_(0.0),
    dragActive_(false)

{
    setFlag(ItemHasContents, true);
    QObject::connect(&border_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(handleBorderUpdated()));
    QObject::connect(&border_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(handleBorderUpdated()));
}

QDeclarativeCircleMapItem::~QDeclarativeCircleMapItem()
{
}

QDeclarativeMapLineProperties *QDeclarativeCircleMapItem::border()
{
    return &border_;
}

void QDeclarativeCircleMapItem::handleBorderUpdated()
{
    updateMapItem(true);
}

void QDeclarativeCircleMapItem::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap, map);
    if (map)
        QObject::connect(map, SIGNAL(cameraDataChanged(CameraData)),
                         this, SLOT(handleCameraDataChanged(CameraData)));
}

void QDeclarativeCircleMapItem::setCenter(QDeclarativeCoordinate *center)
{
    if (center_ == center)
        return;
    if (center_)
        center_->disconnect(this);
    center_ = center;

    if (center_) {
        connect(center_, SIGNAL(latitudeChanged(double)), this,
                SLOT(updateMapItem()));
        connect(center_, SIGNAL(longitudeChanged(double)), this,
                SLOT(updateMapItem()));
        connect(center_, SIGNAL(altitudeChanged(double)), this,
                SLOT(updateMapItem()));
    }

    updateMapItem(true);
    emit centerChanged(center_);
}

QDeclarativeCoordinate* QDeclarativeCircleMapItem::center()
{
    return center_;
}

void QDeclarativeCircleMapItem::setColor(const QColor &color)
{
    if (color_ == color)
        return;
    color_ = color;
    updateMapItem(false);
    emit colorChanged(color_);
}

QColor QDeclarativeCircleMapItem::color() const
{
    return color_;
}

void QDeclarativeCircleMapItem::setRadius(qreal radius)
{

    if (radius_ == radius)
        return;

    radius_ = radius;
    updateMapItem(true);
    emit radiusChanged(radius);
}

qreal QDeclarativeCircleMapItem::radius() const
{
    return radius_;
}


QSGNode* QDeclarativeCircleMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapCircleNode *node = static_cast<MapCircleNode*>(oldNode);

    if (!node) {
        mapCircleNode_ = new MapCircleNode();
        updateMapItem(true);
    }

    mapCircleNode_->update();
    return mapCircleNode_;
}

void QDeclarativeCircleMapItem::updateMapItem(bool dirtyGeometry)
{
    if (!map() || !center() || !center()->coordinate().isValid() || !mapCircleNode_)
        return;

    mapCircleNode_->setBrushColor(color_);
    mapCircleNode_->setPenColor(border_.color());
    mapCircleNode_->setLineWidth(border_.width());

    if (dirtyGeometry)
        mapCircleNode_->setGeometry(*map(), radius(),center()->coordinate());

    const QSizeF& size = mapCircleNode_->size();

    setWidth(size.width());
    setHeight(size.height());

    setPositionOnMap(center()->coordinate(), QPointF(size.width(),size.height()) / 2);
    update();
}

void QDeclarativeCircleMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    if (cameraData.zoomFactor() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomFactor();
        updateMapItem(true);
    }
    else {
        updateMapItem(false);
    }
}

void QDeclarativeCircleMapItem::dragEnded()
{
    if (!dragActive_)
        return;
    dragActive_ = false;
    QPointF newPoint = QPointF(x(),y()) + QPointF(width(), height()) / 2;
    QGeoCoordinate newCoordinate = map()->screenPositionToCoordinate(newPoint, false);
    if (newCoordinate.isValid()) {
        internalCoordinate_.setCoordinate(newCoordinate);
        setCenter(&internalCoordinate_);
    }
}

void QDeclarativeCircleMapItem::dragStarted()
{
    dragActive_ = true;
}

bool QDeclarativeCircleMapItem::contains(QPointF point)
{
    return mapCircleNode_->contains(point);
}


//////////////////////////////////////////////////////////////////////

MapCircleNode::MapCircleNode():
    fillColor_(Qt::black),
    borderColor_(Qt::black),
    borderWidth_(3),
    border_(new MapPolylineNode()),
    geometry_(QSGGeometry::defaultAttributes_Point2D(),0)
{
    geometry_.setDrawingMode(GL_TRIANGLE_FAN);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);

    appendChildNode(border_);
}

MapCircleNode::~MapCircleNode() {}

void MapCircleNode::update()
{
    //TODO: optimize , perform calculation only if polygon has changed
    if (polygon_.size()==0)
        return;

    QSGGeometry *fill = QSGGeometryNode::geometry();

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(polygon_.size() + 1 + 1); //one for center + one to close the circle

    Vertex *vertices = (Vertex *)fill->vertexData();

    //set center
    vertices[fillVertexCount++].position = QVector2D(size_.width()/2,size_.height()/2);

    for (int i = 0; i < polygon_.size(); ++i) {
        vertices[fillVertexCount++].position = QVector2D(polygon_.at(i));
    }
    //close circle
    vertices[fillVertexCount++].position = QVector2D(polygon_.at(0));

    Q_ASSERT(fillVertexCount == fill->vertexCount());

    markDirty(DirtyGeometry);

    if (fillColor_ != fill_material_.color()) {
        fill_material_.setColor(fillColor_);
        setMaterial(&fill_material_);
    }

    border_->setPenColor(borderColor_);
    border_->setLineWidth(borderWidth_);

    border_->update();

    //TODO: implement me : gradient
}

void MapCircleNode::setBrushColor(const QColor &color)
{
    fillColor_= color;
}

QColor MapCircleNode::brushColor() const
{
    return fillColor_;
}

void MapCircleNode::setPenColor(const QColor &color)
{
    borderColor_ = color;
}

QColor MapCircleNode::penColor() const
{
    return borderColor_;
}

qreal MapCircleNode::lineWidth() const
{
    return borderWidth_;
}

void MapCircleNode::setLineWidth(qreal width)
{
    borderWidth_ = width;
}

bool MapCircleNode::contains(QPointF point)
{
    return polygon_.containsPoint(point, Qt::OddEvenFill);
}

void MapCircleNode::setGeometry(const Map& map, qreal radius, const QGeoCoordinate &center)
{
    path_.clear();
    calcualtePeripheralPoints(path_, center, radius, 125);

    qreal w = 0;
    qreal h = 0;

    polygon_.clear();
    updatePolygon(polygon_, map, path_, w, h);
    size_ = QSizeF(w, h);

    QList<QGeoCoordinate> pathClosed = path_;
    pathClosed.append(pathClosed.at(0));
    border_->setGeometry(map, pathClosed);
}

QT_END_NAMESPACE
