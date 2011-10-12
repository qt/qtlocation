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

QList<QVector3D> Map2DPrivate::pointsOnLineWithX(const QVector3D &p1, const QVector3D &p2, double x) const
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

QList<QVector3D> Map2DPrivate::pointsOnLineWithY(const QVector3D &p1, const QVector3D &p2, double y) const
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

QList<QVector3D> Map2DPrivate::pointsOnLineWithZ(const QVector3D &p1, const QVector3D &p2, double z) const
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

QList<QVector3D> Map2DPrivate::clipPolygonToMap(const QList<QVector3D> &points) const
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

    if (clipX0) {
        results = splitPolygonX(results, 0.0).second;
    }

    if (clipX1) {
        results = splitPolygonX(results, sideLength_).first;
    }

    if (clipY0) {
        results = splitPolygonY(results, 0.0).second;
    }

    if (clipY1) {
        results = splitPolygonY(results, sideLength_).first;
    }

    return results;
}

QPair<QList<QVector3D>,QList<QVector3D> > Map2DPrivate::splitPolygonY(const QList<QVector3D> &points, double y) const
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

QPair<QList<QVector3D>,QList<QVector3D> > Map2DPrivate::splitPolygonX(const QList<QVector3D> &points, double x) const
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

QList<TileSpec> Map2DPrivate::updateVisibleTiles()
{
    Frustum f = frustum();

    QList<QVector3D> points;

    points.append(pointsOnLineWithZ(f.topLeftNear(), f.topLeftFar(), baseHeight_));
    points.append(pointsOnLineWithZ(f.topRightNear(), f.topRightFar(), baseHeight_));
    points.append(pointsOnLineWithZ(f.bottomLeftNear(), f.bottomLeftFar(), baseHeight_));
    points.append(pointsOnLineWithZ(f.bottomRightNear(), f.bottomRightFar(), baseHeight_));

    points.append(pointsOnLineWithZ(f.topLeftNear(), f.bottomLeftNear(), baseHeight_));
    points.append(pointsOnLineWithZ(f.bottomLeftNear(), f.bottomRightNear(), baseHeight_));
    points.append(pointsOnLineWithZ(f.bottomRightNear(), f.topRightNear(), baseHeight_));
    points.append(pointsOnLineWithZ(f.topRightNear(), f.topLeftNear(), baseHeight_));

    points.append(pointsOnLineWithZ(f.topLeftFar(), f.bottomLeftFar(), baseHeight_));
    points.append(pointsOnLineWithZ(f.bottomLeftFar(), f.bottomRightFar(), baseHeight_));
    points.append(pointsOnLineWithZ(f.bottomRightFar(), f.topRightFar(), baseHeight_));
    points.append(pointsOnLineWithZ(f.topRightFar(), f.topLeftFar(), baseHeight_));

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
            points = clipPolygonToMap(points);
            tiles.append(tilesFromPoints(points.toVector(), false));
        }
    } else {
        points = clipPolygonToMap(points);
        QPair<QList<QVector3D>, QList<QVector3D> > split = splitPolygonX(points, sideLength_ / 2.0);
        if (!split.first.isEmpty()) {
            tiles.append(tilesFromPoints(split.first.toVector(), false));
        }
        if (!split.second.isEmpty()) {
            tiles.append(tilesFromPoints(split.second.toVector(), true));
        }
    }

    return tiles;
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
    QColor defaultItemColor(128, 128, 128, 256);
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
