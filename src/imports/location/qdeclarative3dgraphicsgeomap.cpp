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

#include "qdeclarative3dgraphicsgeomap_p.h"
#include "qdeclarativegeomapmousearea_p.h"

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
#include <QSGCanvas>
#include <QSGEngine>
#include <QtGui/QGuiApplication>

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Map

    \brief The Map element displays a map.
    \inherits QDeclarativeItem

    \ingroup qml-location-maps

    The Map element can be used be used to display a map of the world.  The
    bulk of the functionality is provided by a mapping plugin described
    by the Plugin element associated with the Map.

    Various map objects can be added to the map.  These map objects are
    specified in terms of coordinates and metres.

    MapObjects can be directly added to the Map element and it will display them
    automatically. The various objects that can be added include:

    \list
    \endlist

    Of the above list, MapObjectView is a special case and not a MapObject as such.
    Here is a small example to illustrate this:

    \snippet doc/src/snippets/declarative/declarative-map.qml Basic MapObjects and View on Map

    Mouse handling is done by adding MapMouseArea items as children of either
    MapObjects or the Map item itself.

    The Map element is part of the \bold{QtLocation 5.0} module.
*/

QDeclarative3DGraphicsGeoMap::QDeclarative3DGraphicsGeoMap(QSGItem *parent)
    : QSGPaintedItem(parent),
      plugin_(0),
      serviceProvider_(0),
      mappingManager_(0),
      center_(0),
      initialCoordinate(0),
//      mapType_(NoMap),
//      connectivityMode_(NoConnectivity),
      componentCompleted_(false),
      flickable_(0),
      pinchArea_(0),
      //hoverItem_(0),
      mouseGrabberItem_(0),
      canvas_(0),
      touchTimer_(-1),
      tileCache_(0)
{
    QLOC_TRACE0;
    initialCoordinate = new QGeoCoordinate(-27.0, 153.0);
    zoomLevel_ = 8;
    size_ = QSizeF(100.0, 100.0);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    setFlags(QSGItem::ItemHasContents);

    tileCache_ = new TileCache();
    map_ = new Map(tileCache_, this);
    connect(map_,
            SIGNAL(updateRequired()),
            this,
            SLOT(update()));
    // Create internal flickable and pinch area.
    flickable_ = new QDeclarativeGeoMapFlickable(map_, this);
    pinchArea_ = new QDeclarativeGeoMapPinchArea(this, this);
    setRenderTarget(QSGPaintedItem::FramebufferObject);
}

// this function is only called & executed in rendering thread
QSGNode* QDeclarative3DGraphicsGeoMap::updatePaintNode(QSGNode* node, UpdatePaintNodeData* data)
{
    Q_UNUSED(node);
    Q_UNUSED(data);
#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
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
#endif
    update(); // is this needed
    return QSGPaintedItem::updatePaintNode(node, data);
}

QDeclarative3DGraphicsGeoMap::~QDeclarative3DGraphicsGeoMap()
{
    // TODO we do not clear the map items atm
//    if (mapData_) {
//        qDeleteAll(mapViews_);
//        // Remove map objects, we can't allow mapObject
//        // to delete the objects because they are owned
//        // by the declarative elements.
//        QList<QDeclarativeGeoMapObject*> objects = mapObjects_;
//        for (int i = 0; i < objects.size(); ++i) {
//            mapData_->removeMapObject(objects.at(i)->mapObject());
//        }
//        delete mapData_;
//    }
    mouseAreas_.clear();
    if (serviceProvider_)
        delete serviceProvider_;
    if (initialCoordinate) {
        delete initialCoordinate;
    }
}

void QDeclarative3DGraphicsGeoMap::componentComplete()
{
    QLOC_TRACE0;
    componentCompleted_ = true;
    populateMap();
    map_->resize(width(), height());
    CameraData cameraData = map_->cameraData();
    map_->setCameraData(cameraData);
    map_->update();
    if (mappingManager_)
        pinchArea_->zoomLevelLimits(mappingManager_->minimumZoomLevel(), mappingManager_->maximumZoomLevel());
    QSGItem::componentComplete();
}

QDeclarativeGeoMapFlickable* QDeclarative3DGraphicsGeoMap::flick()
{
    return flickable_;
}

