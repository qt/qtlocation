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
#include "map.h"
#include "map_p.h"

#include "tilecache.h"
#include "mapsphere_p.h"
#include "projection_p.h"
#include "projection2d_p.h"
#include "tile.h"
#include "mapcontroller.h"

#include "qgeomappingmanager.h"

#include <QMutex>
#include <QMap>

#include <qglscenenode.h>
#include <qgeometrydata.h>
#include <qglbuilder.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglsubsurface.h>

#include <cmath>

Map::Map(TileCache *cache, QObject *parent)
    : QObject(parent)
{
//    d_ptr = new Map3DPrivate(this, cache, 20000.0);

    // edge is 2^max zoom * 4
    d_ptr = new MapPrivate(this, cache, 20, 256);
}

Map::~Map()
{
    delete d_ptr;
}

TileCache* Map::tileCache()
{
    return d_ptr->tileCache();
}

MapController* Map::mapController()
{
    return d_ptr->mapController();
}

void Map::setMappingManager(QGeoMappingManager *manager)
{
    d_ptr->setMappingManager(manager);
}

void Map::paintGL(QGLPainter *painter)
{
    d_ptr->paintGL(painter);
}

QGLCamera* Map::glCamera() const
{
    return d_ptr->glCamera();
}

void Map::resize(int width, int height)
{
    d_ptr->resize(width, height);
}

int Map::width() const
{
    return d_ptr->width();
}

int Map::height() const
{
    return d_ptr->height();
}

void Map::setAutoUpdate(bool autoUpdate)
{
    d_ptr->setAutoUpdate(autoUpdate);
}

bool Map::autoUpdate() const
{
    return d_ptr->autoUpdate();
}

void Map::setCameraData(const CameraData &cameraData)
{
    if (cameraData == d_ptr->cameraData())
        return;

    d_ptr->setCameraData(cameraData);

    if (d_ptr->autoUpdate())
            update();

    emit cameraDataChanged(d_ptr->cameraData());
}

CameraData Map::cameraData() const
{
    return d_ptr->cameraData();
}

void Map::update()
{
    d_ptr->update();
    emit updateRequired();
}

QGeoCoordinate Map::screenPositionToCoordinate(const QPointF &pos, bool clipToViewport) const
{
    if (clipToViewport) {
        int w = d_ptr->width();
        int h = d_ptr->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }

    return d_ptr->screenPositionToCoordinate(pos);
}

QPointF Map::coordinateToScreenPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    QPointF pos = d_ptr->coordinateToScreenPosition(coordinate);

    if (clipToViewport) {
        int w = d_ptr->width();
        int h = d_ptr->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QPointF(qQNaN(), qQNaN());
    }

    return pos;
}

void Map::setActiveMapType(const MapType type)
{
    d_ptr->setActiveMapType(type);
}

const MapType Map::activeMapType() const
{
    return d_ptr->activeMapType();
}

//------------------------------------------------------------//

TileMap::TileMap(int minY, int maxY)
    : size(0),
      minY(minY),
      maxY(maxY),
      minX(maxY - minY + 1, -1),
      maxX(maxY - minY + 1, -1)
{}

void TileMap::adjust(int tileX, int tileY)
{
    int index = tileY - minY;
    int min = minX.at(index);
    int max = maxX.at(index);

    if (min == -1) {
        min = tileX;
        max = tileX;
        minX[index] = min;
        maxX[index] = max;
        size += 1;
    } else {
        int oldSize = (max - min);
        int min2 = qMin(min, tileX);
        if (min2 != min)
            minX[index] = min2;
        int max2 = qMax(max, tileX);
        if (max2 != max)
            maxX[index] = max2;
        int newSize = (max2 - min2);
        size += (newSize - oldSize);
    }
}

IntersectGenerator::IntersectGenerator(const MapPrivate *mp,
                                         double p1,
                                         double p2,
                                         int t1,
                                         int t2,
                                         IntersectGenerator::Axis axis,
                                         int zoomLevel)
    : mp_(mp),
      axis_(axis),
      zoomLevel_(zoomLevel)
{
    if (t1 == t2) {
        hasNext_ = false;
        return;
    }

    bool inc = true;
    if (axis_ == IntersectGenerator::XAxis) {
        inc = (0 < (p2 - p1));
    } else {
        inc = (0 < (p1 - p2));
    }

    step_ = 1;
    adjust_ = 0;
    if (!inc) {
        step_ = -1;
        adjust_ = -1;
    }

    first_ = p1;
    denom_ = p2 - p1;

    current_ = t1;
    end_ = t2 + step_;

    hasNext_ = true;

    generateValue();
}

