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

#include "qdeclarativegeomap_p.h"
#include "qdeclarativegeomapmousearea_p.h"

#include "qdeclarativegeomapscreenitem_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include <Qt3D/qglview.h>
#include <Qt3D/qglsubsurface.h>
#include <QtCore/QCoreApplication>
#include <QtCore/qnumeric.h>
#include <QThread>

#include "tilecache.h"
#include "tile.h"
#include "cameradata.h"
#include "mapcontroller.h"
#include <Qt3D/qglscenenode.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qgeometrydata.h>
#include "qdeclarativegeomapitem_p.h"
#include <cmath>

#include <qgeoserviceprovider.h>
#include <qgeomappingmanager.h>

#include <QDeclarativeContext>
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QModelIndex>
#include <QQuickCanvas>
#include <QSGEngine>
#include <QtGui/QGuiApplication>

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Map QDeclarativeGeoMap
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

    \brief The Map element displays a map.

    The Map element can be used be used to display a map of the world.  The
    bulk of the functionality is provided by a mapping plugin described
    by the Plugin element associated with the Map.

    Various map items can be added to the map.  These map items are
    specified in terms of coordinates and metres.

    MapItems can be directly added to the Map element and it will display them
    automatically. The various objects that can be added include:

    \list
    \o MapItem - The generic QML item
    \endlist

    Of the above list, MapItemView is a special case and not a MapItem as such.
    Here is a small example to illustrate this:

    \snippet todo

    Mouse handling is done by adding MapMouseArea items as children of either
    MapItems or the Map item itself.
*/