void QDeclarative3DGraphicsGeoMap::itemChange(ItemChange change, const ItemChangeData & data)
{
    QLOC_TRACE0;
    if (change == ItemSceneChange)
        canvas_ = data.canvas;
}

void QDeclarative3DGraphicsGeoMap::populateMap()
{
    if (!componentCompleted_)
        return;
    QObjectList kids = children();
    for (int i = 0; i < kids.size(); ++i) {
        // dispatch items appropriately
        QDeclarativeGeoMapObjectView* mapView = qobject_cast<QDeclarativeGeoMapObjectView*>(kids.at(i));
        if (mapView) {
            mapViews_.append(mapView);
            setupMapView(mapView);
            continue;
        }
        QDeclarativeGeoMapItem* mapItem = qobject_cast<QDeclarativeGeoMapItem*>(kids.at(i));
        if (mapItem) {
            addMapItem(mapItem);
        }
        QDeclarativeGeoMapMouseArea *mapMouseArea = qobject_cast<QDeclarativeGeoMapMouseArea*>(kids.at(i));
        if (mapMouseArea) {
            mapMouseArea->setMap(this);
            mouseAreas_.append(mapMouseArea);
        }
    }
}

void QDeclarative3DGraphicsGeoMap::setupMapView(QDeclarativeGeoMapObjectView *view)
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

void QDeclarative3DGraphicsGeoMap::updateAspectRatio()
{
    map_->resize(width(), height());
    if (!map_->autoUpdate())
        map_->update();
}

void QDeclarative3DGraphicsGeoMap::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    setSize(QSizeF(newGeometry.width(), newGeometry.height()));
    updateAspectRatio();
    QSGItem::geometryChanged(newGeometry, oldGeometry);
}

void QDeclarative3DGraphicsGeoMap::keyPressEvent(QKeyEvent *e)
{
    QLOC_TRACE2(" key: ", e->key());
    CameraData cameraData = map_->cameraData();
    if (e->key() == Qt::Key_Left) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLongitude(coord.longitude() - 1);
            cameraData.setCenter(coord);
        } else {
            cameraData.setBearing(cameraData.bearing() - 5.0);
        }
    } else if (e->key() == Qt::Key_Right) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLongitude(coord.longitude() + 1);
            cameraData.setCenter(coord);
        } else {
            cameraData.setBearing(cameraData.bearing() + 5.0);
        }
    } else if (e->key() == Qt::Key_Up) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLatitude(coord.latitude() + 1);
            cameraData.setCenter(coord);
        } else {
            cameraData.setTilt(cameraData.tilt() - 5.0);
        }
    } else if (e->key() == Qt::Key_Down) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLatitude(coord.latitude() - 1);
            cameraData.setCenter(coord);
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

void QDeclarative3DGraphicsGeoMap::paint(QPainter* p)
{
    if (!isVisible())
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

void QDeclarative3DGraphicsGeoMap::setCameraData(const CameraData &camera)
{
    map_->setCameraData(camera);
    if (!map_->autoUpdate())
        map_->update();
}

CameraData QDeclarative3DGraphicsGeoMap::cameraData() const
{
    return map_->cameraData();
}

void QDeclarative3DGraphicsGeoMap::restoreDefaults(QGLPainter *painter)
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

void QDeclarative3DGraphicsGeoMap::earlyDraw(QGLPainter *painter)
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

void QDeclarative3DGraphicsGeoMap::paintGL(QGLPainter *painter)
{
    if (map_) {
        painter->projectionMatrix().scale(1,-1, 1); // qt3d and qsg interpret y differently
        map_->paintGL(painter);
    }
}

/*!
    \qmlproperty Plugin Map::plugin

    This property holds the plugin which provides the mapping functionality.

    This is write-once property.  Once the map has a plugin associated with
    it any attempted modifications of the plugin will be ignored.
*/

void QDeclarative3DGraphicsGeoMap::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_) {
        qmlInfo(this) << tr("Plugin is a write-once property, and cannot be set again.");
        return;
    }
    plugin_ = plugin;
    emit pluginChanged(plugin_);
    serviceProvider_ = new QGeoServiceProvider(plugin_->name(),
                                               plugin_->parameterMap());
    if (serviceProvider_->error() != QGeoServiceProvider::NoError) {
        qWarning() << serviceProvider_->errorString();
        delete serviceProvider_;
        serviceProvider_ = 0;
        return;
    }
    mappingManager_ = serviceProvider_->mappingManager();
    if (!mappingManager_ || serviceProvider_->error() != QGeoServiceProvider::NoError) {
        qWarning() << serviceProvider_->errorString();
        delete serviceProvider_;
        serviceProvider_ = 0;
        delete mappingManager_;
        mappingManager_ = 0;
        return;
    }

    map_->setMappingManager(mappingManager_);