bool IntersectGenerator::hasNext() const
{
    return hasNext_;
}

QPair<double, int> IntersectGenerator::value() const
{
    return value_;
}

void IntersectGenerator::next()
{
    generateValue();
}

void IntersectGenerator::generateValue()
{
    while (current_ != end_) {
        double alpha = 0.0;

        if (axis_ == IntersectGenerator::XAxis) {
            double x = mp_->tileXIntersectToPoint(zoomLevel_, current_).x();
            alpha = (x - first_) / denom_;
        } else {
            double y = mp_->tileYIntersectToPoint(zoomLevel_, current_).y();
            alpha = (y - first_) / denom_;
        }

        if ((0.0 < alpha) && (alpha < 1.0)) {
            value_ = QPair<double,int>(alpha, current_ + adjust_);
            current_ += step_;
            if (current_ == end_)
                hasNext_ = false;
            return;
        }
        current_ += step_;
    }
    hasNext_ = false;
}

//------------------------------------------------------------//

MapPrivate::MapPrivate(Map *parent, TileCache *cache, int maxZoom, int tileSize)
    : autoUpdate_(true),
      map_(parent),
      cache_(cache),
      manager_(0),
      controller_(0),
      activeMapType_(MapType()),
      maxZoom_(maxZoom),
      tileSize_(tileSize),
      baseHeight_(100.0)
{
    sphere_ = new MapSphere(parent, this, cache);
    glCamera_ = new QGLCamera();

    sideLength_ = pow(2.0, 1.0 * maxZoom_) * tileSize;

    projection_ = QSharedPointer<Projection>(new Projection2D(baseHeight_, sideLength_));
    screenPoly_.resize(4);
    screenPoly_[0] = QPointF(0.0, 0.0);
    screenPoly_[1] = QPointF(0.0, sideLength_);
    screenPoly_[2] = QPointF(sideLength_, sideLength_);
    screenPoly_[3] = QPointF(sideLength_, 0.0);

    screenPolyLeft_.resize(4);
    screenPolyLeft_[0] = QPointF(0.0, 0.0);
    screenPolyLeft_[1] = QPointF(0.0, sideLength_);
    screenPolyLeft_[2] = QPointF(sideLength_ / 2.0, sideLength_);
    screenPolyLeft_[3] = QPointF(sideLength_ / 2.0, 0.0);

    screenPolyRight_.resize(4);
    screenPolyRight_[0] = QPointF(sideLength_ / 2.0, 0.0);
    screenPolyRight_[1] = QPointF(sideLength_ / 2.0, sideLength_);
    screenPolyRight_[2] = QPointF(sideLength_, sideLength_);
    screenPolyRight_[3] = QPointF(sideLength_, 0.0);
}

