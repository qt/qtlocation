/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
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
//#include <qgeomapdata.h>
//#include <qgeomapobject.h>

#include <QGraphicsSceneMouseEvent>
#include <QDeclarativeContext>
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QModelIndex>
#include <QApplication>
#include <QSGCanvas>
#include <QSGEngine>
#include <QDesktopWidget>

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Map3D

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

    The Map element is part of the \bold{Qt.location 5.0} module.
*/

QDeclarative3DGraphicsGeoMap::QDeclarative3DGraphicsGeoMap(QSGItem *parent)
    : QSGItem(parent),
      plugin_(0),
      serviceProvider_(0),
      mappingManager_(0),
//      mapData_(0),
      center_(0),
      initialCoordinate(0),
//      mapType_(NoMap),
//      connectivityMode_(NoConnectivity),
      componentCompleted_(false),
#ifdef QSGMOUSEAREA_AVAILABLE
      mouseArea_(0),
#endif
      flickable_(0),
      pinchArea_(0),
      canvas_(0),
      touchTimer_(-1),
      tileCache_(0)

    //  ses_(0),
    //  texture_(0)
{
    initialCoordinate = new QGeoCoordinate(-27.0, 153.0);
    zoomLevel_ = 8;
    size_ = QSizeF(100.0, 100.0);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    setFlags(QSGItem::ItemHasContents);

    tileCache_ = new TileCache();
    map_ = new Map(tileCache_, this);
    connect(map_,
            SIGNAL(updateRequired()),
            this,
            SLOT(update()));
    //connect(map_->mapCamera(), SIGNAL(zoomChanged(double)), this, SLOT(cameraZoomLevelChanged(double)));
    // Create internal flickable and pinch area.
    flickable_ = new QDeclarativeGeoMapFlickable(map_, this);
    pinchArea_ = new QDeclarativeGeoMapPinchArea(this, this);
    qDebug() << __FUNCTION__ << "Created.============================================================";
}

QSGNode* QDeclarative3DGraphicsGeoMap::updatePaintNode(QSGNode* node, UpdatePaintNodeData* data)
{
    Q_UNUSED(node);
    Q_UNUSED(data);
    update();
    return 0;
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
    if (serviceProvider_)
        delete serviceProvider_;
    if (initialCoordinate) {
        delete initialCoordinate;
    }
#ifdef QSGMOUSEAREA_AVAILABLE
    delete mouseArea_;
#endif
}

void QDeclarative3DGraphicsGeoMap::componentComplete()
{
    qDebug() << __FUNCTION__ << "Completed =====-===-==================================================";
    componentCompleted_ = true;
    populateMap();
    map_->resize(width(), height());
    CameraData cameraData = map_->cameraData();
    map_->setCameraData(cameraData);
    map_->update();
    QSGItem::componentComplete();
}

QDeclarativeGeoMapFlickable* QDeclarative3DGraphicsGeoMap::flick()
{
    return flickable_;
}

void QDeclarative3DGraphicsGeoMap::itemChange(ItemChange change, const ItemChangeData & data)
{
    if (change == ItemSceneChange) {
        if (canvas_ && canvas_->sceneGraphEngine()) {
            canvas_->disconnect(this);
            canvas_->sceneGraphEngine()->disconnect(this);
        }
        canvas_ = data.canvas;
        if (canvas_->sceneGraphEngine()) {
            qDebug() << __FUNCTION__ << "Engine exists. Connecting to beforeRendering() " << canvas_->sceneGraphEngine();
            QSGEngine* engine =  canvas_->sceneGraphEngine();
            connect((QObject*)engine, SIGNAL(beforeRendering()), this, SLOT(beforeRendering()), Qt::DirectConnection);
            engine->setClearBeforeRendering(false);
        } else if (canvas_)
            connect(canvas_, SIGNAL(sceneGraphInitialized()), this, SLOT(sceneGraphInitialized()));
    }
}

void QDeclarative3DGraphicsGeoMap::sceneGraphInitialized()
{
    QSGEngine* engine =  canvas_->sceneGraphEngine();
    if (!engine) {
        qmlInfo(this) << tr("Unable to get QSGEngine. Will not be able to render the map.");
        return;
    }
    connect((QObject*)engine, SIGNAL(beforeRendering()), this, SLOT(beforeRendering()), Qt::DirectConnection);
    engine->setClearBeforeRendering(false);
}