//    mapData_ = mappingManager_->createMapData();
//    mapData_->init();
    //mapData_->setParentItem(this);

    // setters
//    mapData_->setWindowSize(size_);
//    mapData_->setZoomLevel(zoomLevel_);

//    if (center_)
//        mapData_->setCenter(center_->coordinate());
//    else
//        mapData_->setCenter(*initialCoordinate);

//    mapData_->setMapType(QGraphicsGeoMap::MapType(mapType_));
//    mapData_->setConnectivityMode(QGraphicsGeoMap::ConnectivityMode(connectivityMode_));

    // Populate the map objects.
    populateMap();
    // setup signals
//    connect(mapData_,
//            SIGNAL(updateMapDisplay(QRectF)),
//            this,
//            SLOT(updateMapDisplay(QRectF)));

//    connect(mapData_,
//            SIGNAL(centerChanged(QGeoCoordinate)),
//            this,
//            SLOT(internalCenterChanged(QGeoCoordinate)));

//    connect(mapData_,
//            SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)),
//            this,
//            SLOT(internalMapTypeChanged(QGraphicsGeoMap::MapType)));

//    connect(mapData_,
//            SIGNAL(connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)),
//            this,
//            SLOT(internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)));

//    connect(mapData_,
//            SIGNAL(windowSizeChanged(QSizeF)),
//            this,
//            SIGNAL(sizeChanged(QSizeF)));

//    connect(mapData_,
//            SIGNAL(zoomLevelChanged(qreal)),
//            this,
//            SIGNAL(zoomLevelChanged(qreal)));
}

void QDeclarative3DGraphicsGeoMap::updateMapDisplay(const QRectF &target)
{
    Q_UNUSED(target);
    QSGItem::update();
}

QDeclarativeGeoServiceProvider* QDeclarative3DGraphicsGeoMap::plugin() const
{
    return plugin_;
}

/*!
    \qmlproperty qreal Map::minimumZoomLevel

    This property holds the minimum valid zoom level for the map.
*/

qreal QDeclarative3DGraphicsGeoMap::minimumZoomLevel() const
{
    if (mappingManager_)
        return mappingManager_->minimumZoomLevel();
    else
        return -1.0;
}

/*!
    \qmlproperty qreal Map::maximumZoomLevel

    This property holds the maximum valid zoom level for the map.
*/

qreal QDeclarative3DGraphicsGeoMap::maximumZoomLevel() const
{
    if (mappingManager_)
        return mappingManager_->maximumZoomLevel();
    else
        return -1.0;
}

// TODO make these more QML like
//QList<MapType> QDeclarative3DGraphicsGeoMap::supportedMapTypes() const;
//QList<ConnectivityMode> QDeclarative3DGraphicsGeoMap::supportedConnectivityModes() const;

/*!
    \qmlproperty QSizeF Map::size

    This property holds the size of the map viewport.
*/
void QDeclarative3DGraphicsGeoMap::setSize(const QSizeF &size)
{
//    if (mapData_) {
//        setWidth(size.width());
//        setHeight(size.height());
//        mapData_->setWindowSize(size);
//    } else {
        if (size_ == size)
            return;

        size_ = size;

        emit sizeChanged(size_);
//    }

}

QSizeF QDeclarative3DGraphicsGeoMap::size() const
{
//    if (mapData_)
//        return mapData_->windowSize();
//    else
        return size_;
}

/*!
    \qmlproperty qreal Map::zoomLevel

    This property holds the zoom level for the map.

    Larger values for the zoom level provide more detail.

    The default value is 8.0.
*/
void QDeclarative3DGraphicsGeoMap::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel_ == zoomLevel)
        return;
    if (mappingManager_ &&
            (zoomLevel < mappingManager_->minimumZoomLevel() ||
             zoomLevel > mappingManager_->maximumZoomLevel()))
        return;
    zoomLevel_ = zoomLevel;
    if (map_) {
        CameraData cameraData = map_->cameraData();
        cameraData.setZoomFactor(zoomLevel);
        map_->setCameraData(cameraData);
        if (!map_->autoUpdate())
            map_->update();
    }
    emit zoomLevelChanged(zoomLevel_);
}

