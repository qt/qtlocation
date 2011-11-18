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

#include "map3d_p.h"
#include "projection3d_p.h"

#include "tile.h"

#include <qgeometrydata.h>
#include <qglbuilder.h>
#include <Qt3D/qglcamera.h>

#include <cmath>

Map3DPrivate::Map3DPrivate(Map *parent, TileCache *cache, double radius)
    : MapPrivate(parent, cache),
      minZoom_(6),
      radius_(radius)
{
    Projection3D* p = new Projection3D(radius_);
    setProjection(QSharedPointer<Projection>(p));
    viewport_.setProjection(p);
}

Map3DPrivate::~Map3DPrivate()
{
}

QRect Map3DPrivate::specToRect(const TileSpec &tileSpec) const
{
    return QRect();
}

QGeometryData Map3DPrivate::generateTileGeometryData(int x, int y, int tileZoom, int geomZoom) const
{
    int z = 1 << geomZoom;

    double x1 = x * 1.0 / z;
    double x2 = ((x + 1) % z) * 1.0 / z;
    if (x2 == 0.0)
        x2 = 1.0;
    double y1 = y * 1.0 / z;
    double y2 = (y + 1) * 1.0 / z;

    QSharedPointer<Projection> p = projection();

    QVector3D tl = p->mercatorToPoint(QVector2D(x1, y1));
    QVector3D tr = p->mercatorToPoint(QVector2D(x2, y1));
    QVector3D bl = p->mercatorToPoint(QVector2D(x1, y2));
    QVector3D br = p->mercatorToPoint(QVector2D(x2, y2));

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

    g.appendVertex(tl);
    g.appendNormal(tl / radius_);
    g.appendTexCoord(QVector2D(tx1 * 1.0 / dz, ty1 * 1.0 / dz));

    g.appendVertex(bl);
    g.appendNormal(bl / radius_);
    g.appendTexCoord(QVector2D(tx1 * 1.0 / dz, ty2 * 1.0 / dz));

    g.appendVertex(br);
    g.appendNormal(br / radius_);
    g.appendTexCoord(QVector2D(tx2 * 1.0 / dz, ty2 * 1.0 / dz));

    g.appendVertex(tr);
    g.appendNormal(tr / radius_);
    g.appendTexCoord(QVector2D(tx2 * 1.0 / dz, ty1 * 1.0 / dz));

    return g;
}

QGLSceneNode* Map3DPrivate::createTileSpecNode(const TileSpec &tileSpec)
{
    int tileZoom = tileSpec.zoom();
    int geomZoom = qMax(minZoom_, tileZoom);

    int dz = 1 << (geomZoom - tileZoom);

    int x1 = tileSpec.x() * dz;
    int y1 = tileSpec.y() * dz;

    QGLBuilder builder;

    for (int x = 0; x < dz; ++x) {
        for (int y = 0; y < dz; ++y) {
            builder.addQuads(generateTileGeometryData(x1 + x, y1 + y, tileZoom, geomZoom));
        }
    }

    return builder.finalizedSceneNode();
}

void Map3DPrivate::updateGlCamera(QGLCamera* glCamera)
{
    CameraData camera = cameraData();

    double altitude = radius_ * camera.distance();

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
}

void Map3DPrivate::updateFrustum(Frustum &frustum)
{
    frustum.update(glCamera(), cameraData().aspectRatio());
    viewport_.setCameraData(cameraData());
}

QList<TileSpec> Map3DPrivate::updateVisibleTiles()
{
    return viewport_.visibleTiles();
}

QGeoCoordinate Map3DPrivate::screenPositionToCoordinate(const QPointF &/*pos*/) const
{
    return QGeoCoordinate();
}

QPointF Map3DPrivate::coordinateToScreenPosition(const QGeoCoordinate &/*coordinate*/) const
{
    return QPointF();
}
