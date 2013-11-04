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

#include <Qt3D/qglscenenode.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglmaterial.h>
#include <Qt3D/qgltexture2d.h>
#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/QGLLightParameters>


#include <QHash>

#include <QPointF>

#include <cmath>

QT_BEGIN_NAMESPACE

class QGeoMapScenePrivate
{
public:
    explicit QGeoMapScenePrivate(QGeoMapScene *scene);
    ~QGeoMapScenePrivate();

    QSize screenSize_; // in pixels
    int tileSize_; // the pixel resolution for each tile
    QGeoCameraData cameraData_;
    QSet<QGeoTileSpec> visibleTiles_;

    QGLCamera *camera_;
    QGLSceneNode *sceneNode_;
    QGLLightParameters* light_;

    // scales up the tile geometry and the camera altitude, resulting in no visible effect
    // other than to control the accuracy of the render by keeping the values in a sensible range
    double scaleFactor_;

    // rounded down, positive zoom is zooming in, corresponding to reduced altitude
    int intZoomLevel_;

    // mercatorToGrid transform
    // the number of tiles in each direction for the whole map (earth) at the current zoom level.
    // it is 1<<zoomLevel
    int sideLength_;

    QHash<QGeoTileSpec, QGLSceneNode *> nodes_;
    QHash<QGeoTileSpec, QSharedPointer<QGeoTileTexture> > textures_;
    QList<QSharedPointer<QGeoTileTexture> > newUploads_;

    // tilesToGrid transform
    int minTileX_; // the minimum tile index, i.e. 0 to sideLength which is 1<< zoomLevel
    int minTileY_;
    int maxTileX_;
    int maxTileY_;
    int tileZ_;    // caches the zoom level for this tile set
    int tileXWrapsBelow_; // the wrap point as a tile index

    // cameraToGrid transform
    double mercatorCenterX_; // centre of camera in grid space (0 to sideLength)
    double mercatorCenterY_;
    double mercatorWidth_;   // width of camera in grid space (0 to sideLength)
    double mercatorHeight_;

    // screenToWindow transform
    double screenOffsetX_; // in pixels
    double screenOffsetY_; // in pixels
    // cameraToScreen transform
    double screenWidth_; // in pixels
    double screenHeight_; // in pixels

    bool useVerticalLock_;
    bool verticalLock_;
    bool linearScaling_;

    void addTile(const QGeoTileSpec &spec, QSharedPointer<QGeoTileTexture> texture);

    QDoubleVector2D screenPositionToMercator(const QDoubleVector2D &pos) const;
    QDoubleVector2D mercatorToScreenPosition(const QDoubleVector2D &mercator) const;

    void setVisibleTiles(const QSet<QGeoTileSpec> &tiles);
    void removeTiles(const QSet<QGeoTileSpec> &oldTiles);
    void updateTiles(const QSet<QGeoTileSpec> &tiles);
    QGeometryData buildGeometry(const QGeoTileSpec &spec);
    QGLSceneNode *buildSceneNodeFromGeometry(const QGeometryData &geom);
    void setTileBounds(const QSet<QGeoTileSpec> &tiles);
    void setupCamera();
    void setScalingOnTextures();

    void paintGL(QGLPainter *painter);

private:
    QGeoMapScene *q_ptr;
    Q_DECLARE_PUBLIC(QGeoMapScene)
};

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

QGLCamera *QGeoMapScene::camera() const
{
    Q_D(const QGeoMapScene);
    return d->camera_;
}

QGLSceneNode *QGeoMapScene::sceneNode() const
{
    Q_D(const QGeoMapScene);
    return d->sceneNode_;
}

bool QGeoMapScene::verticalLock() const
{
    Q_D(const QGeoMapScene);
    return d->verticalLock_;
}

void QGeoMapScene::paintGL(QGLPainter *painter)
{
    Q_D(QGeoMapScene);
    d->paintGL(painter);
}

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
      camera_(new QGLCamera()),
      sceneNode_(new QGLSceneNode()),
      light_(new QGLLightParameters()),
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

QGeoMapScenePrivate::~QGeoMapScenePrivate()
{
    delete sceneNode_;
    delete camera_;
    delete light_;
}

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