QDeclarativeGeoMap::QDeclarativeGeoMap(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      plugin_(0),
      serviceProvider_(0),
      mappingManager_(0),
      zoomLevel_(8.0),
      bearing_(0.0),
      tilt_(0.0),
      center_(0),
//      mapType_(NoMap),
      componentCompleted_(false),
      mappingManagerInitialized_(false),
      flickable_(0),
      pinchArea_(0),
      mouseGrabberItem_(0),
      canvas_(0),
      touchTimer_(-1),
      tileCache_(0)
{
    QLOC_TRACE0;
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    setFlags(QQuickItem::ItemHasContents | QQuickItem::ItemClipsChildrenToShape);

    // Create internal flickable and pinch area.
    tileCache_ = new TileCache();
    map_ = new Map(tileCache_, this);
    flickable_ = new QDeclarativeGeoMapFlickable(this);
    flickable_->setMap(map_);
    pinchArea_ = new QDeclarativeGeoMapPinchArea(this, this);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

// this function is only called & executed in rendering thread
QSGNode* QDeclarativeGeoMap::updatePaintNode(QSGNode* node, UpdatePaintNodeData* data)
{
    Q_UNUSED(node);
    Q_UNUSED(data);
    // add any pending map items
    if (updateMutex_.tryLock()) {
        if (canvas_) {
            for (int i = 0; i < mapItemsPending_.count(); i++) {
                mapItemsPending_.at(i)->setMap(this);
                mapItems_.append(mapItemsPending_.at(i));
                map_->addMapItem(mapItemsPending_.at(i)->mapItem());
            }
            mapItemsPending_.clear();
        }

        // perform any updates needed by items
        // can be optimized if lot of map items: usually the operation is no-op (static map items)
        for (int i = 0; i < mapItems_.count(); ++i)
            mapItems_.at(i)->updateItem();
        updateMutex_.unlock();
    } else {
        QLOC_TRACE1("===== Map item update will be missed, mutex not acquired =====");
    }
    update(); // is this needed
    return QQuickPaintedItem::updatePaintNode(node, data);
}

QDeclarativeGeoMap::~QDeclarativeGeoMap()
{
    // TODO we do not clear the map items atm
//    if (mapData_) {
//        qDeleteAll(mapViews_);
//        // Remove map objects, we can't allow MapItem
//        // to delete the objects because they are owned
//        // by the declarative elements.
//        QList<QDeclarativeGeoMapItem*> objects = MapItems_;
//        for (int i = 0; i < objects.size(); ++i) {
//            mapData_->removeMapItem(objects.at(i)->MapItem());
//        }
//        delete mapData_;
//    }
    mouseAreas_.clear();
}

void QDeclarativeGeoMap::componentComplete()
{
    QLOC_TRACE0;
    componentCompleted_ = true;
    populateMap();
    QQuickItem::componentComplete();
}

QDeclarativeGeoMapFlickable* QDeclarativeGeoMap::flick()
{
    return flickable_;
}

void QDeclarativeGeoMap::itemChange(ItemChange change, const ItemChangeData & data)
{
    QLOC_TRACE0;
    if (change == ItemSceneChange)
        canvas_ = data.canvas;
}

void QDeclarativeGeoMap::populateMap()
{
    QObjectList kids = children();
    for (int i = 0; i < kids.size(); ++i) {
        // dispatch items appropriately
        QDeclarativeGeoMapItemView* mapView = qobject_cast<QDeclarativeGeoMapItemView*>(kids.at(i));
        if (mapView) {
            mapViews_.append(mapView);
            setupMapView(mapView);
            continue;
        }
        QDeclarativeGeoMapItem* mapItem = qobject_cast<QDeclarativeGeoMapItem*>(kids.at(i));
        if (mapItem) {
            addMapItem(mapItem);
        }

        QDeclarativeGeoMapItemBase* mapScreenItem = qobject_cast<QDeclarativeGeoMapItemBase*>(kids.at(i));
        if (mapScreenItem) {
            addMapScreenItem(mapScreenItem);
        }

        QDeclarativeGeoMapMouseArea *mapMouseArea = qobject_cast<QDeclarativeGeoMapMouseArea*>(kids.at(i));
        if (mapMouseArea) {
            mapMouseArea->setMap(this);
            mouseAreas_.append(mapMouseArea);
        }
    }
}

void QDeclarativeGeoMap::setupMapView(QDeclarativeGeoMapItemView *view)
{
    Q_UNUSED(view)
    view->setMapData(this);
    view->repopulate();
}

class ViewportSubsurface : public QGLSubsurface
{
public:
    ViewportSubsurface(QGLAbstractSurface *surface, const QRect &region,
                       qreal adjust)
        : QGLSubsurface(surface, region), m_adjust(adjust) {}

    qreal aspectRatio() const;
    ~ViewportSubsurface() {}

private:
    qreal m_adjust;
};

qreal ViewportSubsurface::aspectRatio() const
{
    return QGLSubsurface::aspectRatio() * m_adjust;
}

void QDeclarativeGeoMap::updateAspectRatio()
{
    if (!mappingManagerInitialized_)
        return;
    map_->resize(width(), height());
    if (!map_->autoUpdate())
        map_->update();
}

void QDeclarativeGeoMap::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    setSize(QSizeF(newGeometry.width(), newGeometry.height()));
    updateAspectRatio();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

// Note: this keyboard handling is for development purposes only
void QDeclarativeGeoMap::keyPressEvent(QKeyEvent *e)
{
    if (!mappingManagerInitialized_)
        return;
    QLOC_TRACE2(" key: ", e->key());
    CameraData cameraData = map_->cameraData();
    if (e->key() == Qt::Key_Left) {
        if (e->modifiers() & Qt::ShiftModifier) {
            map_->mapController()->pan(-1.0 * map_->width() / 8.0, 0);
            cameraData = map_->cameraData();
        } else {
            cameraData.setBearing(cameraData.bearing() - 5.0);
        }
    } else if (e->key() == Qt::Key_Right) {
        if (e->modifiers() & Qt::ShiftModifier) {
            map_->mapController()->pan(map_->width() / 8.0, 0);
            cameraData = map_->cameraData();
        } else {
            cameraData.setBearing(cameraData.bearing() + 5.0);
        }
    } else if (e->key() == Qt::Key_Up) {
        if (e->modifiers() & Qt::ShiftModifier) {
            map_->mapController()->pan(0, map_->height() / 8.0);
            cameraData = map_->cameraData();
        } else {
            cameraData.setTilt(cameraData.tilt() - 5.0);
        }
    } else if (e->key() == Qt::Key_Down) {
        if (e->modifiers() & Qt::ShiftModifier) {
            map_->mapController()->pan(0, -1.0 * map_->height() / 8.0);
            cameraData = map_->cameraData();
        } else {
            cameraData.setTilt(cameraData.tilt() + 5.0);
        }
    } else if (e->key() == Qt::Key_Plus) {
        if (e->modifiers() & Qt::ShiftModifier) {
            cameraData.setDistance(cameraData.distance() / 2.0);
            cameraData.setZoomLevel(cameraData.zoomLevel() + 1);
            map_->setCameraData(cameraData);
            if (!map_->autoUpdate())
                map_->update();
        } else {
            cameraData.setDistance(cameraData.distance() / 1.1);
        }
    } else if (e->key() == Qt::Key_Minus) {
        if (e->modifiers() & Qt::ShiftModifier) {
            if (cameraData.zoomLevel() != 1)
                cameraData.setZoomLevel(cameraData.zoomLevel() - 1);
            cameraData.setDistance(cameraData.distance() * 2.0);
            map_->setCameraData(cameraData);
            if (!map_->autoUpdate())
                map_->update();
        } else {
            cameraData.setDistance(cameraData.distance() * 1.1);
        }
    } else if (e->key() == Qt::Key_U) {
        map_->setCameraData(cameraData);
        if (!map_->autoUpdate())
            map_->update();
    }
    map_->setCameraData(cameraData);
    update();
}

void QDeclarativeGeoMap::paint(QPainter* p)
{
    if (!isVisible() || !mappingManagerInitialized_)
        return;

    QGLPainter painter(p);
    if (!painter.begin()) {
        qmlInfo(this) << tr("GL graphics system is not active; cannot use 3D items");
        return;
    }
    // No stereo rendering, set the eye as neutral
    painter.setEye(QGL::NoEye);
    // TODO this needs to be figured out (or confirmed as invalid thing).
    // Currently applied transforms for this Map3D element - how to get/apply current transforms?
    // QTransform transform = painter->combinedTransform();
    // Then we get the rectangle that is gotten by applying the QTransform on the rect
    // --> this is the viewport for Map3D
    // QRect viewport = transform.mapRect(boundingRect()).toRect();

    // boundingRect is in local coordinates. We need to map it to the scene coordinates
    // in order to render to correct area.
    QRect viewport = mapRectToScene(boundingRect()).toRect();
    qreal adjust = 1.0f;
    ViewportSubsurface surface(painter.currentSurface(), viewport, adjust);
    painter.pushSurface(&surface);
    earlyDraw(&painter);
    if (map_->glCamera()) {
        painter.setCamera(map_->glCamera());
    } else {
        QGLCamera defCamera;
        painter.setCamera(&defCamera);
    }
    paintGL(&painter);
    // Draw the children items
    painter.popSurface();
    // QSG does not expect anyone to alter gl context state; restore defaults.
    // Default heaps of things, because we cannot be sure what the Qt3D internally
    // sets.
    restoreDefaults(&painter);
}

void QDeclarativeGeoMap::restoreDefaults(QGLPainter *painter)
{
    // Disable the effect to return control to the GL paint engine.
    painter->disableEffect();

    // Try to restore the default options
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    // Set the default depth buffer options.
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
    glDepthRangef(0.0f, 1.0f);
#else
    glDepthRange(0.0f, 1.0f);
#endif
    // Set the default blend options.
    glDisable(GL_BLEND);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter->glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter->glBlendEquation(GL_FUNC_ADD);
    else if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

void QDeclarativeGeoMap::earlyDraw(QGLPainter *painter)
{
    glClearColor(0.0,0.0,0.0,0.0);
    // Depth buffer has been cleared already, but color buffer hasn't
    glClear(GL_COLOR_BUFFER_BIT);
    // Force the effect to be updated.
    painter->disableEffect();
#ifdef GL_RESCALE_NORMAL
    // Scale normals by a scale factor derived from modelview matrix.
    // Note: normals need to be unit length.
    glEnable(GL_RESCALE_NORMAL);
#endif

#if !defined(QT_OPENGL_ES_2)
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
#endif

    // Set the default effect for the scene.
    painter->setStandardEffect(QGL::LitMaterial);
    painter->setFaceColor(QGL::AllFaces, Qt::white);
}

void QDeclarativeGeoMap::paintGL(QGLPainter *painter)
{
    painter->projectionMatrix().scale(1,-1, 1); // qt3d and qsg interpret y differently
    map_->paintGL(painter);
}

/*!
    \qmlproperty Plugin QtLocation5::Map::plugin

    This property holds the plugin which provides the mapping functionality.

    This is write-once property.  Once the map has a plugin associated with
    it any attempted modifications of the plugin will be ignored.
*/

void QDeclarativeGeoMap::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_) {
        qmlInfo(this) << tr("Plugin is a write-once property, and cannot be set again.");
        return;
    }
    plugin_ = plugin;
    emit pluginChanged(plugin_);

    serviceProvider_  = plugin_->sharedGeoServiceProvider();
    mappingManager_ = serviceProvider_->mappingManager();

    if (!mappingManager_  || serviceProvider_->error() != QGeoServiceProvider::NoError) {
           qmlInfo(this) << tr("Warning: Plugin does not support mapping.");
           return;
       }

    pinchArea_->zoomLevelLimits(mappingManager_->minimumZoomLevel(), mappingManager_->maximumZoomLevel());
    if (!mappingManager_->isInitialized())
        connect(mappingManager_, SIGNAL(initialized()), this, SLOT(mappingManagerInitialized()));
    else
        mappingManagerInitialized();
}

// this function will only be ever called once
void QDeclarativeGeoMap::mappingManagerInitialized()
{
    mappingManagerInitialized_ = true;
    if (zoomLevel_ < mappingManager_->minimumZoomLevel())
        setZoomLevel(mappingManager_->minimumZoomLevel());
    else if (zoomLevel_ > mappingManager_->maximumZoomLevel())
        setZoomLevel(mappingManager_->maximumZoomLevel());
    connect(map_,
            SIGNAL(updateRequired()),
            this,
            SLOT(update()));
    connect(map_->mapController(),
            SIGNAL(centerChanged(AnimatableCoordinate)),
            this,
            SLOT(mapCenterChanged(AnimatableCoordinate)));
    connect(map_->mapController(),
            SIGNAL(bearingChanged(qreal)),
            this,
            SLOT(mapBearingChanged(qreal)));
    connect(map_->mapController(),
            SIGNAL(tiltChanged(qreal)),
            this,
            SLOT(mapTiltChanged(qreal)));
    connect(map_->mapController(),
            SIGNAL(zoomChanged(qreal)),
            this,
            SLOT(mapZoomLevelChanged(qreal)));

    map_->setMappingManager(mappingManager_);
    map_->resize(width(), height());
    AnimatableCoordinate acenter = map_->mapController()->center();
    acenter.setCoordinate(center()->coordinate());
    map_->mapController()->setCenter(acenter);
    map_->mapController()->setZoom(zoomLevel_);
    map_->mapController()->setBearing(bearing_);
    map_->mapController()->setTilt(tilt_);
    map_->update();
    emit minimumZoomLevelChanged();
    emit maximumZoomLevelChanged();
}

void QDeclarativeGeoMap::updateMapDisplay(const QRectF &target)
{
    Q_UNUSED(target);
    QQuickItem::update();
}

QDeclarativeGeoServiceProvider* QDeclarativeGeoMap::plugin() const
{
    return plugin_;
}

/*!
    \qmlproperty qreal QtLocation5::Map::minimumZoomLevel

    This property holds the minimum valid zoom level for the map.

    The minimum zoom level is defined by the \l plugin used.
    If plugin supporting mapping is not set, a -1.0 is returned.
*/

qreal QDeclarativeGeoMap::minimumZoomLevel() const
{
    if (mappingManager_)
        return mappingManager_->minimumZoomLevel();
    else
        return -1.0;
}

/*!
\qmlproperty qreal QtLocation5::Map::maximumZoomLevel

    This property holds the maximum valid zoom level for the map.

    The maximum zoom level is defined by the \l plugin used.
    If plugin supporting mapping is not set, a -1.0 is returned.
*/

qreal QDeclarativeGeoMap::maximumZoomLevel() const
{
    if (mappingManager_)
        return mappingManager_->maximumZoomLevel();
    else
        return -1.0;
}

void QDeclarativeGeoMap::setBearing(qreal bearing)
{
    if (bearing_ == bearing)
        return;
    bool clockwise = (bearing >= 0);
    qreal fractions = bearing - int(bearing);
    bearing = (int(qAbs(bearing))) % 359;
    if (!clockwise)
        bearing = (-1.0 * bearing) + 360;
    bearing_ = bearing + fractions;
    if (mappingManagerInitialized_)
        map_->mapController()->setBearing(bearing_);
    emit bearingChanged(bearing_);
}

/* <- put '!' mark here, temporarily undocumented until supported
    \qmlproperty qreal Map::bearing

    This property holds the current bearing (starting from 0 and increasing
    clockwise to 359,9 degrees) pointing up.

    For example setting bearing to 10 will set bearing 10 to point up, which
    visually looks like rotating the map counter-clockwise.

    You can also assign negative values, which will internally get
    translated into positive bearing (e.g. -10 equals 350). This is primarily for
    convenience (e.g. you can decrement bearing without worrying about it).
    Assigning values greater than abs(360) will be mod'd (e.g. 365 will result
    in 5).

    The default value is 0 corresponding North pointing up.
*/

qreal QDeclarativeGeoMap::bearing() const
{
    if (mappingManagerInitialized_) {
        if (map_->mapController()->bearing() >= 0)
            return map_->mapController()->bearing();
        else
            return map_->mapController()->bearing() + 360;
    } else {
        return bearing_;
    }
}

void QDeclarativeGeoMap::setTilt(qreal tilt)
{
    if (tilt_ == tilt || tilt > 85.0 || tilt < 0)
        return;
    tilt_ = tilt;
    if (mappingManagerInitialized_)
        map_->mapController()->setTilt(tilt);
    emit tiltChanged(tilt);
}

qreal QDeclarativeGeoMap::tilt() const
{
    if (!mappingManagerInitialized_)
        return tilt_;
    return map_->mapController()->tilt();
}

/*!
\qmlproperty qreal QtLocation5::Map::zoomLevel

    This property holds the zoom level for the map.

    Larger values for the zoom level provide more detail. Zoom levels
    are always non-negative. The default value is 8.0.
*/
void QDeclarativeGeoMap::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel_ == zoomLevel || zoomLevel < 0)
        return;
    if (mappingManagerInitialized_ &&
            (zoomLevel < mappingManager_->minimumZoomLevel() ||
             zoomLevel > mappingManager_->maximumZoomLevel())) {
        return;
    }
    zoomLevel_ = zoomLevel;
    if (mappingManagerInitialized_)
        map_->mapController()->setZoom(zoomLevel_);
    emit zoomLevelChanged(zoomLevel);
}

