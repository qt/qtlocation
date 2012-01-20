/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeomap_p.h"
#include "qgeomap_p_p.h"

#include "qgeotilecache_p.h"
#include "qgeomapsphere_p.h"
#include "qgeoprojection_p.h"
#include "qgeoprojection2d_p.h"
#include "qgeotile_p.h"
#include "qgeomapcontroller_p.h"
#include "qdoublevector2d_p.h"
#include "qdoublevector3d_p.h"

#include "qgeocameratiles_p.h"

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

QT_BEGIN_NAMESPACE

QGeoMap::QGeoMap(QGeoTileCache *cache, QObject *parent)
    : QObject(parent)
{
//    d_ptr = new Map3DPrivate(this, cache, 20000.0);

    // edge is 2^max zoom * 4
    d_ptr = new QGeoMapPrivate(this, cache, 20, 256);
}

QGeoMap::~QGeoMap()
{
    delete d_ptr;
}

QGeoTileCache* QGeoMap::tileCache()
{
    Q_D(QGeoMap);
    return d->tileCache();
}

QGeoMapController* QGeoMap::mapController()
{
    Q_D(QGeoMap);
    return d->mapController();
}

void QGeoMap::setMappingManager(QGeoMappingManager *manager)
{
    Q_D(QGeoMap);
    d->setMappingManager(manager);
}

void QGeoMap::paintGL(QGLPainter *painter)
{
    Q_D(QGeoMap);
    d->paintGL(painter);
}

QGLCamera* QGeoMap::glCamera() const
{
    Q_D(const QGeoMap);
    return d->glCamera();
}

void QGeoMap::resize(int width, int height)
{
    Q_D(QGeoMap);
    d->resize(width, height);

    // always emit this signal to trigger items to redraw
    emit cameraDataChanged(d->cameraData());
}

int QGeoMap::width() const
{
    Q_D(const QGeoMap);
    return d->width();
}

int QGeoMap::height() const
{
    Q_D(const QGeoMap);
    return d->height();
}

void QGeoMap::setCameraData(const QGeoCameraData &cameraData)
{
    Q_D(QGeoMap);

    if (cameraData == d->cameraData())
        return;

    d->setCameraData(cameraData);
    update();

    emit cameraDataChanged(d->cameraData());
}

QGeoCameraData QGeoMap::cameraData() const
{
    Q_D(const QGeoMap);
    return d->cameraData();
}

void QGeoMap::update()
{
    Q_D(QGeoMap);
    d->update();
    emit updateRequired();
}

QGeoCoordinate QGeoMap::screenPositionToCoordinate(const QPointF &pos, bool clipToViewport) const
{
    Q_D(const QGeoMap);
    if (clipToViewport) {
        int w = d->width();
        int h = d->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }

    return d->screenPositionToCoordinate(pos);
}

QPointF QGeoMap::coordinateToScreenPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    Q_D(const QGeoMap);
    QPointF pos = d->coordinateToScreenPosition(coordinate);

    if (clipToViewport) {
        int w = d->width();
        int h = d->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QPointF(qQNaN(), qQNaN());
    }

    return pos;
}

void QGeoMap::setActiveMapType(const QGeoMapType type)
{
    Q_D(QGeoMap);
    d->setActiveMapType(type);
}

const QGeoMapType QGeoMap::activeMapType() const
{
    Q_D(const QGeoMap);
    return d->activeMapType();
}

QGeoMapPrivate::QGeoMapPrivate(QGeoMap *parent, QGeoTileCache *cache, int maxZoom, int tileSize)
    : map_(parent),
      cache_(cache),
      manager_(0),
      controller_(0),
      activeMapType_(QGeoMapType()),
      maxZoom_(maxZoom),
      tileSize_(tileSize),
      baseHeight_(100.0)
{
    sphere_ = new QGeoMapSphere(parent, this, cache);
    glCamera_ = new QGLCamera();

    sideLength_ = pow(2.0, 1.0 * maxZoom_) * tileSize;

    projection_ = QSharedPointer<QGeoProjection>(new QGeoProjection2D(baseHeight_, sideLength_));

    cameraTiles_ = new QGeoCameraTiles(projection_);
    cameraTiles_->setTileSize(tileSize_);
    cameraTiles_->setMaximumZoomLevel(maxZoom_);
}