MapPrivate::~MapPrivate()
{
    // controller_ is a child of map_, don't need to delete it here
    manager_->deregisterMap(map_);
    delete sphere_;
    delete glCamera_;
    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

TileCache* MapPrivate::tileCache()
{
    return cache_;
}

QGLSceneNode* MapPrivate::createTileNode(const Tile &tile)
{
    QGLSceneNode* node = createTileSpecNode(tile.tileSpec());

    QGLMaterial *mat = new QGLMaterial(node);
    mat->setTexture(tile.texture());
    node->setEffect(QGL::LitDecalTexture2D);
    node->setMaterial(mat);

    return node;
}

void MapPrivate::setMappingManager(QGeoMappingManager *manager)
{
    if (manager) {
        manager->registerMap(map_);
        pluginString_ = manager->managerName() + QLatin1String("_") + QString::number(manager->managerVersion());
        sphere_->setMappingManager(manager);
    } else {
        manager->deregisterMap(map_);
    }
    manager_ = manager;
}

MapController* MapPrivate::mapController()
{
    if (!controller_)
        controller_ = new MapController(map_, projection_);
    return controller_;
}

QGLCamera* MapPrivate::glCamera() const
{
    return glCamera_;
}

void MapPrivate::setCameraData(const CameraData &cameraData)
{
    cameraData_ = cameraData;
    cameraData_.setAspectRatio(aspectRatio_);
    cameraData_.setProjection(projection_.toWeakRef());
    updateGlCamera(glCamera_);
    updateFrustum(frustum_);
    visibleTiles_ = updateVisibleTiles();
    if (autoUpdate_)
        sphere_->update(visibleTiles_);
}

CameraData MapPrivate::cameraData() const
{
    return cameraData_;
}

void MapPrivate::update()
{
    sphere_->update(visibleTiles_);
}

void MapPrivate::setAutoUpdate(bool autoUpdate)
{
    autoUpdate_ = autoUpdate;
}

bool MapPrivate::autoUpdate() const
{
    return autoUpdate_;
}

void MapPrivate::resize(int width, int height)
{
    width_ = width;
    height_ = height;
    aspectRatio_ = 1.0 * width_ / height_;
    setCameraData(cameraData_);
}

QVector2D MapPrivate::pointToTile(const QVector3D &point, int zoom, bool roundUp) const
{
    QVector2D p = projection_->pointToMercator(point);

    int z = 1 << zoom;
    int x = 0;
    int y = 0;

    if (p.y() == 1.0)
        y = z - 1;
    else
        y = static_cast<int>(z * p.y()) % z;

    if ((qAbs(p.x()) < 1e-6) || (qAbs(p.x() - 1) < 1e-6))
        if (roundUp)
            x = z - 1;
        else
            x = 0;
    else
        x = static_cast<int>(z * p.x()) % z;

    return QVector2D(x, y);
}

QVector3D MapPrivate::tileXIntersectToPoint(int zoomLevel, int x) const
{
    int zpow2 = 1 << zoomLevel;
    return projection_->mercatorToPoint(QVector2D(x * 1.0 / zpow2, zpow2 / 2.0));
}

QVector3D MapPrivate::tileYIntersectToPoint(int zoomLevel, int y) const
{
    int zpow2 = 1 << zoomLevel;
    return projection_->mercatorToPoint(QVector2D(zpow2 / 2.0, y * 1.0 / zpow2));
}

int MapPrivate::width() const
{
    return width_;
}

int MapPrivate::height() const
{
    return height_;
}

double MapPrivate::aspectRatio() const
{
    return aspectRatio_;
}

void MapPrivate::setActiveMapType(const MapType type)
{
    activeMapType_ = type;
    //TODO: check if this shared
    //make it more optimal
    //rewrite current specs
    QList<TileSpec> temp = visibleTiles_;
    visibleTiles_.clear();
    foreach (TileSpec spec,temp) {
      spec.setMapId(type.mapId());
      visibleTiles_ << spec;
    }

    map_->update();
}

const MapType MapPrivate::activeMapType() const
{
  return activeMapType_;
}

void MapPrivate::tileFetched(const TileSpec &spec)
{
    sphere_->tileFetched(spec);
}

QRect MapPrivate::specToRect(const TileSpec &tileSpec) const
{
    int geomZoom = tileSpec.zoom();
    int x = tileSpec.x();
    int y = tileSpec.y();

    int z = 1 << geomZoom;

    bool rightEdge = false;

    double x1 = x * 1.0 / z;
    double x2 = ((x + 1) % z) * 1.0 / z;
    if (x2 == 0.0) {
        x2 = 1.0;
        rightEdge = true;
    }
    double y1 = y * 1.0 / z;
    double y2 = (y + 1) * 1.0 / z;

    QVector3D tl = projection_->mercatorToPoint(QVector2D(x1, y1));
    QVector3D tr = projection_->mercatorToPoint(QVector2D(x2, y1));
    QVector3D bl = projection_->mercatorToPoint(QVector2D(x1, y2));
    QVector3D br = projection_->mercatorToPoint(QVector2D(x2, y2));

    if (rightEdge) {
        tr.setX(sideLength_);
        br.setX(sideLength_);
    }

    return QRect(bl.x(), bl.y(), br.x() - tl.x() - 1, tl.y() - br.y() - 1);
}

QGLSceneNode* MapPrivate::createTileSpecNode(const TileSpec &tileSpec)
{
    int geomZoom = tileSpec.zoom();
    int tileZoom = geomZoom;
    int x = tileSpec.x();
    int y = tileSpec.y();

    QGLBuilder builder;

    int z = 1 << geomZoom;

    bool rightEdge = false;

    double x1 = x * 1.0 / z;
    double x2 = ((x + 1) % z) * 1.0 / z;
    if (x2 == 0.0) {
        x2 = 1.0;
        rightEdge = true;
    }
    double y1 = y * 1.0 / z;
    double y2 = (y + 1) * 1.0 / z;

    QVector3D tl = projection_->mercatorToPoint(QVector2D(x1, y1));
    QVector3D tr = projection_->mercatorToPoint(QVector2D(x2, y1));
    QVector3D bl = projection_->mercatorToPoint(QVector2D(x1, y2));
    QVector3D br = projection_->mercatorToPoint(QVector2D(x2, y2));

    if (rightEdge) {
        tr.setX(sideLength_);
        br.setX(sideLength_);
    }

    int dz = 1 << (geomZoom - tileZoom);

    int tx1 = x % dz;
    int ty1 = y % dz;

    ty1 = dz - ty1;

    int tx2 = (x + 1) % dz;
    if (tx2 == 0)
        tx2 = dz;

    int ty2 = (y + 1) % dz;
    if (ty2 == 0)
        ty2 = dz;

    ty2 = dz - ty2;

    QGeometryData g;

    QVector3D n = QVector3D(0, 0, 1);

    g.appendVertex(tl);
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(tx1 * 1.0 / dz, ty1 * 1.0 / dz));

    g.appendVertex(bl);
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(tx1 * 1.0 / dz, ty2 * 1.0 / dz));

    g.appendVertex(br);
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(tx2 * 1.0 / dz, ty2 * 1.0 / dz));

    g.appendVertex(tr);
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(tx2 * 1.0 / dz, ty1 * 1.0 / dz));

    builder.addQuads(g);

    return builder.finalizedSceneNode();
}