qreal QDeclarativeGeoMap::zoomLevel() const
{
    if (mappingManagerInitialized_)
        return map_->mapController()->zoom();
    else
        return zoomLevel_;
}

/*!
\qmlproperty Coordinate QtLocation5::Map::center

    This property holds the coordinate which occupies the center of the
    mapping viewport.

    The default value is an arbitrary valid coordinate.
*/
void QDeclarativeGeoMap::setCenter(QDeclarativeCoordinate *center)
{
    if (center == center_)
        return;
    if (center_)
        center_->disconnect(this);
    center_ = center;
    if (center_) {
        connect(center_,
                SIGNAL(latitudeChanged(double)),
                this,
                SLOT(centerLatitudeChanged(double)));
        connect(center_,
                SIGNAL(longitudeChanged(double)),
                this,
                SLOT(centerLongitudeChanged(double)));
        connect(center_,
                SIGNAL(altitudeChanged(double)),
                this,
                SLOT(centerAltitudeChanged(double)));
        if (center_->coordinate().isValid() && mappingManagerInitialized_) {
            AnimatableCoordinate acoord = map_->mapController()->center();
            acoord.setCoordinate(center_->coordinate());
            map_->mapController()->setCenter(acoord);
            update();
        }
    }
    emit centerChanged(center_);
}

