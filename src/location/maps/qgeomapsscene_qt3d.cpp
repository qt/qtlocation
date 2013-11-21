#include <Qt3D/qglscenenode.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglmaterial.h>
#include <Qt3D/qgltexture2d.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/QGLLightParameters>

#include "qgeomapscene_p_p.h"

#include "qgeotilespec_p.h"
#include "qgeotilecache_p.h"
#include <QtPositioning/private/qgeoprojection_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtPositioning/private/qdoublevector3d_p.h>

QT_BEGIN_NAMESPACE

QGeoMapScenePrivate::~QGeoMapScenePrivate()
{
    delete sceneNode_;
    delete camera_;
    delete light_;
}

QGeometryData QGeoMapScenePrivate::buildGeometryData(const QGeoTileSpec &spec)
{
    const QRectF &rect = buildGeometry(spec);
    if (rect.isNull())
        return 0;

    QGeometryData g;

    QDoubleVector3D n = QDoubleVector3D(0, 0, 1);

    //Texture coordinate order for veritcal flip of texture
    g.appendVertex(QVector3D(rect.x(), rect.y(), 0.0));
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(0.0, 0.0));

    g.appendVertex(QVector3D(rect.x(), rect.y() + rect.height(), 0.0));
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(0.0, 1.0));

    g.appendVertex(QVector3D(rect.x() + rect.width(), rect.y()+rect.height(), 0.0));
    g.appendNormal(n);
    g.appendTexCoord(QVector2D(1.0, 1.0));

    g.appendVertex(QVector3D(rect.x() + rect.width(), rect.y(), 0.0));
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
        QGeometryData geom = buildGeometryData(spec);
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
            QGeometryData geom = buildGeometryData(tile);
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

QRectF QGeoMapScenePrivate::buildGeometry(const QGeoTileSpec &spec)
{
    int x = spec.x();

    if (x < tileXWrapsBelow_)
        x += sideLength_;

    if ((x < minTileX_)
            || (maxTileX_ < x)
            || (spec.y() < minTileY_)
            || (maxTileY_ < spec.y())
            || (spec.zoom() != tileZ_)) {
        return QRectF();
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
    return QRectF(x1, y1, x2-x1, y2-y1);
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