void QDeclarative3DGraphicsGeoMap::cameraZoomLevelChanged(double zoomLevel)
{
    emit zoomLevelChanged(static_cast<qreal>(zoomLevel));
}

qreal QDeclarative3DGraphicsGeoMap::zoomLevel() const
{
    if (map_) {
        return map_->cameraData().zoomFactor();
    } else {
        return zoomLevel_;
    }
}

/*!
    \qmlproperty Coordinate Map::center

    This property holds the coordinate which occupies the center of the
    mapping viewport.

    The default value is an arbitrary valid coordinate.
*/
void QDeclarative3DGraphicsGeoMap::setCenter(QDeclarativeCoordinate *center)
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
        if (center_->coordinate().isValid()) {
            CameraData cameraData = map_->cameraData();
            cameraData.setCenter(center_->coordinate());
            map_->setCameraData(cameraData);
            update();
        }
    }
    emit centerChanged(center_);
}

QDeclarativeCoordinate* QDeclarative3DGraphicsGeoMap::center()
{
    if (!center_)
        return new QDeclarativeCoordinate(map_->cameraData().center());
    return center_;
}

void QDeclarative3DGraphicsGeoMap::centerLatitudeChanged(double latitude)
{
    if (qIsNaN(latitude))
        return;
    CameraData cameraData = map_->cameraData();
    QGeoCoordinate coord = cameraData.center();
    coord.setLatitude(latitude);
    cameraData.setCenter(coord);
    map_->setCameraData(cameraData);
    update();
    emit centerChanged(center_);
}

void QDeclarative3DGraphicsGeoMap::centerLongitudeChanged(double longitude)
{
    if (qIsNaN(longitude))
        return;
    CameraData cameraData = map_->cameraData();
    QGeoCoordinate coord = cameraData.center();
    coord.setLongitude(longitude);
    cameraData.setCenter(coord);
    map_->setCameraData(cameraData);
    update();
    emit centerChanged(center_);
}

void QDeclarative3DGraphicsGeoMap::centerAltitudeChanged(double altitude)
{
    if (qIsNaN(altitude))
        return;
    CameraData cameraData = map_->cameraData();
    QGeoCoordinate coord = cameraData.center();
    coord.setAltitude(altitude);
    cameraData.setCenter(coord);
    map_->setCameraData(cameraData);
    update();
    emit centerChanged(center_);
}

/*!
    \qmlproperty enumeration Map::mapType

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
//void QDeclarative3DGraphicsGeoMap::setMapType(QDeclarative3DGraphicsGeoMap::MapType mapType)
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

//QDeclarative3DGraphicsGeoMap::MapType QDeclarative3DGraphicsGeoMap::mapType() const
//{
//    if (mapData_) {
//        return QDeclarative3DGraphicsGeoMap::MapType(mapData_->mapType());
//    } else {
//        return mapType_;
//    }
//}

/*!
    \qmlproperty enumeration Map::connectivityMode

    This property holds the connectivity mode used to fetch the map data.

    The mode can be one of:
    \list
    \o Map.OfflineMode
    \o Map.OnlineMode
    \o Map.HybridMode
    \endlist

    The default value is determined by the plugin.
*/
//void QDeclarative3DGraphicsGeoMap::setConnectivityMode(QDeclarative3DGraphicsGeoMap::ConnectivityMode connectivityMode)
//{
//    if (mapData_) {
//        mapData_->setConnectivityMode(QGraphicsGeoMap::ConnectivityMode(connectivityMode));
//    } else {
//        if (connectivityMode_ == connectivityMode)
//            return;

//        connectivityMode_ = connectivityMode;

//        emit connectivityModeChanged(connectivityMode_);
//    }
//}

//QDeclarative3DGraphicsGeoMap::ConnectivityMode QDeclarative3DGraphicsGeoMap::connectivityMode() const
//{
//    if (mapData_)
//        return QDeclarative3DGraphicsGeoMap::ConnectivityMode(mapData_->connectivityMode());
//    else
//        return connectivityMode_;
//}