QDeclarativeCoordinate* QDeclarativeGeoMap::center()
{
    if (!center_) {
        if (mappingManagerInitialized_)
            center_ = new QDeclarativeCoordinate(map_->mapController()->center().coordinate());
        else
            center_ = new QDeclarativeCoordinate(QGeoCoordinate(0,0,0));
        connect(center_,
                SIGNAL(latitudeChanged(double)),
                this,
                SLOT(centerLatitudeChanged(double)));
        connect(center_,
                SIGNAL(longitudeChanged(double)),
                this,
                SLOT(centerLongitudeChanged(double)));
        connect(center_,
                SIGNAL(altitudeChanged(double)),
                this,
                SLOT(centerAltitudeChanged(double)));
    }
    return center_;
}

void QDeclarativeGeoMap::mapZoomLevelChanged(qreal zoom)
{
    if (zoom == zoomLevel_)
        return;
    zoomLevel_ = zoom;
    emit zoomLevelChanged(zoomLevel_);
}

void QDeclarativeGeoMap::mapTiltChanged(qreal tilt)
{
    if (tilt == zoomLevel_)
        return;
    tilt_ = tilt;
    emit tiltChanged(tilt_);
}

void QDeclarativeGeoMap::mapBearingChanged(qreal bearing)
{
    if (bearing == bearing_)
        return;
    bearing_ = bearing;
    emit bearingChanged(bearing_);
}

