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
#include "viewporttiles_p.h"
#include "viewporttiles_p_p.h"

#include "viewporttilesold_p.h"
#include "viewporttilesnew_p.h"

#include "tilespec.h"
#include "cameradata.h"
#include "spheregeometry_p.h"
#include "sphere_p.h"

#include <QVector2D>
#include <QVector>

ViewportTiles::ViewportTiles(int codePath)
{
    if (codePath == 1) {
        d_ptr = new ViewportTilesPrivateOld();
    } else {
        d_ptr = new ViewportTilesPrivateNew();
    }
}

ViewportTiles::~ViewportTiles()
{
    delete d_ptr;
}

void ViewportTiles::setZoomLevel(int zoomLevel)
{
    d_ptr->setZoomLevel(zoomLevel);
}

int ViewportTiles::zoomLevel() const
{
    return d_ptr->zoomLevel();
}

void ViewportTiles::setSphere(const SphereGeometry &sphere)
{
    d_ptr->setSphere(sphere);
}

QList<TileSpec> ViewportTiles::visibleTiles(const ViewportCamera &viewportCamera) const
{
    return d_ptr->visibleTiles(viewportCamera);
}

ViewportTilesPrivate::ViewportTilesPrivate()
{
}

ViewportTilesPrivate::~ViewportTilesPrivate()
{
}

void ViewportTilesPrivate::setZoomLevel(int zoomLevel)
{
    zoomLevel_ = zoomLevel;
}

int ViewportTilesPrivate::zoomLevel() const
{
    return zoomLevel_;
}

void ViewportTilesPrivate::setSphere(const SphereGeometry &sphere)
{
    sphere_ = sphere;
}

SphereGeometry ViewportTilesPrivate::sphere() const
{
    return sphere_;
}

bool sorter(const QPair<double, int> &v1, const QPair<double, int> &v2)
{
    return v1.first < v2.first;
}

struct TileMap2
{
    int minY;
    int maxY;
    QHash<int, QPair<int, int> > range;

    void adjust(int tileX, int tileY);
};

void TileMap2::adjust(int tileX, int tileY)
{
    if (minY == -1)
        minY = tileY;
    if (maxY == -1)
        maxY = tileY;

    minY = qMin(minY, tileY);
    maxY = qMax(maxY, tileY);

    if (range.contains(tileY)) {
        QPair<int,int> r = range[tileY];
        range[tileY] = QPair<int, int>(qMin(r.first, tileX), qMax(r.second, tileX));
    } else {
        range[tileY] = QPair<int, int>(tileX, tileX);
    }
}