void QDeclarative3DGraphicsGeoMap::populateMap()
{
//    if (!mapData_ || !componentCompleted_)
    if (!componentCompleted_)
        return;
    QObjectList kids = children();
    for (int i = 0; i < kids.size(); ++i) {
        //qDebug() << "Looping through..: " << kids.at(i)->metaObject()->className() << kids.at(i)->objectName();
        // dispatch items appropriately
        QDeclarativeGeoMapObjectView* mapView = qobject_cast<QDeclarativeGeoMapObjectView*>(kids.at(i));
        if (mapView) {
            mapViews_.append(mapView);
            setupMapView(mapView);
            continue;
        }
        QDeclarativeGeoMapObject *mapObject = qobject_cast<QDeclarativeGeoMapObject*>(kids.at(i));
        if (mapObject) {
            mapObjects_.append(mapObject);
//            objectMap_.insert(mapObject->mapObject(), mapObject);
//            mapData_->addMapObject(mapObject->mapObject());
            // to solve when we have map object support
            //mapObject->setMap(this);
            continue;
        }

        QDeclarativeGeoMapItem* mapItem = qobject_cast<QDeclarativeGeoMapItem*>(kids.at(i));
        if (mapItem) {
            addMapItem(mapItem);
        }
#ifdef QSGMOUSEAREA_AVAILABLE
        QSGMouseArea *mouseArea = qobject_cast<QSGMouseArea*>(kids.at(i));
        if (mouseArea && !mouseArea_) {
            //qDebug() << "Got mouse area.";
            mouseArea_ = mouseArea;
            // This is the trick to get mouse events reach Map element;
            // if visible is true then the MouseArea will consume events
            // before Map has chance to use it.
            mouseArea_->setVisible(false);
            connect(mouseArea_, SIGNAL(visibleChanged()), this, SLOT(mouseChanged()));
            connect(mouseArea_, SIGNAL(enabledChanged()), this, SLOT(mouseChanged()));
            connect(mouseArea_, SIGNAL(widthChanged()), this, SLOT(mouseChanged()));
            connect(mouseArea_, SIGNAL(heightChanged()), this, SLOT(mouseChanged()));
        } else if (mouseArea && mouseArea_) {
            qmlInfo(this) <<  tr("Warning: only one MouseArea / Map supported. Extra MouseArea ignored.");
        }
#endif
        QDeclarativeGeoMapMouseArea *mapMouseArea = qobject_cast<QDeclarativeGeoMapMouseArea*>(kids.at(i));
        if (mapMouseArea) {
            qmlInfo(this) << tr("Warning: MapMouseArea is no longer supported. Use normal MouseArea instead.");
        }
    }
}

void QDeclarative3DGraphicsGeoMap::mapItemTextureChanged()
{
#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
    for (int i = mapItemsPending_.count() - 1; i >= 0; --i) {
        if (mapItemsPending_.at(i)->hasValidTexture()) {
            disconnect(mapItemsPending_.at(i), SIGNAL(textureChanged()), this, SLOT(mapItemTextureChanged()));
            mapItemsPending_.at(i)->setMap(this);
            mapItems_.append(mapItemsPending_.at(i));
            map_->addMapItem(mapItemsPending_.at(i)->mapItem());
            mapItemsPending_.removeAt(i);
        }
    }
#endif
}

void QDeclarative3DGraphicsGeoMap::mouseChanged()
{
#ifdef QSGMOUSEAREA_AVAILABLE
    if (mouseArea_
            && mouseArea_->isVisible()
            && mouseArea_->isEnabled()
            && mouseArea_->width() > 0
            && mouseArea_->height() > 0) {
        qmlInfo(this) << tr("Warning: MouseArea in Map visible, enabled and has geometry. Map unable to intercept & dispatch mouse to map objects.");
    }
#endif
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

void QDeclarative3DGraphicsGeoMap::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

void QDeclarative3DGraphicsGeoMap::showEvent(QShowEvent *)
{
    updateAspectRatio();
}

void QDeclarative3DGraphicsGeoMap::updateAspectRatio()
{
    map_->resize(width(), height());
    if (!map_->autoUpdate())
        map_->update();
}

void QDeclarative3DGraphicsGeoMap::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    updateAspectRatio();
}