void QDeclarativeGeoMap::mapCenterChanged(AnimatableCoordinate center)
{
    if (center.coordinate() != this->center()->coordinate()) {
        QDeclarativeCoordinate* currentCenter = this->center();
        currentCenter->setCoordinate(center.coordinate());
        emit centerChanged(currentCenter);
    }
}

void QDeclarativeGeoMap::centerLatitudeChanged(double latitude)
{
    if (qIsNaN(latitude))
        return;
    if (mappingManagerInitialized_) {
        AnimatableCoordinate acoord = map_->mapController()->center();
        QGeoCoordinate coord = acoord.coordinate();
        // if the change originated from app, emit (other changes via mapctrl signals)
        if (latitude != coord.latitude())
            emit centerChanged(center_);
        coord.setLatitude(latitude);
        acoord.setCoordinate(coord);
        map_->mapController()->setCenter(acoord);
        update();
    } else {
        emit centerChanged(center_);
    }
}

void QDeclarativeGeoMap::centerLongitudeChanged(double longitude)
{
    if (qIsNaN(longitude))
        return;
    if (mappingManagerInitialized_) {
        AnimatableCoordinate acoord = map_->mapController()->center();
        QGeoCoordinate coord = acoord.coordinate();
        // if the change originated from app, emit (other changes via mapctrl signals)
        if (longitude != coord.longitude())
            emit centerChanged(center_);
        coord.setLongitude(longitude);
        acoord.setCoordinate(coord);
        map_->mapController()->setCenter(acoord);
        update();
    } else {
        emit centerChanged(center_);
    }
}