QGeometryData QGeoMapScenePrivate::buildGeometry(const QGeoTileSpec &spec)
{
    int x = spec.x();

    if (x < tileXWrapsBelow_)
        x += sideLength_;

    if ((x < minTileX_)
            || (maxTileX_ < x)
            || (spec.y() < minTileY_)
            || (maxTileY_ < spec.y())
            || (spec.zoom() != tileZ_)) {
        return 0;
    }

    double edge = scaleFactor_ * tileSize_;

    double x1 = (x - minTileX_);
    double x2 = x1 + 1.0;

    double y1 = (minTileY_ - spec.y());
    double y2 = y1 - 1.0;

    x1 *= edge;
    x2 *= edge;
    y1 *= edge;
    y2 *= edge;

    QGeometryData g;

    QDoubleVector3D n = QDoubleVector3D(0, 0, 1);

    //Texture coordinate order for veritcal flip of texture
    g.appendVertex(QVector3D(x1, y1, 0.0));
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(0.0, 0.0));

    g.appendVertex(QVector3D(x1, y2, 0.0));
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(0.0, 1.0));

    g.appendVertex(QVector3D(x2, y2, 0.0));
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(1.0, 1.0));

    g.appendVertex(QVector3D(x2, y1, 0.0));
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(1.0, 0.0));

    return g;
}

QGLSceneNode *QGeoMapScenePrivate::buildSceneNodeFromGeometry(const QGeometryData &geom)
{
    QGLBuilder builder;
    builder.addQuads(geom);
    return builder.finalizedSceneNode();
}


void QGeoMapScenePrivate::setScalingOnTextures()
{
    if (!linearScaling_) {
        foreach (const QSharedPointer<QGeoTileTexture> &tex, textures_.values()) {
            tex->texture->setBindOptions(tex->texture->bindOptions() &
                                         (~QGLTexture2D::LinearFilteringBindOption));
        }
    } else {
        foreach (const QSharedPointer<QGeoTileTexture> &tex, textures_.values()) {
            tex->texture->setBindOptions(tex->texture->bindOptions() |
                                         (QGLTexture2D::LinearFilteringBindOption));
        }
    }
}

void QGeoMapScenePrivate::addTile(const QGeoTileSpec &spec, QSharedPointer<QGeoTileTexture> texture)
{
    if (!visibleTiles_.contains(spec)) // Don't add the geometry if it isn't visible
        return;
    if (linearScaling_) {
        texture->texture->setBindOptions(texture->texture->bindOptions() |
                                         (QGLTexture2D::LinearFilteringBindOption));
    } else {
        texture->texture->setBindOptions(texture->texture->bindOptions() &
                                         (~QGLTexture2D::LinearFilteringBindOption));
    }

    //Avoid expensive conversion of ARGB32_Premultiplied to ARGB32
    if (texture->texture->image().format() == QImage::Format_ARGB32_Premultiplied) {
        texture->texture->setBindOptions(texture->texture->bindOptions() |
                                         (QGLTexture2D::PremultipliedAlphaBindOption));
    }

    //There are tiles for different zoom levels, no need for mipmaps
    texture->texture->setBindOptions(texture->texture->bindOptions() & (~QGLTexture2D::MipmapBindOption));

    //We flip the texture coordinates instead of the texture
    texture->texture->setBindOptions(texture->texture->bindOptions() & (~QGLTexture2D::InvertedYBindOption));

    QGLSceneNode *node = nodes_.value(spec, 0);
    if (!node) {
        QGeometryData geom = buildGeometry(spec);
        node = buildSceneNodeFromGeometry(geom);
        if (!node)
            return;

        QGLMaterial *mat = new QGLMaterial(node);
        mat->setTexture(texture->texture);
        node->setEffect(QGL::LitDecalTexture2D);
        node->setMaterial(mat);

        sceneNode_->addNode(node);
        nodes_.insert(spec, node);
        textures_.insert(spec, texture);
        newUploads_ << texture;

    } else {
        // TODO handle texture updates when we make node removal more efficient
        if (textures_[spec].data() != texture.data()) {
            textures_.insert(spec, texture);
            node->material()->setTexture(texture->texture);
            newUploads_ << texture;
        }
    }
}

