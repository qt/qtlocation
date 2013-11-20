/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeomapscene_p.h"

#include "qgeocameradata_p.h"

#include "qgeotilecache_p.h"
#include "qgeotilespec_p.h"

#include <QtPositioning/private/qgeoprojection_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtPositioning/private/qdoublevector3d_p.h>

#include <QHash>

#include <QPointF>

#include <cmath>

#include "qgeomapscene_p_p.h"

#ifndef NO_QT3D_RENDERER
#include <QGLLightParameters>
#include <QGLSceneNode>
#include <QGLCamera>
#endif

QT_BEGIN_NAMESPACE

QGeoMapScene::QGeoMapScene()
    : QObject(),
      d_ptr(new QGeoMapScenePrivate(this)) {}

QGeoMapScene::~QGeoMapScene()
{
    delete d_ptr;
}

void QGeoMapScene::setUseVerticalLock(bool lock)
{
    Q_D(QGeoMapScene);
    d->useVerticalLock_ = lock;
}

void QGeoMapScene::setScreenSize(const QSize &size)
{
    Q_D(QGeoMapScene);
    d->screenSize_ = size;
}

void QGeoMapScene::setTileSize(int tileSize)
{
    Q_D(QGeoMapScene);
    d->tileSize_ = tileSize;
}

void QGeoMapScene::setCameraData(const QGeoCameraData &cameraData)
{
    Q_D(QGeoMapScene);
    d->cameraData_ = cameraData;
    d->intZoomLevel_ = static_cast<int>(std::floor(d->cameraData_.zoomLevel()));
    float delta = cameraData.zoomLevel() - d->intZoomLevel_;
    if (qAbs(delta) < 0.05) {
        d->linearScaling_ = false;
        d->setScalingOnTextures();
    } else {
        d->linearScaling_ = true;
        d->setScalingOnTextures();
    }
    d->sideLength_ = 1 << d->intZoomLevel_;
}

void QGeoMapScene::setVisibleTiles(const QSet<QGeoTileSpec> &tiles)
{
    Q_D(QGeoMapScene);
    d->setVisibleTiles(tiles);
}

void QGeoMapScene::addTile(const QGeoTileSpec &spec, QSharedPointer<QGeoTileTexture> texture)
{
    Q_D(QGeoMapScene);
    d->addTile(spec, texture);
}

QDoubleVector2D QGeoMapScene::screenPositionToMercator(const QDoubleVector2D &pos) const
{
    Q_D(const QGeoMapScene);
    return d->screenPositionToMercator(pos);
}

QDoubleVector2D QGeoMapScene::mercatorToScreenPosition(const QDoubleVector2D &mercator) const
{
    Q_D(const QGeoMapScene);
    return d->mercatorToScreenPosition(mercator);
}

//QGLCamera *QGeoMapScene::camera() const
//{
//    Q_D(const QGeoMapScene);
//    return d->camera_;
//}

//QGLSceneNode *QGeoMapScene::sceneNode() const
//{
//    Q_D(const QGeoMapScene);
//    return d->sceneNode_;
//}

bool QGeoMapScene::verticalLock() const
{
    Q_D(const QGeoMapScene);
    return d->verticalLock_;
}

#ifndef NO_QT3D_RENDERER
void QGeoMapScene::paintGL(QGLPainter *painter)
{
    Q_D(QGeoMapScene);
    d->paintGL(painter);
}
#endif

QSet<QGeoTileSpec> QGeoMapScene::texturedTiles()
{
    Q_D(QGeoMapScene);
    QSet<QGeoTileSpec> textured;
    foreach (const QGeoTileSpec &tile, d->textures_.keys()) {
        textured += tile;
    }
    return textured;
}

QGeoMapScenePrivate::QGeoMapScenePrivate(QGeoMapScene *scene)
    : tileSize_(0),
#ifndef NO_QT3D_RENDERER
      camera_(new QGLCamera()),
      sceneNode_(new QGLSceneNode()),
      light_(new QGLLightParameters()),
#endif
      scaleFactor_(10.0),
      intZoomLevel_(0),
      sideLength_(0),
      minTileX_(-1),
      minTileY_(-1),
      maxTileX_(-1),
      maxTileY_(-1),
      tileZ_(0),
      tileXWrapsBelow_(0),
      mercatorCenterX_(0.0),
      mercatorCenterY_(0.0),
      mercatorWidth_(0.0),
      mercatorHeight_(0.0),
      screenOffsetX_(0.0),
      screenOffsetY_(0.0),
      screenWidth_(0.0),
      screenHeight_(0.0),
      useVerticalLock_(false),
      verticalLock_(false),
      linearScaling_(true),
      q_ptr(scene) {}