void QDeclarativeGeoMap::centerAltitudeChanged(double altitude)
{
    if (qIsNaN(altitude))
        return;
    if (mappingManagerInitialized_) {
        AnimatableCoordinate acoord = map_->mapController()->center();
        QGeoCoordinate coord = acoord.coordinate();
        // if the change originated from app, emit (other changes via mapctrl signals)
        if (altitude != coord.altitude())
            emit centerChanged(center_);
        coord.setAltitude(altitude);
        acoord.setCoordinate(coord);
        map_->mapController()->setCenter(acoord);
        update();
    } else {
        emit centerChanged(center_);
    }
}

/*!
\qmlproperty enumeration QtLocation5::Map::mapType

    This property holds the type of map to display.

    The type can be one of:
    \list
    \o Map.StreetMap
    \o Map.SatelliteMapDay
    \o Map.SatelliteMapNight
    \o Map.TerrainMap
    \endlist

    The default value is determined by the plugin.
*/
//void QDeclarativeGeoMap::setMapType(QDeclarativeGeoMap::MapType mapType)
//{
//    if (mapData_) {
//        mapData_->setMapType(QGraphicsGeoMap::MapType(mapType));
//    } else {
//        if (mapType_ == mapType)
//            return;

//        mapType_ = mapType;

//        emit mapTypeChanged(mapType_);
//    }
//}

//QDeclarativeGeoMap::MapType QDeclarativeGeoMap::mapType() const
//{
//    if (mapData_) {
//        return QDeclarativeGeoMap::MapType(mapData_->mapType());
//    } else {
//        return mapType_;
//    }
//}

/*!
    \qmlmethod QtLocation5::Map::toCoordinate(QPointF screenPosition)

    Returns the coordinate which corresponds to the screen position
    \a screenPosition.

    Returns an invalid coordinate if \a screenPosition is not within
    the current viewport.

    An example to constraint landmarks of a model to just those
    currently on Map:
    \snippet examples/declarative-location/landmarkmap/landmarkmap.qml Map toCoordinate

*/

QDeclarativeCoordinate* QDeclarativeGeoMap::toCoordinate(QPointF screenPosition) const
{
    QGeoCoordinate coordinate;
    if (mappingManagerInitialized_)
        coordinate = map_->screenPositionToCoordinate(screenPosition);
    // by default objects returned from method call get javascript ownership,
    // so we don't need to worry about this as long as we don't set the parent
    return new QDeclarativeCoordinate(coordinate);
}

/*!
\qmlmethod QtLocation5::Map::toScreenPosition(Coordinate coordinate)

    Returns the screen position which corresponds to the coordinate
    \a coordinate.

    Returns an invalid QPointF if \a coordinate is not within the
    current viewport.
*/

QPointF QDeclarativeGeoMap::toScreenPosition(QDeclarativeCoordinate* coordinate) const
{
    QPointF point(qQNaN(), qQNaN());
    if (coordinate && mappingManagerInitialized_)
        point = map_->coordinateToScreenPosition(coordinate->coordinate());
    return point;
}

void QDeclarativeGeoMap::pan(int dx, int dy)
{
    if (!mappingManagerInitialized_)
        return;
    map_->mapController()->pan(dx, dy);
}

void QDeclarativeGeoMap::touchEvent(QTouchEvent *event)
{
    if (!mappingManagerInitialized_) {
        event->ignore();
        return;
    }
    QLOC_TRACE0;
    event->accept();
    pinchArea_->touchEvent(event);
}

void QDeclarativeGeoMap::wheelEvent(QWheelEvent *event)
{
    QLOC_TRACE0;
    event->accept();
    emit wheel(event->delta());
}

// event delivery helpers
bool QDeclarativeGeoMap::deliverMouseEvent(QMouseEvent* event)
{
    QLOC_TRACE2("mouse grabber item: ", mouseGrabberItem_);
    // todo deliver first for map items

    // mouse grabber item is an item that has received the initial
    // press event. all events will be given to that item (can be
    // outside of mouse area) until mouse is released. this enables
    // for example entered() exited() -signals whilst mouse is pressed
    lastMousePosition_ = event->windowPos();

    if (!mouseGrabberItem_ &&
            event->type() == QEvent::MouseButtonPress &&
            (event->button() & event->buttons()) == event->buttons()) {
        QList<QDeclarativeGeoMapMouseArea*> mouseAreas = mouseAreasAt(event->pos());
        for (int i = 0; i < mouseAreas.count(); ++i) {
            QDeclarativeGeoMapMouseArea* item = mouseAreas.at(i);
            QLOC_TRACE2("delivering initial mouse press for: ", item->objectName());
            if (deliverInitialMousePressEvent(item, event)) {
                QLOC_TRACE2("initial mouse press accepted by: ", item->objectName());
                return true;
            }
        }
        QLOC_TRACE1("no item found for initial mouse press");
        return false;
    }
    if (mouseGrabberItem_) {
        bool transformOk;
        const QTransform &transform = itemTransform(mouseGrabberItem_, &transformOk);
        QMouseEvent me(event->type(),                     // event type
                       transform.map(event->windowPos()), // pos coordinates translated into that of mapmousearea (local coords)
                       event->windowPos(),                // window position (coords relative to window)
                       event->screenPos(),                // global position (absolute coords)
                       event->button(),                   // button causing the event
                       event->buttons(),                  // buttons pressed when event was caused
                       event->modifiers());               // any keyboard modifiers held when event was triggered
        me.accept();
        mouseGrabberItem_->mouseEvent(&me);
        QLOC_TRACE2("mouse grabber accepted event: ", me.isAccepted());
        event->setAccepted(me.isAccepted());
        if (me.isAccepted())
            return true;
    }
    return false;
}