void MapPrivate::paintGL(QGLPainter *painter)
{
    double side = pow(2.0, cameraData_.zoomFactor()) * tileSize_;
    double mapWidth = width_ * 1.0;
    double mapHeight = height_ * 1.0;
    double offsetX = 0.0;
    double offsetY = 0.0;

    if (side < mapWidth) {
        offsetX = (mapWidth - side) / 2.0;
        mapWidth = side;
    }

    if (side < mapHeight) {
        offsetY = (mapHeight - side) / 2.0;
        mapHeight = side;
    }

    glEnable(GL_SCISSOR_TEST);

    painter->setScissor(QRect(offsetX, offsetY, mapWidth, mapHeight));

    QGLCamera *camera = glCamera();

    bool old = camera->blockSignals(true);

    glDisable(GL_DEPTH_TEST);

    QVector3D c = camera->center();
    c.setX(c.x() + sideLength_);
    camera->setCenter(c);

    QVector3D e = camera->eye();
    e.setX(e.x() + sideLength_);
    camera->setEye(e);

    painter->setCamera(camera);
    painter->projectionMatrix().scale(1, -1, 1);
    sphere_->paintGL(painter);

    c.setX(c.x() - 2 * sideLength_);
    camera->setCenter(c);
    e.setX(e.x() - 2 * sideLength_);
    camera->setEye(e);

    painter->setCamera(camera);
    painter->projectionMatrix().scale(1, -1, 1);
    sphere_->paintGL(painter);

    c.setX(c.x() + sideLength_);
    camera->setCenter(c);
    e.setX(e.x() + sideLength_);
    camera->setEye(e);

    painter->setCamera(camera);
    painter->projectionMatrix().scale(1, -1, 1);
    sphere_->paintGL(painter);

    glEnable(GL_DEPTH_TEST);

    camera->blockSignals(old);
}

void MapPrivate::updateGlCamera(QGLCamera* glCamera)
{
    bool old = glCamera->blockSignals(true);

    CameraData camera = cameraData();

    double f = 1.0 * qMin(width(), height()) / tileSize_;

    double altitude = sideLength_ * camera.distance() * f / 2.0;

    QGeoCoordinate coord = camera.center();
    coord.setAltitude(0.0);
    QVector3D center = projection_->coordToPoint(coord);
    coord.setAltitude(altitude);
    QVector3D eye = projection_->coordToPoint(coord);

//    if (pow(2.0, cameraData_.zoomFactor()) * tileSize_ < height_) {
//        center.setY(sideLength_ / 2.0);
//        eye.setY(sideLength_ / 2.0);
//    }

    QVector3D view = eye - center;
    QVector3D side = QVector3D::normal(view, QVector3D(0.0, 1.0, 0.0));
    QVector3D up = QVector3D::normal(side, view);

    QMatrix4x4 mBearing;
    mBearing.rotate(-1.0 * camera.bearing(), view);
    up = mBearing * up;

    QVector3D side2 = QVector3D::normal(up, view);
    QMatrix4x4 mTilt;
    mTilt.rotate(camera.tilt(), side2);
    eye = (mTilt * view) + center;

    view = eye - center;
    side = QVector3D::normal(view, QVector3D(0.0, 1.0, 0.0));
    up = QVector3D::normal(view, side2);

    QMatrix4x4 mRoll;
    mRoll.rotate(camera.roll(), view);
    up = mRoll * up;

    double nearPlane = 1.0;
    double farPlane = 2.0 * altitude;

    glCamera->setCenter(center);
    glCamera->setEye(eye);
    glCamera->setUpVector(up);
    glCamera->setNearPlane(nearPlane);
    glCamera->setFarPlane(farPlane);

    glCamera->blockSignals(old);

    // TODO fold into above code if this works for screen <-> coordinate conversions
    viewSize_ = glCamera->viewSize();
    eye_ = eye;
    projectionMatrix_ = glCamera->projectionMatrix(aspectRatio()) * glCamera->modelViewMatrix();
}