QDoubleVector2D QGeoMapScenePrivate::screenPositionToMercator(const QDoubleVector2D &pos) const
{
    double x = mercatorWidth_ * (((pos.x() - screenOffsetX_) / screenWidth_) - 0.5);
    x += mercatorCenterX_;

    if (x > 1.0 * sideLength_)
        x -= 1.0 * sideLength_;
    if (x < 0.0)
        x += 1.0 * sideLength_;

    x /= 1.0 * sideLength_;

    double y = mercatorHeight_ * (((pos.y() - screenOffsetY_) / screenHeight_) - 0.5);
    y += mercatorCenterY_;
    y /= 1.0 * sideLength_;

    return QDoubleVector2D(x, y);
}

QDoubleVector2D QGeoMapScenePrivate::mercatorToScreenPosition(const QDoubleVector2D &mercator) const
{
    double mx = sideLength_ * mercator.x();

    double lb = mercatorCenterX_ - mercatorWidth_ / 2.0;
    if (lb < 0.0)
        lb += sideLength_;
    double ub = mercatorCenterX_ + mercatorWidth_ / 2.0;
    if (sideLength_ < ub)
        ub -= sideLength_;

    double m = (mx - mercatorCenterX_) / mercatorWidth_;

    double mWrapLower = (mx - mercatorCenterX_ - sideLength_) / mercatorWidth_;
    double mWrapUpper = (mx - mercatorCenterX_ + sideLength_) / mercatorWidth_;

    // correct for crossing dateline
    if (qFuzzyCompare(ub - lb + 1.0, 1.0) || (ub < lb) ) {
        if (mercatorCenterX_ < ub) {
            if (lb < mx) {
                 m = mWrapLower;
            }
        } else if (lb < mercatorCenterX_) {
            if (mx <= ub) {
                m = mWrapUpper;
            }
        }
    }

    // apply wrapping if necessary so we don't return unreasonably large pos/neg screen positions
    // also allows map items to be drawn properly if some of their coords are out of the screen
    if ( qAbs(mWrapLower) < qAbs(m) )
        m = mWrapLower;
    if ( qAbs(mWrapUpper) < qAbs(m) )
        m = mWrapUpper;

    double x = screenWidth_ * (0.5 + m);
    double y = screenHeight_ * (0.5 + (sideLength_ * mercator.y() - mercatorCenterY_) / mercatorHeight_);

    return QDoubleVector2D(x + screenOffsetX_, y + screenOffsetY_);
}

void QGeoMapScenePrivate::setTileBounds(const QSet<QGeoTileSpec> &tiles)
{
    if (tiles.isEmpty()) {
        minTileX_ = -1;
        minTileY_ = -1;
        maxTileX_ = -1;
        maxTileY_ = -1;
        tileZ_ = -1;
        return;
    }

    typedef QSet<QGeoTileSpec>::const_iterator iter;
    iter i = tiles.constBegin();
    iter end = tiles.constEnd();

    tileZ_ = i->zoom();

    // determine whether the set of map tiles crosses the dateline.
    // A gap in the tiles indicates dateline crossing
    bool hasFarLeft = false;
    bool hasFarRight = false;
    bool hasMidLeft = false;
    bool hasMidRight = false;

    for (; i != end; ++i) {
        int x = (*i).x();
        if (x == 0)
            hasFarLeft = true;
        else if (x == (sideLength_ - 1))
            hasFarRight = true;
        else if (x == ((sideLength_ / 2) - 1)) {
            hasMidLeft = true;
        } else if (x == (sideLength_ / 2)) {
            hasMidRight = true;
        }
    }

    // if dateline crossing is detected we wrap all x pos of tiles
    // that are in the left half of the map.
    tileXWrapsBelow_ = 0;

    if (hasFarLeft && hasFarRight) {
        if (!hasMidRight) {
            tileXWrapsBelow_ = sideLength_ / 2;
        } else if (!hasMidLeft) {
            tileXWrapsBelow_ = (sideLength_ / 2) - 1;
        }
    }

    // finally, determine the min and max bounds
    i = tiles.constBegin();

    QGeoTileSpec tile = *i;

    int x = tile.x();
    if (tile.x() < tileXWrapsBelow_)
        x += sideLength_;

    minTileX_ = x;
    maxTileX_ = x;
    minTileY_ = tile.y();
    maxTileY_ = tile.y();

    ++i;

    for (; i != end; ++i) {
        tile = *i;
        int x = tile.x();
        if (tile.x() < tileXWrapsBelow_)
            x += sideLength_;

        minTileX_ = qMin(minTileX_, x);
        maxTileX_ = qMax(maxTileX_, x);
        minTileY_ = qMin(minTileY_, tile.y());
        maxTileY_ = qMax(maxTileY_, tile.y());
    }
}


QT_END_NAMESPACE