bool QDeclarativeGeoMap::deliverInitialMousePressEvent(QDeclarativeGeoMapMouseArea* ma, QMouseEvent* event)
{
    if (ma->acceptedMouseButtons() & event->button()) {
        QPointF p = ma->mapFromScene(event->windowPos());
        if (QRectF(0, 0, ma->width(), ma->height()).contains(p)) {
            QMouseEvent me(event->type(), p, event->windowPos(), event->screenPos(),
                           event->button(), event->buttons(), event->modifiers());
                me.accept();
                mouseGrabberItem_ = ma;
                ma->mouseEvent(&me);
                event->setAccepted(me.isAccepted());
                QLOC_TRACE2("the initial mouse press accepted: ", me.isAccepted());
                if (me.isAccepted())
                    return true;
                QLOC_TRACE1("nulling the mouse grabber");
                mouseGrabberItem_ = 0;
            }
        }
    return false;
}

void QDeclarativeGeoMap::mousePressEvent(QMouseEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    if (!mappingManagerInitialized_) {
        event->ignore();
        return;
    }
    bool consumed = deliverMouseEvent(event);
    consumed |= flickable_->mousePressEvent(event);
    if (consumed)
        event->accept();
    else
        event->ignore();
}

// returns list of mouse areas under 'pos'. returned list is in the priority order in which the
// mouse events should be provided (currently does not consider 'z')
QList<QDeclarativeGeoMapMouseArea*> QDeclarativeGeoMap::mouseAreasAt(QPoint pos)
{
    QList<QDeclarativeGeoMapMouseArea*> list;
    for (int i = mouseAreas_.count() - 1; i >= 0; --i) {
        if (mouseAreas_.at(i)->boundingRect().contains(mouseAreas_.at(i)->mapFromScene(pos))) {
            list.append(mouseAreas_.at(i));
        }
    }
    return list;
}

void QDeclarativeGeoMap::mouseReleaseEvent(QMouseEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    if (!mappingManagerInitialized_) {
        event->ignore();
        return;
    }
    bool consumed = false;
    if (mouseGrabberItem_) {
        consumed = deliverMouseEvent(event);
        mouseGrabberItem_ = 0;
    }
    consumed |= flickable_->mouseReleaseEvent(event);
    if (consumed)
        event->accept();
    else
        event->ignore();
}

void QDeclarativeGeoMap::mouseDoubleClickEvent(QMouseEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    if (!mappingManagerInitialized_) {
        event->ignore();
        return;
    }
    if (!mouseGrabberItem_ && (event->button() & event->buttons()) == event->buttons()) {
        QList<QDeclarativeGeoMapMouseArea*> mouseAreas = mouseAreasAt(event->pos());
        for (int i = 0; i < mouseAreas.count(); ++i) {
            if (deliverInitialMousePressEvent(mouseAreas.at(i), event)) {
                event->accept();
                return;
            }
        }
        event->ignore();
        return;
    }
    if (deliverMouseEvent(event))
        event->accept();
    else
        event->ignore();
}

void QDeclarativeGeoMap::mouseMoveEvent(QMouseEvent *event)
{
    //QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    if (!mappingManagerInitialized_) {
        event->ignore();
        return;
    }
    bool consumed = false;
    if (mouseGrabberItem_)
        consumed = deliverMouseEvent(event);
    consumed |= flickable_->mouseMoveEvent(event);
    if (consumed)
        event->accept();
    else
        event->ignore();
}

//void QDeclarativeGeoMap::internalMapTypeChanged(QGraphicsGeoMap::MapType mapType)
//{
//    emit mapTypeChanged(QDeclarativeGeoMap::MapType(mapType));
//}

//void QDeclarativeGeoMap::internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)
//{
//    emit connectivityModeChanged(QDeclarativeGeoMap::ConnectivityMode(connectivityMode));
//}