/*
void QDeclarative3DGraphicsGeoMap::enterEvent(QEvent *)
{
    setFocus(true);
    //grabKeyboard();
}

void QDeclarative3DGraphicsGeoMap::leaveEvent(QEvent *e)
{
    setFocus(false);
    //releaseKeyboard();
}
*/

void QDeclarative3DGraphicsGeoMap::keyPressEvent(QKeyEvent *e)
{
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

// Note: this slot will be executed in the QSG rendering thread
// (Qt::DirectConnection) - not in the GUI/main thread of the app.
// Hence thread-safen the critical sections of code to avoid crashes.
void QDeclarative3DGraphicsGeoMap::beforeRendering()
{
    if (!isVisible())
        return;
    QGLPainter painter;
    if (!painter.begin()) {
        qmlInfo(this) << tr("GL graphics system is not active; cannot use 3D items");
        return;
    }

#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
    // Update any map objects that may have dirty textures
    for (int i = 0; i < mapItems_.count(); ++i) {
        mapItems_.at(i)->updateItem();
    }
#endif
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
    glClearColor(0.0,255,0.0,0.0);
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
        // QGLSceneNode *node = map_->sceneNode();
        map_->paintGL(painter);
//        QGLSceneNode *node = map_->sceneNodeForRendering();
//        if (node) {
//            node->draw(painter);
//            map_->sceneNodeRenderingDone();
//        } else {
//            qDebug() << "=-=-=-=-=-=-=-=- WILL MISS A MAP FRAME =-=-=-=-=-=-=-=-=-=-=-=-=";
//        }
    }
}

void QDeclarative3DGraphicsGeoMap::geometryChanged(const QRectF &newGeometry,
                                                   const QRectF & /*oldGeometry*/)
{
    setSize(newGeometry.size());
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
    if (center_) {
        center_->disconnect(this);
    }
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

//        if (mapData_) {
//            mapData_->setCenter(center_->coordinate());
//        }
    }
    emit declarativeCenterChanged(center_);
}

QDeclarativeCoordinate* QDeclarative3DGraphicsGeoMap::center()
{
//    if (mapData_ && center_)
//        center_->setCoordinate(mapData_->center());
    return center_;
}

void QDeclarative3DGraphicsGeoMap::centerLatitudeChanged(double /*latitude*/)
{
//    if (mapData_ && center_)
//        mapData_->setCenter(center_->coordinate());
}

void QDeclarative3DGraphicsGeoMap::centerLongitudeChanged(double /*longitude*/)
{
//    if (mapData_ && center_)
//        mapData_->setCenter(center_->coordinate());
}

void QDeclarative3DGraphicsGeoMap::centerAltitudeChanged(double /*altitude*/)
{
//    if (mapData_ && center_)
//        mapData_->setCenter(center_->coordinate());
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

//    if (mapData_)
//        coordinate = mapData_->screenPositionToCoordinate(screenPosition);

    return new QDeclarativeCoordinate(coordinate,
                                      const_cast<QDeclarative3DGraphicsGeoMap *>(this));
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

//    if (mapData_)
//        point = mapData_->coordinateToScreenPosition(coordinate->coordinate());

    return point;
}

void QDeclarative3DGraphicsGeoMap::pan(int dx, int dy)
{
    Q_UNUSED(dx); // TODO panning support
    Q_UNUSED(dy);
    //qDebug() << "pan: " << dx << dy;
    //if (mapData_) {
    //    mapData_->pan(dx, dy);
    //    update();
    if (map_) {
        // TODO zzz map_->
    } else {
        qmlInfo(this) << tr("Map plugin is not set, cannot pan.");
    }
}

void QDeclarative3DGraphicsGeoMap::touchEvent(QTouchEvent *event)
{
    //qDebug() << "touchEvent in Map3D, enter, sending touchEvent to pinch area.";
    if (pinchArea_)
        pinchArea_->touchEvent(event);
}