QGeoMapPrivate::~QGeoMapPrivate()
{
    // controller_ is a child of map_, don't need to delete it here

    delete cameraTiles_;

    manager_->deregisterMap(map_);
    delete sphere_;
    delete glCamera_;
    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

QGeoTileCache* QGeoMapPrivate::tileCache()
{
    return cache_;
}

QGLSceneNode* QGeoMapPrivate::createTileNode(const QGeoTile &tile)
{
    QGLSceneNode* node = createTileSpecNode(tile.tileSpec());

    QGLMaterial *mat = new QGLMaterial(node);
    mat->setTexture(tile.texture());
    node->setEffect(QGL::LitDecalTexture2D);
    node->setMaterial(mat);

    return node;
}

void QGeoMapPrivate::setMappingManager(QGeoMappingManager *manager)
{
    if (manager) {
        manager->registerMap(map_);
        pluginString_ = manager->managerName() + QLatin1String("_") + QString::number(manager->managerVersion());
        cameraTiles_->setPluginString(pluginString_);
        sphere_->setMappingManager(manager);
    } else {
        manager->deregisterMap(map_);
    }
    manager_ = manager;
}

QGeoMapController* QGeoMapPrivate::mapController()
{
    if (!controller_)
        controller_ = new QGeoMapController(map_, projection_);
    return controller_;
}

QGLCamera* QGeoMapPrivate::glCamera() const
{
    return glCamera_;
}

void QGeoMapPrivate::setCameraData(const QGeoCameraData &cameraData)
{
    cameraData_ = cameraData;
    cameraData_.setAspectRatio(aspectRatio_);
    cameraData_.setProjection(projection_.toWeakRef());
    updateGlCamera(glCamera_);
    updateFrustum(frustum_);

    cameraTiles_->setCamera(cameraData_);
    visibleTiles_ = cameraTiles_->tiles();

    sphere_->update(visibleTiles_.toList());
}

QGeoCameraData QGeoMapPrivate::cameraData() const
{
    return cameraData_;
}

void QGeoMapPrivate::update()
{
    sphere_->update(visibleTiles_.toList());
}

void QGeoMapPrivate::resize(int width, int height)
{
    width_ = width;
    height_ = height;
    aspectRatio_ = 1.0 * width_ / height_;
    cameraTiles_->setScreenSize(QSize(width, height));
    setCameraData(cameraData_);
}

int QGeoMapPrivate::width() const
{
    return width_;
}

int QGeoMapPrivate::height() const
{
    return height_;
}

double QGeoMapPrivate::aspectRatio() const
{
    return aspectRatio_;
}

void QGeoMapPrivate::setActiveMapType(const QGeoMapType &type)
{
    activeMapType_ = type;
    cameraTiles_->setMapType(type);
    visibleTiles_ = cameraTiles_->tiles();

    map_->update();
}

const QGeoMapType QGeoMapPrivate::activeMapType() const
{
  return activeMapType_;
}

void QGeoMapPrivate::tileFetched(const QGeoTileSpec &spec)
{
    sphere_->tileFetched(spec);
}

QRect QGeoMapPrivate::specToRect(const QGeoTileSpec &tileSpec) const
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

    QDoubleVector3D tl = projection_->mercatorToPoint(QDoubleVector2D(x1, y1));
    QDoubleVector3D tr = projection_->mercatorToPoint(QDoubleVector2D(x2, y1));
    QDoubleVector3D bl = projection_->mercatorToPoint(QDoubleVector2D(x1, y2));
    QDoubleVector3D br = projection_->mercatorToPoint(QDoubleVector2D(x2, y2));

    if (rightEdge) {
        tr.setX(sideLength_);
        br.setX(sideLength_);
    }

    return QRect(bl.x(), bl.y(), br.x() - tl.x() - 1, tl.y() - br.y() - 1);
}