/*!
    \qmlmethod QtLocation5::Map::addMapItem(MapItem)

    Adds the given MapOject to the Map. If the object already
    is on the Map, it will not be added again.

    As an example, consider you have a MapCircle presenting your current position:

    \snippet doc/src/snippets/declarative/testpolyMapItems.qml Basic map position marker definition
    You can add it to Map (alterntively it can be defined as a child element of the Map):

    \snippet doc/src/snippets/declarative/testpolyMapItems.qml Basic add MapItem
    Note: MapItemViews can not be added with this method.
*/

// all map item additions and removals, whether internal or app originated,
// must go through these functions
void QDeclarativeGeoMap::addMapItem(QDeclarativeGeoMapItem *item)
{
    QLOC_TRACE0;
    if (!item || mapItems_.contains(item) || mapItemsPending_.contains(item))
        return;
    updateMutex_.lock();
    // set pending, we'll add them once we have GL context
    mapItemsPending_.append(item);
    connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(mapItemDestroyed(QObject*)));
    updateMutex_.unlock();
}

void QDeclarativeGeoMap::removeMapItem(QDeclarativeGeoMapItem *item)
{
    QLOC_TRACE0;
    if (!item || !map_)
        return;
    if (mapItemsPending_.contains(item)) {
        mapItemsPending_.removeAll(item);
        return;
    }
    if (!mapItems_.contains(item))
        return;
    updateMutex_.lock();
    item->setMap(0);
    // stop listening to destroyed()
    item->disconnect(this);
    // these can be optmized for perf, as we already check the 'contains' above
    mapItems_.removeOne(item);
    mapItemsPending_.removeOne(item);
    map_->removeMapItem(item->mapItem());
    updateMutex_.unlock();
}

// TODO clears all items including ones from models/MapItemview which is not intended
void QDeclarativeGeoMap::clearMapItems()
{
    if (mapItems_.isEmpty() || !map_)
        return;
    updateMutex_.lock();
    mapItems_.clear();
    mapItemsPending_.clear();
    map_->clearMapItems();
    updateMutex_.unlock();
}

// when a map item is getting destroyed, we need make sure resources are released gracefully
// with QML we must prepare for arbitrary deletion times
void QDeclarativeGeoMap::mapItemDestroyed(QObject* item)
{
    QDeclarativeGeoMapItem* mapItem = qobject_cast<QDeclarativeGeoMapItem*>(item);
    if (mapItem)
        removeMapItem(mapItem);
}

void QDeclarativeGeoMap::addMapScreenItem(QDeclarativeGeoMapItemBase *item)
{
    QLOC_TRACE0;
    if (!item || mapScreenItems_.contains(item))
        return;
    updateMutex_.lock();
    item->setMap(this, map_);
    mapScreenItems_.append(item);
    connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(mapScreenItemDestroyed(QObject*)));
    updateMutex_.unlock();
}

void QDeclarativeGeoMap::removeMapScreenItem(QDeclarativeGeoMapItemBase *item)
{
    QLOC_TRACE0;
    if (!item || !map_)
        return;
    if (!mapScreenItems_.contains(item))
        return;
    updateMutex_.lock();
    item->setMap(0, 0);
    // stop listening to destroyed()
    item->disconnect(this);
    // these can be optmized for perf, as we already check the 'contains' above
    mapScreenItems_.removeOne(item);
    updateMutex_.unlock();
}

void QDeclarativeGeoMap::clearMapScreenItems()
{
    if (mapScreenItems_.isEmpty())
        return;
    updateMutex_.lock();
    mapScreenItems_.clear();
    updateMutex_.unlock();
}

void QDeclarativeGeoMap::mapScreenItemDestroyed(QObject* item)
{
    QDeclarativeGeoMapItemBase* mapScreenItem = qobject_cast<QDeclarativeGeoMapItemBase*>(item);
    if (mapScreenItem)
        removeMapScreenItem(mapScreenItem);
}

void QDeclarativeGeoMap::setActiveMouseArea(QDeclarativeGeoMapMouseArea *area)
{
    Q_UNUSED(area); // TODO
    // to be done when the item picking is clear
}


/*!
    \qmlmethod QtLocation5::Map::removeMapItem(MapItem)

    Removes the given MapItem from the Map. If the MapItem does not
    exist, function does nothing.

    As an example, consider you have a MapCircle presenting your current position:
    \snippet doc/src/snippets/declarative/testpolyMapItems.qml Basic map position marker definition

    You can remove it from the Map element:
    \snippet doc/src/snippets/declarative/testpolyMapItems.qml Basic remove MapItem


*/

// This function is strictly for testing purposes
int QDeclarativeGeoMap::testGetDeclarativeMapItemCount()
{
    return mapItems_.count();
}

#include "moc_qdeclarativegeomap_p.cpp"

QT_END_NAMESPACE