void QDeclarative3DGraphicsGeoMap::wheelEvent(QWheelEvent *event)
{
    emit wheel(event->delta());
    //QSGItem::wheelEvent(event);
}

void QDeclarative3DGraphicsGeoMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "mousePressEvent in Map3D, enter +------------------------------------------------------------------";
//    if (!mapData_) {
//        qmlInfo(this) << tr("Map plugin is not set, mouse event cannot be processed.");
//        return;
//    }
#ifdef QSGMOUSEAREA_AVAILABLE
    canvas()->sendEvent(mouseArea_, event);
#endif
    if (flickable_)
        flickable_->mousePressEvent(event);
    if (pinchArea_)
        pinchArea_->mousePressEvent(event);

    //QSGItem::mousePressEvent(event);
}

void QDeclarative3DGraphicsGeoMap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
#ifdef QSGMOUSEAREA_AVAILABLE
    canvas()->sendEvent(mouseArea_, event);
#endif
    if (flickable_)
        flickable_->mouseReleaseEvent(event);
    if (pinchArea_)
        pinchArea_->mouseReleaseEvent(event);

    //QSGItem::mouseReleaseEvent(event);
}

void QDeclarative3DGraphicsGeoMap::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
//    if (!mapData_) {
//        qmlInfo(this) << tr("Map plugin is not set, mouse event cannot be processed.");
//        return;
//    }
#ifdef QSGMOUSEAREA_AVAILABLE
    canvas()->sendEvent(mouseArea_, event);
#endif
    //QSGItem::mouseDoubleClickEvent(event);
}

void QDeclarative3DGraphicsGeoMap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
#ifdef QSGMOUSEAREA_AVAILABLE
    canvas()->sendEvent(mouseArea_, event);
#endif
    if (flickable_)
        flickable_->mouseMoveEvent(event);
    if (pinchArea_)
        pinchArea_->mouseMoveEvent(event);
    //QSGItem::mouseMoveEvent(event);
}

void QDeclarative3DGraphicsGeoMap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
//    if (!mapData_)
//        return;
#ifdef QSGMOUSEAREA_AVAILABLE
    if (mouseArea_ && mouseArea_->hoverEnabled()) {
        canvas()->sendEvent(mouseArea_, event);
    }
#endif
}

void QDeclarative3DGraphicsGeoMap::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
#ifdef QSGMOUSEAREA_AVAILABLE
    if (mouseArea_ && mouseArea_->hoverEnabled()) {
        canvas()->sendEvent(mouseArea_, event);
    }
#endif
}

void QDeclarative3DGraphicsGeoMap::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
//    if (!mapData_)
//        return;
#ifdef QSGMOUSEAREA_AVAILABLE
    if (mouseArea_ && mouseArea_->hoverEnabled()) {
        canvas()->sendEvent(mouseArea_, event);
    }
#endif
}

void QDeclarative3DGraphicsGeoMap::internalCenterChanged(const QGeoCoordinate &coordinate)
{
    emit declarativeCenterChanged(new QDeclarativeCoordinate(coordinate, this));
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

void QDeclarative3DGraphicsGeoMap::addMapItem(QDeclarativeGeoMapItem *item)
{
#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
    if (!item || mapItems_.contains(item))
        return;
    item->setMap(this);
    mapItems_.append(item);
    map_->addMapItem(item->mapItem());
#endif
}

void QDeclarative3DGraphicsGeoMap::removeMapItem(QDeclarativeGeoMapItem *item)
{
#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
    if (!item || (!mapItems_.contains(item) && !mapItemsPending_.contains(item)))
        return;
    item->setMap(0);
    // these can be optmized for perf, as we already check the 'contains' above
    mapItems_.removeOne(item);
    mapItemsPending_.removeOne(item);
    map_->removeMapItem(item->mapItem());
#endif
}

// TODO clears all items including ones from models/mapobjectview which is not intended
void QDeclarative3DGraphicsGeoMap::clearMapItems()
{
    if (mapItems_.isEmpty())
        return;
    mapItems_.clear();
    mapItemsPending_.clear();
    map_->clearMapItems();
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
    return objectMap_.values().count();
}

#include "moc_qdeclarative3dgraphicsgeomap_p.cpp"

QT_END_NAMESPACE