QGLSceneNode* QGeoMapPrivate::createTileSpecNode(const QGeoTileSpec &tileSpec)
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

    QDoubleVector3D tl = projection_->mercatorToPoint(QDoubleVector2D(x1, y1));
    QDoubleVector3D tr = projection_->mercatorToPoint(QDoubleVector2D(x2, y1));
    QDoubleVector3D bl = projection_->mercatorToPoint(QDoubleVector2D(x1, y2));
    QDoubleVector3D br = projection_->mercatorToPoint(QDoubleVector2D(x2, y2));

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

    QDoubleVector3D n = QDoubleVector3D(0, 0, 1);

    g.appendVertex(tl);
    g.appendNormal(n);
    g.appendTexCoord(QDoubleVector2D(tx1 * 1.0 / dz, ty1 * 1.0 / dz));

    g.appendVertex(bl);
    g.appendNormal(n);
    g.appendTexCoord(QDoubleVector2D(tx1 * 1.0 / dz, ty2 * 1.0 / dz));

    g.appendVertex(br);
    g.appendNormal(n);
    g.appendTexCoord(QDoubleVector2D(tx2 * 1.0 / dz, ty2 * 1.0 / dz));

    g.appendVertex(tr);
    g.appendNormal(n);
    g.appendTexCoord(QDoubleVector2D(tx2 * 1.0 / dz, ty1 * 1.0 / dz));

    builder.addQuads(g);

    return builder.finalizedSceneNode();
}

void QGeoMapPrivate::paintGL(QGLPainter *painter)
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

    QDoubleVector3D c = QDoubleVector3D(camera->center());
    c.setX(c.x() + sideLength_);
    camera->setCenter(c);

    QDoubleVector3D e = QDoubleVector3D(camera->eye());
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

void QGeoMapPrivate::updateGlCamera(QGLCamera* glCamera)
{
    bool old = glCamera->blockSignals(true);

    QGeoCameraData camera = cameraData();

    double f = 1.0 * qMin(width(), height()) / tileSize_;

    double altitude = sideLength_ * camera.distance() * f / 2.0;

    QGeoCoordinate coord = camera.center();
    coord.setAltitude(0.0);
    QDoubleVector3D center = projection_->coordToPoint(coord);
    coord.setAltitude(altitude);
    QDoubleVector3D eye = projection_->coordToPoint(coord);

//    if (pow(2.0, cameraData_.zoomFactor()) * tileSize_ < height_) {
//        center.setY(sideLength_ / 2.0);
//        eye.setY(sideLength_ / 2.0);
//    }

    QDoubleVector3D view = eye - center;
    QDoubleVector3D side = QDoubleVector3D::normal(view, QDoubleVector3D(0.0, 1.0, 0.0));
    QDoubleVector3D up = QDoubleVector3D::normal(side, view);

/*
    QMatrix4x4 mBearing;
    mBearing.rotate(-1.0 * camera.bearing(), view);
    up = mBearing * up;

    QDoubleVector3D side2 = QDoubleVector3D::normal(up, view);
    QMatrix4x4 mTilt;
    mTilt.rotate(camera.tilt(), side2);
    eye = (mTilt * view) + center;

    view = eye - center;
    side = QDoubleVector3D::normal(view, QDoubleVector3D(0.0, 1.0, 0.0));
    up = QDoubleVector3D::normal(view, side2);

    QMatrix4x4 mRoll;
    mRoll.rotate(camera.roll(), view);
    up = mRoll * up;
*/

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

QGeoCoordinate QGeoMapPrivate::screenPositionToCoordinate(const QPointF &pos) const
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

    QDoubleVector3D tl = frustum_.topLeftFar();
    QDoubleVector3D tr = frustum_.topRightFar();
    QDoubleVector3D bl = frustum_.bottomLeftFar();

    QDoubleVector3D n = (1 - x - y) * tl + x * tr + y * bl;

    if (eye_.z() == n.z())
        return QGeoCoordinate();

    double alpha = eye_.z() / (eye_.z() - n.z());
    QDoubleVector3D c = (1 - alpha) * eye_ + alpha * n;

    return projection_->pointToCoord(c);
}

QPointF QGeoMapPrivate::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    QDoubleVector3D c = projection_->coordToPoint(coordinate);
    QDoubleVector3D d = QDoubleVector3D(projectionMatrix_.map(c));
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

    QPointF altPointRight(point.x() + side, point.y());
    QPointF altPointLeft(point.x() - side, point.y());

    QPointF ret = point;
    qreal minDist = qAbs(point.x() - width_ / 2.0);
    qreal dist;

    if ((dist = qAbs(altPointRight.x() - width_ / 2.0)) < minDist) {
        ret = altPointRight;
        minDist = dist;
    }
    if ((dist = qAbs(altPointLeft.x() - width_ / 2.0)) < minDist) {
        ret = altPointLeft;
        minDist = dist;
    }

    return ret;
}

void QGeoMapPrivate::updateFrustum(QGeoFrustum &frustum)
{
    frustum.update(glCamera(), cameraData().aspectRatio());
}

QT_END_NAMESPACE