QGeoCoordinate MapPrivate::screenPositionToCoordinate(const QPointF &pos) const
{
    double side = pow(2.0, cameraData_.zoomFactor()) * tileSize_;
    double mapWidth = width_ * 1.0;
    double mapHeight = height_ * 1.0;
    double offsetX = 0.0;
    double offsetY = 0.0;

    if (side < mapWidth) {
        offsetX = (mapWidth - side) / 2.0;
        mapWidth = side;
    }

    if (side < mapHeight) {
        offsetY = (mapHeight - side) / 2.0;
        mapHeight = side;
    }

    double posX = pos.x() - offsetX;
    double posY = pos.y() - offsetY;

    if (posX < 0.0)
        return QGeoCoordinate();
    if (mapWidth < posX)
        return QGeoCoordinate();

    if (posY < 0.0)
        return QGeoCoordinate();
    if (mapHeight < posY)
        return QGeoCoordinate();

    double w = mapWidth / viewSize_.width();
    double h = mapHeight / viewSize_.height();
    double x = (posX - w) / w;
    double y = (posY - h) / h;

    x = (x + 1.0) / 2.0;
    y = (y + 1.0) / 2.0;

    QVector3D tl = frustum_.topLeftFar();
    QVector3D tr = frustum_.topRightFar();
    QVector3D bl = frustum_.bottomLeftFar();

    QVector3D n = (1 - x - y) * tl + x * tr + y * bl;

    if (eye_.z() == n.z())
        return QGeoCoordinate();

    double alpha = eye_.z() / (eye_.z() - n.z());
    QVector3D c = (1 - alpha) * eye_ + alpha * n;

    return projection_->pointToCoord(c);
}

QPointF MapPrivate::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    QVector3D c = projection_->coordToPoint(coordinate);
    QVector3D d = projectionMatrix_.map(c);
    QPointF point =  QPointF((d.x() + 1.0) * width() / 2.0, (-d.y() + 1.0) * height() / 2.0);

    double side = pow(2.0, cameraData_.zoomFactor()) * tileSize_;
    double mapWidth = width_ * 1.0;
    double offsetX = 0.0;

    if (side < mapWidth) {
        offsetX = (mapWidth - side) / 2.0;
        mapWidth = side;

        if (point.x() < offsetX)
            point.setX(point.x() + mapWidth);

        if (offsetX + mapWidth < point.x())
            point.setX(point.x() - mapWidth);
    }

    return point;
}

void MapPrivate::updateFrustum(Frustum &frustum)
{
    frustum.update(glCamera(), cameraData().aspectRatio());
}