// return true if new tiles introduced in [tiles]
void QGeoMapScenePrivate::setVisibleTiles(const QSet<QGeoTileSpec> &tiles)
{
    Q_Q(QGeoMapScene);

    // detect if new tiles introduced
    bool newTilesIntroduced = !visibleTiles_.contains(tiles);

    // work out the tile bounds for the new scene
    setTileBounds(tiles);

    // set up the gl camera for the new scene
    setupCamera();

    QSet<QGeoTileSpec> toRemove = visibleTiles_ - tiles;
    QSet<QGeoTileSpec> toUpdate = visibleTiles_ - toRemove;
    if (!toRemove.isEmpty())
        removeTiles(toRemove);

    // only need to update tiles when the bounds have changed,
    if (visibleTiles_ != tiles && !toUpdate.isEmpty())
        updateTiles(toUpdate);

    visibleTiles_ = tiles;
    if (newTilesIntroduced)
        emit q->newTilesVisible(visibleTiles_);
}

void QGeoMapScenePrivate::updateTiles(const QSet<QGeoTileSpec> &tiles)
{
    typedef QSet<QGeoTileSpec>::const_iterator iter;
    iter i = tiles.constBegin();
    iter end = tiles.constEnd();
    for (; i != end; ++i) {
        QGeoTileSpec tile = *i;
        QGLSceneNode *node = nodes_.value(tile, 0);

        if (node) {
            QGeometryData geom = buildGeometry(tile);
            // if the new geometry (after wrapping) is the same as the old one,
            // it can be reused
            if ( node->children().size() > 0) {
                if (node->children().front()->geometry() == geom)
                    continue;
            }

            sceneNode_->removeNode(node);
            QGLSceneNode *newNode = buildSceneNodeFromGeometry(geom);
            if (newNode) {
                QGLMaterial *mat = new QGLMaterial(newNode);
                mat->setTexture(textures_[tile]->texture);
                newNode->setEffect(QGL::LitDecalTexture2D);
                newNode->setMaterial(mat);
                sceneNode_->addNode(newNode);
                nodes_.insert(tile, newNode);
            } else {
                nodes_.remove(tile);
                textures_.remove(tile);
            }
            delete node;
        }
    }
}