/*!
    \qmlproperty list<QGeoMapObject> Map::objects
    \default

    This property holds the list of objects associated with this map.

    The various objects that can be added include:
    \list
    \o MapRectangle
    \o MapCircle
    \o MapText
    \o MapImage
    \o MapPolygon
    \o MapPolyline
    \o MapGroup
    \endlist
*/

/*!
    \qmlmethod Map::toCoordinate(QPointF screenPosition)

    Returns the coordinate which corresponds to the screen position
    \a screenPosition.

    Returns an invalid coordinate if \a screenPosition is not within
    the current viewport.

    An example to constraint landmarks of a model to just those
    currently on Map:
    \snippet examples/declarative-location/landmarkmap/landmarkmap.qml Map toCoordinate

*/

QDeclarativeCoordinate* QDeclarative3DGraphicsGeoMap::toCoordinate(QPointF screenPosition) const
{
    QGeoCoordinate coordinate;
    if (map_)
        coordinate = map_->screenPositionToCoordinate(screenPosition);
    // by default objects returned from method call get javascript ownership,
    // so we don't need to worry about this as long as we don't set the parent
    return new QDeclarativeCoordinate(coordinate);
}

/*!
    \qmlmethod Map::toScreenPosition(Coordinate coordinate)

    Returns the screen position which corresponds to the coordinate
    \a coordinate.

    Returns an invalid QPointF if \a coordinate is not within the
    current viewport.
*/

QPointF QDeclarative3DGraphicsGeoMap::toScreenPosition(QDeclarativeCoordinate* coordinate) const
{
    QPointF point;
    if (map_)
        point = map_->coordinateToScreenPosition(coordinate->coordinate());
    return point;
}

void QDeclarative3DGraphicsGeoMap::pan(int dx, int dy)
{
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    qWarning() << __FUNCTION__ << " of Map not implemented."; // TODO
}

void QDeclarative3DGraphicsGeoMap::touchEvent(QTouchEvent *event)
{
    QLOC_TRACE0;
    event->accept();
    pinchArea_->touchEvent(event);
}

void QDeclarative3DGraphicsGeoMap::wheelEvent(QWheelEvent *event)
{
    QLOC_TRACE0;
    event->accept();
    emit wheel(event->delta());
}

// event delivery helpers
bool QDeclarative3DGraphicsGeoMap::deliverMouseEvent(QMouseEvent* event)
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

bool QDeclarative3DGraphicsGeoMap::deliverInitialMousePressEvent(QDeclarativeGeoMapMouseArea* ma, QMouseEvent* event)
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

void QDeclarative3DGraphicsGeoMap::mousePressEvent(QMouseEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    bool consumed = deliverMouseEvent(event);
    consumed |= flickable_->mousePressEvent(event);
    if (consumed)
        event->accept();
    else
        event->ignore();
}

// returns list of mouse areas under 'pos'. returned list is in the priority order in which the
// mouse events should be provided (currently does not consider 'z')
QList<QDeclarativeGeoMapMouseArea*> QDeclarative3DGraphicsGeoMap::mouseAreasAt(QPoint pos)
{
    QList<QDeclarativeGeoMapMouseArea*> list;
    for (int i = mouseAreas_.count() - 1; i >= 0; --i) {
        if (mouseAreas_.at(i)->boundingRect().contains(mouseAreas_.at(i)->mapFromScene(pos))) {
            list.append(mouseAreas_.at(i));
        }
    }
    return list;
}

