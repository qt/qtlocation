/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
#include "map2d_p.h"
#include "projection2d_p.h"

#include "tile.h"
#include "mapitem.h"

#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglcamera.h>

#include <cmath>

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
            return;
        }
        current_ += step_;
    }
    hasNext_ = false;
}

Map2DPrivate::Map2DPrivate(Map *parent, TileCache *cache, int maxZoom, int tileSize)
    : MapPrivate(parent, cache),
      maxZoom_(maxZoom),
      tileSize_(tileSize),
      baseHeight_(100.0)
{
    sideLength_ = pow(2.0, 1.0 * maxZoom_);

    setProjection(QSharedPointer<Projection>(new Projection2D(baseHeight_, sideLength_)));
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

Map2DPrivate::~Map2DPrivate() {}

QGLSceneNode* Map2DPrivate::createTileNode(const Tile &tile)
{
    int geomZoom = tile.tileSpec().zoom();
    int tileZoom = geomZoom;
    int x = tile.tileSpec().x();
    int y = tile.tileSpec().y();

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

    QSharedPointer<Projection> p = projection();

    QVector3D tl = p->mercatorToPoint(QVector2D(x1, y1));
    QVector3D tr = p->mercatorToPoint(QVector2D(x2, y1));
    QVector3D bl = p->mercatorToPoint(QVector2D(x1, y2));
    QVector3D br = p->mercatorToPoint(QVector2D(x2, y2));

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

    QGLSceneNode *node = builder.finalizedSceneNode();

    QGLMaterial *mat = new QGLMaterial(node);
//    tile.texture()->bind();
    mat->setTexture(tile.texture());
    node->setEffect(QGL::LitDecalTexture2D);

    node->setMaterial(mat);

    return node;
}

void Map2DPrivate::updateGlCamera(QGLCamera* glCamera)
{
    bool old = glCamera->blockSignals(true);

    CameraData camera = cameraData();

    double f = 1.0 * qMin(width(), height()) / tileSize_;

    double altitude = sideLength_ * camera.distance() * f / 2.0;

    QSharedPointer<Projection> p = projection();

    QGeoCoordinate coord = camera.center();
    coord.setAltitude(0.0);
    QVector3D center = p->coordToPoint(coord);
    coord.setAltitude(altitude);
    QVector3D eye = p->coordToPoint(coord);

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

void Map2DPrivate::updateFrustum(Frustum &frustum)
{
    frustum.update(glCamera(), cameraData().aspectRatio());
}

QList<TileSpec> Map2DPrivate::updateVisibleTiles()
{
    Frustum f = frustum();

    QPolygonF poly(4);

    QVector3D tln = f.topLeftNear();
    QVector3D tlf = f.topLeftFar();
    double tl = (tln.z() - baseHeight_) / (tln.z() - tlf.z());

    poly[0] = ((1 - tl) * tln + tl * tlf).toPointF();

    QVector3D bln = f.bottomLeftNear();
    QVector3D blf = f.bottomLeftFar();
    double bl = (bln.z() - baseHeight_) / (bln.z() - blf.z());

    poly[1] = ((1 - bl) * bln + bl * blf).toPointF();

    QVector3D brn = f.bottomRightNear();
    QVector3D brf = f.bottomRightFar();
    double br = (brn.z() - baseHeight_) / (brn.z() - brf.z());

    poly[2] = ((1 - br) * brn + br * brf).toPointF();

    QVector3D trn = f.topRightNear();
    QVector3D trf = f.topRightFar();
    double tr = (trn.z() - baseHeight_) / (trn.z() - trf.z());

    poly[3] = ((1 - tr) * trn + tr * trf).toPointF();

    bool widePoly = false;
    bool clip = false;
    for (int i = 0; i < poly.size(); ++i) {
        QPointF p = poly.at(i);
        if (p.x() >= sideLength_) {
            widePoly = true;
            break;
        }
        if ((p.x() < 0)
                || (sideLength_ < p.x())
                || (p.y() < 0)
                || (sideLength_ < p.y())) {
            clip = true;
        }
    }

    if (!widePoly) {
        if (!clip) {
            QVector<QVector3D> points(poly.size());

            for (int i = 0; i < poly.size(); ++i)
                points[i] = QVector3D(poly.at(i));

            return tilesFromPoints(points, false);
        } else {
            QPolygonF clipped = intersection(poly, screenPoly_);

            QList<TileSpec> tiles;

            if (!clipped.isEmpty()) {
                QVector<QVector3D> points(clipped.size() - 1);

                for (int i = 0; i < clipped.size() - 1; ++i)
                    points[i] = QVector3D(clipped.at(i));

                return tilesFromPoints(points, false);
            }

            return tiles;
        }

    } else {
        QPolygonF clippedLeft = intersection(poly, screenPolyLeft_);
        QPolygonF clippedRight = intersection(poly, screenPolyRight_);

        QList<TileSpec> tiles;

        if (!clippedLeft.isEmpty()) {
            QVector<QVector3D> pointsLeft(clippedLeft.size() - 1);

            for (int i = 0; i < clippedLeft.size() - 1; ++i)
                pointsLeft[i] = QVector3D(clippedLeft.at(i));

            if (clippedRight.isEmpty())
                return tilesFromPoints(pointsLeft, false);
            else
                tiles.append(tilesFromPoints(pointsLeft, false));
        }

        if (!clippedRight.isEmpty()) {
            QVector<QVector3D> pointsRight(clippedRight.size() - 1);

            for (int i = 0; i < clippedRight.size() - 1; ++i)
                pointsRight[i] = QVector3D(clippedRight.at(i));

            if (tiles.isEmpty())
                return tilesFromPoints(pointsRight, true);
            else
                tiles.append(tilesFromPoints(pointsRight, true));
        }

        return tiles;
    }
}

QPolygonF Map2DPrivate::intersection(const QPolygonF &p1, const QPolygonF &p2) const
{
    QVector<double> x(4);
    QVector<double> y(4);

    x[0] = p1.at(0).x();
    x[1] = p1.at(2).x();
    x[2] = p2.at(0).x();
    x[3] = p2.at(2).x();

    y[0] = p1.at(0).y();
    y[1] = p1.at(2).y();
    y[2] = p2.at(0).y();
    y[3] = p2.at(2).y();

    qSort(x);
    qSort(y);

    QPolygonF result(5);

    result[0] = QPointF(x.at(1), y.at(1));
    result[1] = QPointF(x.at(2), y.at(1));
    result[2] = QPointF(x.at(2), y.at(2));
    result[3] = QPointF(x.at(1), y.at(2));
    result[4] = QPointF(x.at(1), y.at(1));

    return result;
}

void Map2DPrivate::tilesFromLine(const QVector3D &p1,
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

QList<TileSpec> Map2DPrivate::tilesFromPoints(const QVector<QVector3D> &points, bool roundUp) const
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
            results << TileSpec(zoomLevel, x, y);
    }

    return results;
}

void Map2DPrivate::updateMapItemSceneNode(MapItem *item)
{
    double z = item->zoom();

    if (z < 0) {
        // TODO draw item on camera
        return;
    }

    double scale = sideLength_ / (tileSize_ * pow(2, z));
    QPointF anchor = item->anchor() * scale;
    QSizeF size = item->size() * scale;
    double alt = 0.0;
    QVector3D point = projection()->coordToPoint(item->coordinate());
    QVector3D tl = QVector3D(point.x() + anchor.x(), point.y() - anchor.y(), alt);
    QVector3D tr = QVector3D(tl.x() + size.width(), tl.y(), alt);
    QVector3D bl = QVector3D(tl.x(), tl.y() - size.height(), alt);
    QVector3D br = QVector3D(tl.x() + size.width(), tl.y() - size.height(), alt);

    QVector3D up(0.0, 0.0, 1.0);

    QGeometryData g;

    g.appendVertex(tl);
    g.appendNormal(up);
    g.appendTexCoord(QVector2D(0.0, 0.0));

    g.appendVertex(bl);
    g.appendNormal(up);
    g.appendTexCoord(QVector2D(0.0, 1.0));

    g.appendVertex(br);
    g.appendNormal(up);
    g.appendTexCoord(QVector2D(1.0, 1.0));

    g.appendVertex(tr);
    g.appendNormal(up);
    g.appendTexCoord(QVector2D(1.0, 0.0));

    QGLBuilder builder;
    builder.addQuads(g);
    QGLSceneNode *node = builder.finalizedSceneNode();

    QGLMaterial *mat = new QGLMaterial(node);
    QColor defaultItemColor(128, 128, 128, 128);
    mat->setColor(defaultItemColor);
    mat->setSpecularColor(defaultItemColor);
    mat->setDiffuseColor(defaultItemColor);
    mat->setShininess(1.0);
    node->setEffect(QGL::LitMaterial);

    node->setMaterial(mat);
    item->setSceneNode(node);
}

QGeoCoordinate Map2DPrivate::screenPositionToCoordinate(const QPointF &pos) const
{
    double w = width() * 1.0 / viewSize_.width();
    double h = height() * 1.0 / viewSize_.height();
    double x = (pos.x() - w) / w;
    double y = (pos.y() - h) / h;

    x = (x + 1.0) / 2.0;
    y = (y + 1.0) / 2.0;

    QVector3D tl = frustum().topLeftFar();
    QVector3D tr = frustum().topRightFar();
    QVector3D bl = frustum().bottomLeftFar();

    QVector3D n = (1 - x - y) * tl + x * tr + y * bl;

    if (eye_.z() == n.z())
        return QGeoCoordinate();

    double alpha = eye_.z() / (eye_.z() - n.z());
    QVector3D c = (1 - alpha) * eye_ + alpha * n;

    return projection()->pointToCoord(c);
}

QPointF Map2DPrivate::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    QVector3D c = projection()->coordToPoint(coordinate);
    QVector3D d = projectionMatrix_.map(c);
    return QPointF((d.x() + 1.0) * width() / 2.0, (-d.y() + 1.0) * height() / 2.0);
}