void QGeoMapScenePrivate::removeTiles(const QSet<QGeoTileSpec> &oldTiles)
{
    typedef QSet<QGeoTileSpec>::const_iterator iter;
    iter i = oldTiles.constBegin();
    iter end = oldTiles.constEnd();

    for (; i != end; ++i) {
        QGeoTileSpec tile = *i;
        QGLSceneNode *node = nodes_.value(tile, 0);
        if (node) {
            // TODO protect with mutex?
            sceneNode_->removeNode(node);
            nodes_.remove(tile);
            textures_.remove(tile);
            delete node;
        }
    }
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

void QGeoMapScenePrivate::setupCamera()
{

    double f = 1.0 * qMin(screenSize_.width(), screenSize_.height());

    // fraction of zoom level
    double z = std::pow(2.0, cameraData_.zoomLevel() - intZoomLevel_);

    // calculate altitdue that allows the visible map tiles
    // to fit in the screen correctly (note that a larger f will cause
    // the camera be higher, resulting in gray areas displayed around
    // the tiles)
    double altitude = scaleFactor_ * f / (2.0 * z);

    double aspectRatio = 1.0 * screenSize_.width() / screenSize_.height();

    double a = f / (z * tileSize_);

    // mercatorWidth_ and mercatorHeight_ define the ratio for
    // mercator and screen coordinate conversion,
    // see mercatorToScreenPosition() and screenPositionToMercator()
    if (aspectRatio > 1.0) {
        mercatorHeight_ = a;
        mercatorWidth_ = a * aspectRatio;
    } else {
        mercatorWidth_ = a;
        mercatorHeight_ = a / aspectRatio;
    }

    // calculate center

    double edge = scaleFactor_ * tileSize_;

    // first calculate the camera center in map space in the range of 0 <-> sideLength (2^z)
    QDoubleVector3D center = (sideLength_ * QGeoProjection::coordToMercator(cameraData_.center()));

    // wrap the center if necessary (due to dateline crossing)
    if (center.x() < tileXWrapsBelow_)
        center.setX(center.x() + 1.0 * sideLength_);

    mercatorCenterX_ = center.x();
    mercatorCenterY_ = center.y();

    // work out where the camera center is w.r.t minimum tile bounds
    center.setX(center.x() - 1.0 * minTileX_);
    center.setY(1.0 * minTileY_ - center.y());

    // letter box vertically
    if (useVerticalLock_ && (mercatorHeight_ > 1.0 * sideLength_)) {
        center.setY(-1.0 * sideLength_ / 2.0);
        mercatorCenterY_ = sideLength_ / 2.0;
        screenOffsetY_ = screenSize_.height() * (0.5 - sideLength_ / (2 * mercatorHeight_));
        screenHeight_ = screenSize_.height() - 2 * screenOffsetY_;
        mercatorHeight_ = 1.0 * sideLength_;
        verticalLock_ = true;
    } else {
        screenOffsetY_ = 0.0;
        screenHeight_ = screenSize_.height();
        verticalLock_ = false;
    }

    if (mercatorWidth_ > 1.0 * sideLength_) {
        screenOffsetX_ = screenSize_.width() * (0.5 - (sideLength_ / (2 * mercatorWidth_)));
        screenWidth_ = screenSize_.width() - 2 * screenOffsetX_;
        mercatorWidth_ = 1.0 * sideLength_;
    } else {
        screenOffsetX_ = 0.0;
        screenWidth_ = screenSize_.width();
    }

    // apply necessary scaling to the camera center
    center *= edge;

    // calculate eye

    QDoubleVector3D eye = center;
    eye.setZ(altitude);

    // calculate up

    QDoubleVector3D view = eye - center;
    QDoubleVector3D side = QDoubleVector3D::normal(view, QDoubleVector3D(0.0, 1.0, 0.0));
    QDoubleVector3D up = QDoubleVector3D::normal(side, view);

    // old bearing, tilt and roll code
    //    QMatrix4x4 mBearing;
    //    mBearing.rotate(-1.0 * camera.bearing(), view);
    //    up = mBearing * up;

    //    QDoubleVector3D side2 = QDoubleVector3D::normal(up, view);
    //    QMatrix4x4 mTilt;
    //    mTilt.rotate(camera.tilt(), side2);
    //    eye = (mTilt * view) + center;

    //    view = eye - center;
    //    side = QDoubleVector3D::normal(view, QDoubleVector3D(0.0, 1.0, 0.0));
    //    up = QDoubleVector3D::normal(view, side2);

    //    QMatrix4x4 mRoll;
    //    mRoll.rotate(camera.roll(), view);
    //    up = mRoll * up;

    // near plane and far plane

    double nearPlane = 1.0;
    double farPlane = 4.0 * edge;

    // TODO protect with mutex?
    // set glcamera parameters
    camera_->setCenter(center);
    camera_->setEye(eye);
    camera_->setUpVector(up);
    camera_->setNearPlane(nearPlane);
    camera_->setFarPlane(farPlane);
}

void QGeoMapScenePrivate::paintGL(QGLPainter *painter)
{
    // TODO protect with mutex?

    // TODO add a shortcut here for when we don't need to repeat and clip the map
    // NOTE- this is important as the repeat code below removes a lot of accuracy
    // by converting to float and adding/removing large numbers when at high zoom

    // do any pending upload/releases
    while (!newUploads_.isEmpty()) {
        if (!newUploads_.front()->textureBound) {
            newUploads_.front()->texture->bind();
            newUploads_.front()->texture->clearImage();
            newUploads_.front()->textureBound = true;
        }
        newUploads_.pop_front();
    }

    glEnable(GL_SCISSOR_TEST);

    painter->setScissor(QRect(screenOffsetX_, screenOffsetY_, screenWidth_, screenHeight_));

    painter->setCamera(camera_);

    painter->setMainLight(light_);

    sceneNode_->draw(painter);

    QGLCamera *camera = camera_;

    bool old = camera->blockSignals(true);

    glDisable(GL_DEPTH_TEST);

    double sideLength = scaleFactor_ * tileSize_ * sideLength_;

    QDoubleVector3D c = QDoubleVector3D(camera->center());
    c.setX(c.x() + sideLength);
    camera->setCenter(c);

    QDoubleVector3D e = QDoubleVector3D(camera->eye());
    e.setX(e.x() + sideLength);
    camera->setEye(e);

    painter->setCamera(camera);
    sceneNode_->draw(painter);

    c.setX(c.x() - 2 * sideLength);
    camera->setCenter(c);
    e.setX(e.x() - 2 * sideLength);
    camera->setEye(e);

    painter->setCamera(camera);
    sceneNode_->draw(painter);

    c.setX(c.x() + sideLength);
    camera->setCenter(c);
    e.setX(e.x() + sideLength);
    camera->setEye(e);

    painter->setCamera(camera);
    sceneNode_->draw(painter);

    glEnable(GL_DEPTH_TEST);

    camera->blockSignals(old);
}

QT_END_NAMESPACE