void QDeclarative3DGraphicsGeoMap::mouseReleaseEvent(QMouseEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
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

void QDeclarative3DGraphicsGeoMap::mouseDoubleClickEvent(QMouseEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
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

void QDeclarative3DGraphicsGeoMap::mouseMoveEvent(QMouseEvent *event)
{
    //QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    /* hover placeholder
    if (!mouseGrabberItem_) {
        if (lastMousePosition_.isNull())
            lastMousePosition_ = event->windowPos();
        QPointF last = lastMousePosition_;
        lastMousePosition_ = event->windowPos();
        bool accepted = event->isAccepted();
        bool delivered = deliverHoverEvent(mouseAreaAt(event->pos()), event->windowPos(), last, event->modifiers(), accepted);
        // exit any hover areas if we're out of bounds
        if (!delivered)
            accepted = clearHover();
        event->setAccepted(accepted);
        return;
    }
    */
    bool consumed = false;
    if (mouseGrabberItem_)
        consumed = deliverMouseEvent(event);
    consumed |= flickable_->mouseMoveEvent(event);
    if (consumed)
        event->accept();
    else
        event->ignore();
}

// hover functions are a placeholder. It works to an extent but
// will need more work and testing if will be supported officially
/*

bool QDeclarative3DGraphicsGeoMap::deliverHoverEvent(QDeclarativeGeoMapMouseArea* ma, const QPointF &scenePos, const QPointF &lastScenePos,
                                         Qt::KeyboardModifiers modifiers, bool &accepted)
{
    if (hoverItem_ && ma == hoverItem_) {
        // hovering on the same item as before
        QLOC_TRACE2(" deliver hover for same item: ", ma->objectName());
        accepted = sendHoverEvent(QEvent::HoverMove, ma, scenePos, lastScenePos, modifiers, accepted);
        return true;
    } else {
        // leave previous hover item since it is different
        if (hoverItem_) {
            sendHoverEvent(QEvent::HoverLeave, hoverItem_, scenePos, lastScenePos, modifiers, accepted);
            QLOC_TRACE2("exiting previous hover area: ", hoverItem_->objectName());
            hoverItem_ = 0;
        }
        // enter new hover item if any
        if (ma && ma->hoverEnabled()) {
            QLOC_TRACE2("entering new hover area: ",  ma->objectName());
            sendHoverEvent(QEvent::HoverEnter, ma, scenePos, lastScenePos, modifiers, accepted);
            hoverItem_ = ma;
        } else {
            if (ma) {
                QLOC_TRACE2("no hover area, hovering disabled: ",  ma->objectName());
            } else {
                QLOC_TRACE1("no hover area");
            }
        }
        return true;
    }
    return false;
}


void QDeclarative3DGraphicsGeoMap::hoverEvent(QHoverEvent* event)
{
    QDeclarativeGeoMapMouseArea* ma = mouseAreaAt(event->pos());
    if (ma) {
        QLOC_TRACE2("for mouse area: ", ma->objectName());
    } else {
        QLOC_TRACE1("no mouse area");
    }
    if (lastMousePosition_.isNull())
        lastMousePosition_ = event->pos(); // hmmm todo was windowPos
    QPointF last = lastMousePosition_;
    lastMousePosition_ = event->pos(); // hmmm todo was windowPos
    bool accepted = event->isAccepted();
    (bool)deliverHoverEvent(ma, event->pos(), last, event->modifiers(), accepted);
}


bool QDeclarative3DGraphicsGeoMap::clearHover()
{
    QLOC_TRACE0;
    if (!hoverItem_)
        return false;
    QPointF pos = QCursor::pos();
    bool accepted = sendHoverEvent(QEvent::HoverLeave, hoverItem_, pos, pos, QGuiApplication::keyboardModifiers(), true);
    hoverItem_ = 0;
    return accepted;
}

bool QDeclarative3DGraphicsGeoMap::sendHoverEvent(QEvent::Type type, QSGItem *item,
                                      const QPointF &scenePos, const QPointF &lastScenePos,
                                      Qt::KeyboardModifiers modifiers, bool accepted)
{
    bool transformOk;
    const QTransform &transform = itemTransform(item, &transformOk);
    //create copy of event
    QHoverEvent hoverEvent(type, transform.map(scenePos), transform.map(lastScenePos), modifiers);
    hoverEvent.setAccepted(accepted);
    QDeclarativeGeoMapMouseArea* ma = static_cast<QDeclarativeGeoMapMouseArea*>(item);
    ma->hoverEvent(&hoverEvent);
    return hoverEvent.isAccepted();
}

void QDeclarative3DGraphicsGeoMap::hoverEnterEvent(QHoverEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    QLOC_TRACE2(" type: ", event->type());
    hoverEvent(event);
}

void QDeclarative3DGraphicsGeoMap::hoverMoveEvent(QHoverEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    QLOC_TRACE2(" type: ", event->type());
    hoverEvent(event);
}

void QDeclarative3DGraphicsGeoMap::hoverLeaveEvent(QHoverEvent *event)
{
    QLOC_TRACE2(" ~~~~~~~ event, coordinates: ", event->pos());
    QLOC_TRACE2(" type: ", event->type());
    hoverEvent(event);
}
*/

void QDeclarative3DGraphicsGeoMap::internalCenterChanged(const QGeoCoordinate &coordinate)
{
    emit centerChanged(new QDeclarativeCoordinate(coordinate, this));
}

//void QDeclarative3DGraphicsGeoMap::internalMapTypeChanged(QGraphicsGeoMap::MapType mapType)
//{
//    emit mapTypeChanged(QDeclarative3DGraphicsGeoMap::MapType(mapType));
//}

//void QDeclarative3DGraphicsGeoMap::internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)
//{
//    emit connectivityModeChanged(QDeclarative3DGraphicsGeoMap::ConnectivityMode(connectivityMode));
//}

/*!
    \qmlmethod Map::addMapObject(MapObject)

    Adds the given MapOject to the Map. If the object already
    is on the Map, it will not be added again.

    As an example, consider you have a MapCircle presenting your current position:

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic map position marker definition
    You can add it to Map (alterntively it can be defined as a child element of the Map):

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic add MapObject
    Note: MapObjectViews can not be added with this method.
*/

/*
void QDeclarative3DGraphicsGeoMap::addMapObject(QDeclarativeGeoMapObject *object)
{
    if (!mapData_)
        qmlInfo(this) << tr("Map plugin is not set, map object cannot be added.");
    if (!mapData_ || !object || objectMap_.contains(object->mapObject()))
        return;
    mapObjects_.append(object);
    objectMap_.insert(object->mapObject(), object);
    mapData_->addMapObject(object->mapObject());
    // TODO
    // object->setMap(this);
}
*/

// all map item additions and removals, whether internal or app originated,
// must go through these functions
void QDeclarative3DGraphicsGeoMap::addMapItem(QDeclarativeGeoMapItem *item)
{
    QLOC_TRACE0;
#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
    if (!item || mapItems_.contains(item) || mapItemsPending_.contains(item))
        return;
    updateMutex_.lock();
    // set pending, we'll add them once we have GL context
    mapItemsPending_.append(item);
    connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(mapItemDestroyed(QObject*)));
    updateMutex_.unlock();
#else
    Q_UNUSED(item);
#endif
}

void QDeclarative3DGraphicsGeoMap::removeMapItem(QDeclarativeGeoMapItem *item)
{
    QLOC_TRACE0;
#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
    if (!item)
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
#else
    Q_UNUSED(item);
#endif
}

// TODO clears all items including ones from models/mapobjectview which is not intended
void QDeclarative3DGraphicsGeoMap::clearMapItems()
{
    if (mapItems_.isEmpty())
        return;
    updateMutex_.lock();
    mapItems_.clear();
    mapItemsPending_.clear();
    map_->clearMapItems();
    updateMutex_.unlock();
}

// when a map item is getting destroyed, we need make sure resources are released gracefully
// with QML we must prepare for arbitrary deletion times
void QDeclarative3DGraphicsGeoMap::mapItemDestroyed(QObject* item)
{
    QDeclarativeGeoMapItem* mapItem = qobject_cast<QDeclarativeGeoMapItem*>(item);
    if (mapItem)
        removeMapItem(mapItem);
}

void QDeclarative3DGraphicsGeoMap::setActiveMouseArea(QDeclarativeGeoMapMouseArea *area)
{
    Q_UNUSED(area); // TODO
    // to be done when the item picking is clear
}


/*!
    \qmlmethod Map::removeMapObject(MapObject)

    Removes the given MapObject from the Map. If the MapObject does not
    exist, function does nothing.

    As an example, consider you have a MapCircle presenting your current position:
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic map position marker definition

    You can remove it from the Map element:
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic remove MapObject


*/

/*
void QDeclarative3DGraphicsGeoMap::removeMapObject(QDeclarativeGeoMapObject *object)
{
    if (!mapData_)
        qmlInfo(this) << tr("Map plugin is not set, map object cannot be removed.");
    if (!mapData_ || !object || !objectMap_.contains(object->mapObject()))
        return;
    objectMap_.remove(object->mapObject());
    mapObjects_.removeOne(object);
    mapData_->removeMapObject(object->mapObject());
}
*/

// This function is strictly for testing purposes
int QDeclarative3DGraphicsGeoMap::testGetDeclarativeMapObjectCount()
{
    return mapItems_.count();
}

#include "moc_qdeclarative3dgraphicsgeomap_p.cpp"

QT_END_NAMESPACE