QList<TileSpec> MapPrivate::updateVisibleTiles()
{
    QList<QVector3D> points;

    points.append(pointsOnLineWithZ(frustum_.topLeftNear(), frustum_.topLeftFar(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.topRightNear(), frustum_.topRightFar(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.bottomLeftNear(), frustum_.bottomLeftFar(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.bottomRightNear(), frustum_.bottomRightFar(), baseHeight_));

    points.append(pointsOnLineWithZ(frustum_.topLeftNear(), frustum_.bottomLeftNear(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.bottomLeftNear(), frustum_.bottomRightNear(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.bottomRightNear(), frustum_.topRightNear(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.topRightNear(), frustum_.topLeftNear(), baseHeight_));

    points.append(pointsOnLineWithZ(frustum_.topLeftFar(), frustum_.bottomLeftFar(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.bottomLeftFar(), frustum_.bottomRightFar(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.bottomRightFar(), frustum_.topRightFar(), baseHeight_));
    points.append(pointsOnLineWithZ(frustum_.topRightFar(), frustum_.topLeftFar(), baseHeight_));

    QList<TileSpec> tiles;

    if (points.isEmpty())
        return tiles;

    // sort points into a right handed polygon

    LengthSorter sorter;

    // - initial sort to remove duplicates
    sorter.base = points.first();
    qSort(points.begin(), points.end(), sorter);
    for (int i = points.size() - 1; i > 0; --i) {
        if (points.at(i) == points.at(i - 1))
            points.removeAt(i);
    }

    // - proper sort
    //   - start with the first point, put it in the sorted part of the list
    //   - add the nearest unsorted point to the last sorted point to the end
    //     of the sorted points
    QList<QVector3D>::iterator i;
    for (i = points.begin(); i != points.end(); ++i) {
        sorter.base = *i;
        if (i + 1 != points.end())
            qSort(i + 1, points.end(), sorter);
    }

    // - determine if what we have is right handed
    if (points.size() >= 3) {
        QVector3D normal = QVector3D::normal(points.at(1) - points.at(0),
                                             points.at(2) - points.at(1));
        // - if not, reverse the list
        if (normal.z() < 0.0) {
            int s = points.size();
            int s2 = s / 2;
            for (int i = 0; i < s2; ++i) {
                points.swap(i, s - 1 - i);
            }
        }
    }

    // work out if the polygon needs clipping
    // - if we go off the far right edge we need to clip into
    //   two regions - one which rounds down now and one which rounds up
    // - otherwise if we cross an edge of the map we just need to clip
    //   to the map square

    bool round = false;
    bool clip = false;
    for (int i = 0; i < points.size(); ++i) {
        QVector3D p = points.at(i);
        if (p.x() >= sideLength_) {
            round = true;
            break;
        }
        if ((p.x() < 0)
                || (sideLength_ < p.x())
                || (p.y() < 0)
                || (sideLength_ < p.y())) {
            clip = true;
        }
    }

    if (!round) {
        if (!clip) {
            tiles.append(tilesFromPoints(points.toVector(), false));
        } else {
            QPair<QList<QVector3D>,QList<QVector3D> > pair = clipPolygonToMap(points);
            if (!pair.first.isEmpty())
                tiles.append(tilesFromPoints(pair.first.toVector(), true));
            if (!pair.second.isEmpty())
                tiles.append(tilesFromPoints(pair.second.toVector(), false));
        }
    } else {
        QPair<QList<QVector3D>,QList<QVector3D> > pair = clipPolygonToMap(points);
        if (!pair.first.isEmpty()) {
            QPair<QList<QVector3D>, QList<QVector3D> > split = splitPolygonX(pair.first, sideLength_ / 2.0);
            if (!split.first.isEmpty()) {
                tiles.append(tilesFromPoints(split.first.toVector(), false));
            }
            if (!split.second.isEmpty()) {
                tiles.append(tilesFromPoints(split.second.toVector(), true));
            }
        }
        if (!pair.second.isEmpty()) {
            QPair<QList<QVector3D>, QList<QVector3D> > split = splitPolygonX(pair.second, sideLength_ / 2.0);
            if (!split.first.isEmpty()) {
                tiles.append(tilesFromPoints(split.first.toVector(), false));
            }
            if (!split.second.isEmpty()) {
                tiles.append(tilesFromPoints(split.second.toVector(), true));
            }
        }
    }

    return tiles;
}

QList<TileSpec> MapPrivate::tilesFromPoints(const QVector<QVector3D> &points, bool roundUp) const
{
    int numPoints = points.size();

    if (numPoints == 0)
        return QList<TileSpec>();

    int zoomLevel = cameraData().zoomLevel();

    int minY = -1;
    int maxY = -1;

    QVector<QVector2D> tiles(points.size());
    for (int i = 0; i < numPoints; ++i) {
        QVector2D t = pointToTile(points.at(i), zoomLevel, roundUp);
        if (minY == -1) {
            minY = t.y();
            maxY = t.y();
        } else {
            minY = qMin(minY, static_cast<int>(t.y()));
            maxY = qMax(maxY, static_cast<int>(t.y()));
        }
        tiles[i] = t;
    }

    TileMap map(minY, maxY);

    for (int i1 = 0; i1 < numPoints; ++i1) {
        int i2 = (i1 + 1) % numPoints;
        tilesFromLine(points.at(i1), points.at(i2), tiles.at(i1), tiles.at(i2), zoomLevel, map);
    }

    QList<TileSpec> results;

    results.reserve(map.size);

    int size = map.minX.size();
    for (int i = 0; i < size; ++i) {
        int y = map.minY + i;
        int minX = map.minX[i];
        int maxX = map.maxX[i];
        for (int x = minX; x <= maxX; ++x)
            results << TileSpec(pluginString_, activeMapType().mapId(), zoomLevel, x, y);
    }

    return results;
}

void MapPrivate::tilesFromLine(const QVector3D &p1,
                                 const QVector3D &p2,
                                 const QVector2D &t1,
                                 const QVector2D &t2,
                                 int zoomLevel,
                                 TileMap &map) const
{
    IntersectGenerator xGen = IntersectGenerator(this, p1.x(), p2.x(), t1.x(), t2.x(),
                                                 IntersectGenerator::XAxis, zoomLevel);
    IntersectGenerator yGen = IntersectGenerator(this, p1.y(), p2.y(), t1.y(), t2.y(),
                                                 IntersectGenerator::YAxis, zoomLevel);

    int tileX = t1.x();
    int tileY = t1.y();

    map.adjust(tileX, tileY);

    while (xGen.hasNext() && yGen.hasNext()) {
        QPair<double, int> x = xGen.value();
        QPair<double, int> y = yGen.value();
        if (x.first < y.first) {
            tileX = x.second;
            map.adjust(tileX, tileY);
            xGen.next();
        } else if (x.first > y.first) {
            tileY = y.second;
            map.adjust(tileX, tileY);
            yGen.next();
        } else {
            map.adjust(tileX, y.second);
            map.adjust(x.second, tileY);
            tileX = x.second;
            tileY = y.second;
            map.adjust(tileX, tileY);
            xGen.next();
            yGen.next();
        }
    }

    while (xGen.hasNext()) {
        tileX = xGen.value().second;
        map.adjust(tileX, tileY);
        xGen.next();
    }

    while (yGen.hasNext()) {
        tileY = yGen.value().second;
        map.adjust(tileX, tileY);
        yGen.next();
    }
}

QPair<QList<QVector3D>,QList<QVector3D> > MapPrivate::clipPolygonToMap(const QList<QVector3D> &points) const
{
    bool clipX0 = false;
    bool clipX1 = false;
    bool clipY0 = false;
    bool clipY1 = false;
    int size = points.size();
    for (int i = 0; i < size; ++i) {
        QVector3D p = points.at(i);
        if (p.x() < 0.0)
            clipX0 = true;
        if (sideLength_ < p.x())
            clipX1 = true;
        if (p.y() < 0.0)
            clipY0 = true;
        if (sideLength_ < p.y())
            clipY1 = true;

    }

    QList<QVector3D> results = points;

    if (clipY0) {
        results = splitPolygonY(results, 0.0).second;
    }

    if (clipY1) {
        results = splitPolygonY(results, sideLength_).first;
    }

    if (clipX0) {
        if (clipX1) {
            results = splitPolygonX(results, 0.0).second;
            results = splitPolygonX(results, sideLength_).first;
            return QPair<QList<QVector3D>,QList<QVector3D> >(results, QList<QVector3D>());
        } else {
            QPair<QList<QVector3D>,QList<QVector3D> > pair = splitPolygonX(results, 0.0);
            for (int i = 0; i < pair.first.size(); ++i) {
                pair.first[i].setX(pair.first.at(i).x() + sideLength_);
            }
            return pair;
        }
    } else {
        if (clipX1) {
            QPair<QList<QVector3D>,QList<QVector3D> > pair = splitPolygonX(results, sideLength_);
            for (int i = 0; i < pair.second.size(); ++i) {
                pair.second[i].setX(pair.second.at(i).x() - sideLength_);
            }
            return pair;
        } else {
            return QPair<QList<QVector3D>,QList<QVector3D> >(results, QList<QVector3D>());
        }
    }
}

QPair<QList<QVector3D>,QList<QVector3D> > MapPrivate::splitPolygonY(const QList<QVector3D> &points, double y) const
{
    QList<QVector3D> pointsBelow;
    QList<QVector3D> pointsAbove;

    int size = points.size();

    if (size == 0) {
        return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
    }

    bool allAbove = true;
    bool allBelow = true;

    for (int i = 0; i < size; ++i) {
        double py = points.at(i).y();
        if (py < y)
            allAbove = false;
        if (y < py)
            allBelow = false;
    }

    if (allAbove) {
        if (allBelow) {
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
        } else {
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, points);
        }
    } else {
        if (allBelow) {
            return QPair<QList<QVector3D>,QList<QVector3D> >(points, pointsAbove);
        }
    }


    int intersect1 = -1;
    int intersect2 = -1;

    // add intersects

    QList<QVector3D> tmpPoints = points;

    for (int i1 = 0; i1 < size; ++i1) {
        int i2 = (i1 + 1) % size;

        QVector3D p1 = tmpPoints.at(i1);
        QVector3D p2 = tmpPoints.at(i2);

        if (p1.y() == y) {
            if (intersect1 == -1)
                intersect1 = i1;
            else if (intersect2 == -1)
                intersect2 = i1;
            else
                qDebug() << __FUNCTION__ << " more than 2 intersections";
        }

        if (((p1.y() < y) && (y < p2.y()))
                || ((p2.y() < y) && (y < p1.y()))) {
            QList<QVector3D> newPoints = pointsOnLineWithY(p1, p2, y);
            if (newPoints.size() == 1) {
                tmpPoints.insert(i1 + 1, newPoints.at(0));
                ++size;
                // will get added to intersect1 or intersect 2 in next iteration
            }
        }
    }

    // split at intersects
    if ((intersect1 != -1) && (intersect2 != -1)) {

        size = tmpPoints.size();

        bool firstBelow = true;

        for (int i = intersect1; i <= intersect2; ++i) {
            QVector3D p = tmpPoints.at(i);
            if (y < p.y())
                firstBelow = false;
            pointsBelow.append(p);
        }

        for (int i = intersect2; i <= intersect1 + size; ++i) {
            pointsAbove.append(tmpPoints.at(i % size));
        }

        if (firstBelow)
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
        else
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsAbove, pointsBelow);

    } else {
        qDebug() << __FUNCTION__ << " less than 2 intersections";
    }

    return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
}

QPair<QList<QVector3D>,QList<QVector3D> > MapPrivate::splitPolygonX(const QList<QVector3D> &points, double x) const
{
    QList<QVector3D> pointsBelow;
    QList<QVector3D> pointsAbove;

    int size = points.size();

    if (size == 0) {
        return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
    }

    bool allAbove = true;
    bool allBelow = true;

    for (int i = 0; i < size; ++i) {
        double px = points.at(i).x();
        if (px < x)
            allAbove = false;
        if (x < px)
            allBelow = false;
    }

    if (allAbove) {
        if (allBelow) {
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
        } else {
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, points);
        }
    } else {
        if (allBelow) {
            return QPair<QList<QVector3D>,QList<QVector3D> >(points, pointsAbove);
        }
    }

    int intersect1 = -1;
    int intersect2 = -1;

    // add intersects

    QList<QVector3D> tmpPoints = points;

    for (int i1 = 0; i1 < size; ++i1) {
        int i2 = (i1 + 1) % size;

        QVector3D p1 = tmpPoints.at(i1);
        QVector3D p2 = tmpPoints.at(i2);

        if (p1.x() == x) {
            if (intersect1 == -1)
                intersect1 = i1;
            else if (intersect2 == -1)
                intersect2 = i1;
            else
                qDebug() << __FUNCTION__ << " more than 2 intersections";
        }

        if (((p1.x() < x) && (x < p2.x()))
                || ((p2.x() < x) && (x < p1.x()))) {
            QList<QVector3D> newPoints = pointsOnLineWithX(p1, p2, x);
            if (newPoints.size() == 1) {
                tmpPoints.insert(i1 + 1, newPoints.at(0));
                ++size;
                // will get added to intersect1 or intersect 2 in next iteration
            }
        }
    }

    // split at intersects
    if ((intersect1 != -1) && (intersect2 != -1)) {

        size = tmpPoints.size();

        bool firstBelow = true;

        for (int i = intersect1; i <= intersect2; ++i) {
            QVector3D p = tmpPoints.at(i);
            if (x < p.x())
                firstBelow = false;
            pointsBelow.append(p);
        }

        for (int i = intersect2; i <= intersect1 + size; ++i) {
            pointsAbove.append(tmpPoints.at(i % size));
        }

        if (firstBelow)
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
        else
            return QPair<QList<QVector3D>,QList<QVector3D> >(pointsAbove, pointsBelow);

    } else {
        qDebug() << __FUNCTION__ << " less than 2 intersections";
    }

    return QPair<QList<QVector3D>,QList<QVector3D> >(pointsBelow, pointsAbove);
}

QList<QVector3D> MapPrivate::pointsOnLineWithX(const QVector3D &p1, const QVector3D &p2, double x) const
{
    QList<QVector3D> results;

    if (p1.x() == p2.x()) {
        if (p1.x() == x) {
            results.append(p1);
            results.append(p2);
        }
    } else {
        double f = (p1.x() - x) / (p1.x() - p2.x());
        if ((0 <= f) && (f <= 1.0))
            results.append((1 - f) * p1 + f * p2);
    }

    return results;
}

QList<QVector3D> MapPrivate::pointsOnLineWithY(const QVector3D &p1, const QVector3D &p2, double y) const
{
    QList<QVector3D> results;

    if (p1.y() == p2.y()) {
        if (p1.y() == y) {
            results.append(p1);
            results.append(p2);
        }
    } else {
        double f = (p1.y() - y) / (p1.y() - p2.y());
        if ((0 <= f) && (f <= 1.0))
            results.append((1 - f) * p1 + f * p2);
    }

    return results;
}

QList<QVector3D> MapPrivate::pointsOnLineWithZ(const QVector3D &p1, const QVector3D &p2, double z) const
{
    QList<QVector3D> results;

    if (p1.z() == p2.z()) {
        if (p1.z() == z) {
            results.append(p1);
            results.append(p2);
        }
    } else {
        double f = (p1.z() - z) / (p1.z() - p2.z());
        if ((0 <= f) && (f <= 1.0))
            results.append((1 - f) * p1 + f * p2);
    }

    return results;
}