/*
    This function returns the tiles that are touched by the line
    running from start to end at zoom level  zoom.

    We first determine the tiles that the start and end points are in,
    and from that we determine the minumum and maximum tile x and tile y values.

    Between the minimum and maximum y values we calculate the points at which
    the line intersects the tile boundaries.

    The tile y value is entirely determined by the y value of the point, and
    we do some algebra to find the the point of intersection for a give tile y value.

    For each intersection we store the fraction of the way along the
    line from start-end the intersection occurs and what the new tile y value
    should be.  These are stored in a QList<<QPair<double, int> >

    There is a slight difference between the case of ascending and
    descending y values and which tile value should be stored, which is accounted for.

    We do something similar for the x values, although in this case the ratio of
    the x and z values determines where the line of intersection is.

    We have to treat x > z and x < z as special cases to avoid dividing by zero.

    We sort both QList<<QPair<double, int> > by the double value and initialise tileX
    and tileY to the tile values of the starting point.

    We then look at the double values at the start of both lists.  If one is smaller than
    the other we take it from the list, update tileX or tileY appropriately, and add it to
    the list of resulting tiles.  If both values are equal we need to add (tileX, newTileY),
    (newTileX, tileY) and (newTileX, newTileY) to the results.

    If either of the lists is exhausted we can just add anything that remains in the other list.
*/
void ViewportTilesPrivate::tilesFromArc(const Arc &arc, bool roundUp, TileMap2 &map) const
{
    QVector3D start = arc.start();
    QVector3D end = arc.end();

    int zpow2 = 1 << zoomLevel_;

    QVector2D t1 = sphere().pointToTile(start, zoomLevel_, roundUp);
    QVector2D t2 = sphere().pointToTile(end, zoomLevel_, roundUp);

    QVector<QPair<double, int> > yVals(qAbs(t1.y() - t2.y()) + 2);
    int yIndex = -1;

    yVals[++yIndex] = QPair<double, int>(0.0, t1.y());
    yVals[++yIndex] = QPair<double, int>(1.0, t2.y());

    int yStep = t2.y() - t1.y();

    if (yStep != 0) {

        yStep /= qAbs(yStep);

        int yAdjust = 0;

        int startY = t1.y();
        int endY = t2.y();
        if (yStep < 0) {
            yAdjust = -1;
        } else {
            ++startY;
            ++endY;
        }

        for (int y = startY; y != endY; y += yStep) {
            QVector3D p = sphere().mercatorToPoint(zpow2 / 2.0, y * 1.0 / zpow2);
            QPair<double, double> factors = arc.yIntersectFactor(p.y());

            bool found = false;
            double tf;

            if (factors.first != -1.0) {
                found = true;
                tf = factors.first;

                if (factors.second != -1.0) {
                    double e1 = qAbs(p.y() - arc.interpolate(factors.first).y());
                    double e2 = qAbs(p.y() - arc.interpolate(factors.second).y());
                    if (e2 < e1)
                        tf = factors.second;
                }
            }

            if (found) {
                yVals[++yIndex] = QPair<double, int>(tf, y + yAdjust);
//                qWarning() << "y " << y << " factor " << tf << te;
//            } else {
//                qWarning() << "  no y solution for " << y;
            }
        }

    }

    yVals.resize(yIndex + 1);
    qSort(yVals.begin(), yVals.end(), sorter);

    QVector<QPair<double, int> > xVals(qAbs(t1.x() - t2.x()) + 2);
    int xIndex = -1;

    xVals[++xIndex] = QPair<double, int>(0.0, t1.x());
    xVals[++xIndex] = QPair<double, int>(1.0, t2.x());

    int xStep = t2.x() - t1.x();

    if (xStep != 0) {

        xStep /= qAbs(xStep);

        int xAdjust = 0;

        int startX = t1.x();
        int endX = t2.x();
        if (xStep < 0) {
            xAdjust = -1;
        } else {
            ++startX;
            ++endX;
        }

        for (int x = startX; x != endX; x += xStep) {
            QVector3D p = sphere().mercatorToPoint(x * 1.0 / zpow2, zpow2 / 2.0);
            QPair<double, double> factors = arc.tIntersectFactor(p.x(), p.z());

            double t;
            if (qAbs(p.x()) < qAbs(p.z()))
                t = p.x() / p.z();
            else
                t = p.z() / p.x();

            bool found = false;
            double tf;

            if (factors.first != -1.0) {
                found = true;
                tf = factors.first;

                if (factors.second != -1.0) {
                    QVector3D v = arc.interpolate(factors.first);
                    double e1 = 0.0;
                    if (qAbs(v.x()) < qAbs(v.z()))
                        e1 = qAbs(t - v.x() / v.z());
                    else
                        e1 = qAbs(t - v.z() / v.x());

                    v = arc.interpolate(factors.second);

                    double e2 = 0.0;
                    if (qAbs(v.x()) < qAbs(v.z()))
                        e2 = qAbs(t - v.x() / v.z());
                    else
                        e2 = qAbs(t - v.z() / v.x());

                    if (e2 < e1)
                        tf = factors.second;
                }
            }

            if (found) {
                xVals[++xIndex] = QPair<double, int>(tf, x + xAdjust);
//                qWarning() << "x " << x << " factor " << tf << te;
//            } else {
//                qWarning() << "  no x solution for " << x;
            }
        }
    }

    xVals.resize(xIndex + 1);
    qSort(xVals.begin(), xVals.end(), sorter);

    int tileX = t1.x();
    int tileY = t1.y();

    int ySize = yVals.size();
    int xSize = xVals.size();
    yIndex = 0;
    xIndex = 0;

    map.adjust(tileX, tileY);

    while ((xIndex != xSize) && (yIndex != ySize)) {
        QPair<double, int> x = xVals.at(xIndex);
        QPair<double, int> y = yVals.at(yIndex);
        if (x.first < y.first) {
            tileX = xVals.at(xIndex++).second;
            map.adjust(tileX, tileY);
        } else if (x.first > y.first) {
            tileY = yVals.at(yIndex++).second;
            map.adjust(tileX, tileY);
        } else {
            int tileX2 = xVals.at(xIndex++).second;
            int tileY2 = yVals.at(yIndex++).second;
            map.adjust(tileX, tileY2);
            map.adjust(tileX2, tileY);
            tileX = tileX2;
            tileY = tileY2;
            map.adjust(tileX, tileY);
        }
    }

    for (int x = xIndex; x < xSize; ++x) {
        tileX = xVals.at(x).second;
        map.adjust(tileX, tileY);
    }

    for (int y = yIndex; y < ySize; ++y) {
        tileY = yVals.at(y).second;
        map.adjust(tileX, tileY);
    }
}

/*
  This function takes a list of points at which the view frustum intersects the world sphere,
  and returns the list of visible tiles that are bounded by that list of points.

  It is assumed that the polygon described by the list of points doesn't cross the dateline
   or contain a pole.  The visibleTiles(int zoom) function takes care of that.

  The tiles along the line for each of the points are gathered, and from that we can
  determine which tiles are contained in the polygon.

  The minumum and maximum tile y values are found by a linear scan, after which we
  determine the minimum and maximum x values for each of the y values.  We request all of the
  tile with that y value which have an x value between the minimum and maximum x values (inclusive).
*/
QList<TileSpec> ViewportTilesPrivate::tilesFromSpherePatch(const SpherePatch &patch) const
{
    QList<TileSpec> results;

    int numArcs = patch.arcCount();

    if (numArcs == 0)
        return results;

    TileMap2 map;
    map.minY = -1;
    map.maxY = -1;

    for (int i = 0; i < numArcs; ++i) {
        tilesFromArc(patch.arc(i), patch.roundUpTiles(), map);
    }

    int minY = map.minY;
    int maxY = map.maxY;


    for (int y = minY; y <= maxY; ++y) {
        if (!map.range.contains(y))
            continue;
        int minX = map.range[y].first;
        int maxX = map.range[y].second;
        for (int x = minX; x <= maxX; ++x) {
            results << TileSpec(zoomLevel_, x, y);
        }
    }

    return results;
}

QList<TileSpec> ViewportTilesPrivate::tilesFromSphere(const Sphere &sphere) const
{
    QList<TileSpec> tiles;

//    qWarning() << "sphere";
//    qWarning() << sphere;

    int numPatches = sphere.patches().size();
    for (int i = 0; i < numPatches; ++i) {
        tiles.append(tilesFromSpherePatch(sphere.patches().at(i)));
    }

    return tiles;
}
