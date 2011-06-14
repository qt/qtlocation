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
#include "qdeclarativelandmark_p.h"
#include "qglview.h"
#include "qglsubsurface.h"
#include "tilecache.h"
#include "tile.h"
#include "tilesphere.h"

#include <qglscenenode.h>
#include <qglbuilder.h>
#include <qgeometrydata.h>

#include <qgeoserviceprovider.h>
#include <qgeomappingmanager.h>
#include <qgeomapdata.h>
#include <qgeomapobject.h>

#include <QGraphicsSceneMouseEvent>
#include <QDeclarativeContext>
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QModelIndex>

#include <QDebug>

QTM_BEGIN_NAMESPACE

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
QDeclarative3DGraphicsGeoMap::QDeclarative3DGraphicsGeoMap(QSGPaintedItem *parent)
    : QSGPaintedItem(parent),
      plugin_(0),
      serviceProvider_(0),
      mappingManager_(0),
      mapData_(0),
      center_(0),
      initialCoordinate(0),
      mapType_(NoMap),
      connectivityMode_(NoConnectivity),
      componentCompleted_(false),
      activeMouseArea_(0),
      tileCache_(0),
      tileSphere_(0),
      sceneNode_(0)
{
    initialCoordinate = new QGeoCoordinate(-27.0, 153.0);
    zoomLevel_ = 8;
    size_ = QSizeF(100.0, 100.0);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);

    setRenderTarget(QSGPaintedItem::FramebufferObject);
    setFillColor(Qt::yellow);

    tileCache_ = new TileCache();
    tileSphere_ = new TileSphere(this, tileCache_, 4, 1.0);
    connect(tileSphere_,
            SIGNAL(sphereUpdated()),
            this,
            SLOT(update()));
    sceneNode_ = tileSphere_->sphereSceneNode();
    tileSphere_->setRadius(3.0);
    tileSphere_->setZoom(5);
    tileSphere_->limitView(26, 16, 30, 20);
    tileSphere_->update();
}

QDeclarative3DGraphicsGeoMap::~QDeclarative3DGraphicsGeoMap()
{
    if (mapData_) {
        qDeleteAll(mapViews_);
        // Remove map objects, we can't allow mapObject
        // to delete the objects because they are owned
        // by the declarative elements.
        QList<QDeclarativeGeoMapObject*> objects = mapObjects_;
        for (int i = 0; i < objects.size(); ++i) {
            mapData_->removeMapObject(objects.at(i)->mapObject());
        }
        delete mapData_;
    }
    if (serviceProvider_)
        delete serviceProvider_;
    if (initialCoordinate) {
        delete initialCoordinate;
    }
}

void QDeclarative3DGraphicsGeoMap::componentComplete()
{
    componentCompleted_ = true;
    populateMap();
}

void QDeclarative3DGraphicsGeoMap::populateMap()
{
    if (!mapData_ || !componentCompleted_)
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
        QDeclarativeGeoMapObject *mapObject = qobject_cast<QDeclarativeGeoMapObject*>(kids.at(i));
        if (mapObject) {
            mapObjects_.append(mapObject);
            objectMap_.insert(mapObject->mapObject(), mapObject);
            mapData_->addMapObject(mapObject->mapObject());
            // to solve when we have map object support
            //mapObject->setMap(this);
            continue;
        }
        QDeclarativeGeoMapMouseArea *mouseArea
        = qobject_cast<QDeclarativeGeoMapMouseArea*>(kids.at(i));
        if (mouseArea) {
            // TODO mouse areas won't work
            //mouseArea->setMap(this);
            //mouseAreas_.append(mouseArea);
        }
    }
}

void QDeclarative3DGraphicsGeoMap::setupMapView(QDeclarativeGeoMapObjectView *view)
{
    Q_UNUSED(view) // TODO
    //view->setMapData(this);
    //view->repopulate();
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

void QDeclarative3DGraphicsGeoMap::paint(QPainter *painter)
{
    qDebug() << __FUNCTION__ << "----------- Map3d, mapData_:" << mapData_;
    // to paint the old mercator projection:
    //if (mapData_)
    //    mapData_->paint(painter, 0);

    QGLPainter glPainter;
    if (!glPainter.begin(painter)) {
        qWarning("GL graphics system is not active; cannot use 3D items");
        return;
    }
    // No stereo rendering, set the eye as neutral
    glPainter.setEye(QGL::NoEye);

    // Currently applied transforms for this Map3D element
    QTransform transform = painter->combinedTransform();
    // QSGItem::boundingRect() returns (top(0), left(0), width, height).
    // Then we get the rectangle that is gotten by applying the QTransform on the rect
    // --> this is the viewport for Map3D
    QRect viewport = transform.mapRect(boundingRect()).toRect();
    qDebug() << "Viewport paramteres: " << viewport.width() << viewport.height() << viewport.top() << viewport.left();
    qreal adjust = 1.0f;
    ViewportSubsurface surface(glPainter.currentSurface(), viewport, adjust);
    qDebug() << "surface before: " << &surface;
    glPainter.pushSurface(&surface);
    qDebug() << "surface after: " << &surface;

    earlyDraw(&glPainter);
    // From QGLView
    QGLCamera defCamera;
    glPainter.setCamera(&defCamera);
    paintGL(&glPainter);
    // Draw the children items



    glPainter.popSurface();
}

void QDeclarative3DGraphicsGeoMap::earlyDraw(QGLPainter *painter)
{
    // If we have a parent, then assume that the parent has painted
    // the background and overpaint over the top of it.  If we don't
    // have a parent, then clear to black.
    if (parentItem()) {
        glClear(GL_DEPTH_BUFFER_BIT);
    } else {
        painter->setClearColor(Qt::black);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Force the effect to be updated.  The GL paint engine
    // has left the GL state in an unknown condition.
    painter->disableEffect();

#ifdef GL_RESCALE_NORMAL
    glEnable(GL_RESCALE_NORMAL);
#endif

    // Set the default effect for the scene.
    painter->setStandardEffect(QGL::LitMaterial);
    painter->setFaceColor(QGL::AllFaces, Qt::white);
}


// Private paint
void QDeclarative3DGraphicsGeoMap::paintGL(QGLPainter *painter)
{
    qDebug() << __FUNCTION__ << " of Map3D, sceneNode_: " << sceneNode_;
    if (sceneNode_)
        sceneNode_->draw(painter);
    qDebug() << __FUNCTION__ << " after";
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
  qDebug("setPlugin called for Map");
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
    mapData_ = mappingManager_->createMapData();
    mapData_->init();
    //mapData_->setParentItem(this);

    // setters
    mapData_->setWindowSize(size_);
    mapData_->setZoomLevel(zoomLevel_);

    if (center_)
        mapData_->setCenter(center_->coordinate());
    else
        mapData_->setCenter(*initialCoordinate);

    mapData_->setMapType(QGraphicsGeoMap::MapType(mapType_));
    mapData_->setConnectivityMode(QGraphicsGeoMap::ConnectivityMode(connectivityMode_));

    // Populate the map objects.
    populateMap();
    // setup signals
    connect(mapData_,
            SIGNAL(updateMapDisplay(QRectF)),
            this,
            SLOT(updateMapDisplay(QRectF)));

    connect(mapData_,
            SIGNAL(centerChanged(QGeoCoordinate)),
            this,
            SLOT(internalCenterChanged(QGeoCoordinate)));

    connect(mapData_,
            SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)),
            this,
            SLOT(internalMapTypeChanged(QGraphicsGeoMap::MapType)));

    connect(mapData_,
            SIGNAL(connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)),
            this,
            SLOT(internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)));

    connect(mapData_,
            SIGNAL(windowSizeChanged(QSizeF)),
            this,
            SIGNAL(sizeChanged(QSizeF)));

    connect(mapData_,
            SIGNAL(zoomLevelChanged(qreal)),
            this,
            SIGNAL(zoomLevelChanged(qreal)));
}

void QDeclarative3DGraphicsGeoMap::updateMapDisplay(const QRectF &target)
{
    QSGPaintedItem::update(target.toRect());
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
    if (mapData_) {
        setWidth(size.width());
        setHeight(size.height());
        mapData_->setWindowSize(size);
    } else {
        if (size_ == size)
            return;

        size_ = size;

        emit sizeChanged(size_);
    }

}

QSizeF QDeclarative3DGraphicsGeoMap::size() const
{
    if (mapData_)
        return mapData_->windowSize();
    else
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
    if (mapData_) {
        mapData_->setZoomLevel(zoomLevel);
    } else {
        if (zoomLevel_ == zoomLevel)
            return;

        zoomLevel_ = zoomLevel;

        emit zoomLevelChanged(zoomLevel_);
    }
}

qreal QDeclarative3DGraphicsGeoMap::zoomLevel() const
{
    if (mapData_) {
        return mapData_->zoomLevel();
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

        if (mapData_) {
            mapData_->setCenter(center_->coordinate());
        }
    }
    emit declarativeCenterChanged(center_);
}

QDeclarativeCoordinate* QDeclarative3DGraphicsGeoMap::center()
{
    if (mapData_ && center_)
        center_->setCoordinate(mapData_->center());
    return center_;
}

void QDeclarative3DGraphicsGeoMap::centerLatitudeChanged(double /*latitude*/)
{
    if (mapData_ && center_)
        mapData_->setCenter(center_->coordinate());
}

void QDeclarative3DGraphicsGeoMap::centerLongitudeChanged(double /*longitude*/)
{
    if (mapData_ && center_)
        mapData_->setCenter(center_->coordinate());
}

void QDeclarative3DGraphicsGeoMap::centerAltitudeChanged(double /*altitude*/)
{
    if (mapData_ && center_)
        mapData_->setCenter(center_->coordinate());
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
void QDeclarative3DGraphicsGeoMap::setMapType(QDeclarative3DGraphicsGeoMap::MapType mapType)
{
    if (mapData_) {
        mapData_->setMapType(QGraphicsGeoMap::MapType(mapType));
    } else {
        if (mapType_ == mapType)
            return;

        mapType_ = mapType;

        emit mapTypeChanged(mapType_);
    }
}

QDeclarative3DGraphicsGeoMap::MapType QDeclarative3DGraphicsGeoMap::mapType() const
{
    if (mapData_) {
        return QDeclarative3DGraphicsGeoMap::MapType(mapData_->mapType());
    } else {
        return mapType_;
    }
}

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
void QDeclarative3DGraphicsGeoMap::setConnectivityMode(QDeclarative3DGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    if (mapData_) {
        mapData_->setConnectivityMode(QGraphicsGeoMap::ConnectivityMode(connectivityMode));
    } else {
        if (connectivityMode_ == connectivityMode)
            return;

        connectivityMode_ = connectivityMode;

        emit connectivityModeChanged(connectivityMode_);
    }
}

QDeclarative3DGraphicsGeoMap::ConnectivityMode QDeclarative3DGraphicsGeoMap::connectivityMode() const
{
    if (mapData_)
        return QDeclarative3DGraphicsGeoMap::ConnectivityMode(mapData_->connectivityMode());
    else
        return connectivityMode_;
}

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

    if (mapData_)
        coordinate = mapData_->screenPositionToCoordinate(screenPosition);

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

    if (mapData_)
        point = mapData_->coordinateToScreenPosition(coordinate->coordinate());

    return point;
}

void QDeclarative3DGraphicsGeoMap::pan(int dx, int dy)
{
    if (mapData_) {
        mapData_->pan(dx, dy);
        update();
    } else {
        qmlInfo(this) << tr("Map plugin is not set, cannot pan.");
    }
}

QDeclarativeGeoMapMouseEvent* QDeclarative3DGraphicsGeoMap::createMapMouseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !mapData_)
        return 0;

    QDeclarativeGeoMapMouseEvent *mouseEvent = new QDeclarativeGeoMapMouseEvent(this);

    mouseEvent->setButton(event->button());
    mouseEvent->setModifiers(event->modifiers());

    QGeoCoordinate coordinate = mapData_->screenPositionToCoordinate(event->pos());
    mouseEvent->setCoordinate(new QDeclarativeCoordinate(coordinate, this));
    mouseEvent->setX(event->pos().x());
    mouseEvent->setY(event->pos().y());

    return mouseEvent;
}

QDeclarativeGeoMapMouseEvent* QDeclarative3DGraphicsGeoMap::createMapMouseEvent(QGraphicsSceneHoverEvent *event)
{
    if (!event || !mapData_)
        return 0;

    QDeclarativeGeoMapMouseEvent *mouseEvent = new QDeclarativeGeoMapMouseEvent(this);

    mouseEvent->setModifiers(event->modifiers());

    QGeoCoordinate coordinate = mapData_->screenPositionToCoordinate(event->pos());
    mouseEvent->setCoordinate(new QDeclarativeCoordinate(coordinate, this));
    mouseEvent->setX(event->pos().x());
    mouseEvent->setY(event->pos().y());

    return mouseEvent;
}

void QDeclarative3DGraphicsGeoMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!mapData_) {
        qmlInfo(this) << tr("Map plugin is not set, mouse event cannot be processed.");
        return;
    }

    // First deliver the mouse event to possible map objects (they will accept it if
    // they have their own mouse area). If no accepting objects, check if the map
    // object itself has any mouse areas. This way the map objects have higher priority
    // in mouse event handling.
    QList<QGeoMapObject*> objects = mapData_->mapObjectsAtScreenPosition(event->pos());

    QDeclarativeGeoMapMouseEvent *mouseEvent = createMapMouseEvent(event);

    for (int i = objects.size() - 1; i >= 0; --i) {
        QDeclarativeGeoMapObject* mapObject = objectMap_.value(objects.at(i), 0);
        if (mapObject) {
            mapObject->pressEvent(mouseEvent);
            if (mouseEvent->accepted()) {
                event->setAccepted(true);
                delete mouseEvent;
                return;
            }
        }
    }

    for (int i = 0; i < mouseAreas_.size(); ++i) {
        mouseAreas_.at(i)->pressEvent(mouseEvent);
        if (mouseEvent->accepted()) {
            event->setAccepted(true);
            delete mouseEvent;
            return;
        }
    }

    delete mouseEvent;

    //QDeclarativeItem::mousePressEvent(event);
    QSGPaintedItem::mousePressEvent(event);
}

void QDeclarative3DGraphicsGeoMap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeGeoMapMouseEvent *mouseEvent = createMapMouseEvent(event);

    if (activeMouseArea_)
        activeMouseArea_->releaseEvent(mouseEvent);
    activeMouseArea_ = 0;

    bool accepted = (mouseEvent->accepted());

    delete mouseEvent;

    if (accepted)
        return;

    //QDeclarativeItem::mouseReleaseEvent(event);
    QSGPaintedItem::mouseReleaseEvent(event);
}

void QDeclarative3DGraphicsGeoMap::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (!mapData_) {
        qmlInfo(this) << tr("Map plugin is not set, mouse event cannot be processed.");
        return;
    }

    activeMouseArea_ = 0;

    QList<QGeoMapObject*> objects = mapData_->mapObjectsAtScreenPosition(event->pos());

    QDeclarativeGeoMapMouseEvent *mouseEvent = createMapMouseEvent(event);

    for (int i = objects.size() - 1; i >= 0; --i) {
        QDeclarativeGeoMapObject* mapObject = objectMap_.value(objects.at(i), 0);
        if (mapObject) {
            mapObject->doubleClickEvent(mouseEvent);
            if (mouseEvent->accepted()) {
                event->setAccepted(true);
                delete mouseEvent;
                return;
            }
        }
    }

    for (int i = 0; i < mouseAreas_.size(); ++i) {
        mouseAreas_.at(i)->doubleClickEvent(mouseEvent);
        if (mouseEvent->accepted()) {
            event->setAccepted(true);
            delete mouseEvent;
            return;
        }
    }

    bool accepted = (mouseEvent->accepted());

    delete mouseEvent;

    if (accepted)
        return;

    //QDeclarativeItem::mouseDoubleClickEvent(event);
    QSGPaintedItem::mouseDoubleClickEvent(event);
}

void QDeclarative3DGraphicsGeoMap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeGeoMapMouseEvent *mouseEvent = createMapMouseEvent(event);

    if (activeMouseArea_)
        activeMouseArea_->moveEvent(mouseEvent);

    bool accepted = (mouseEvent->accepted());

    delete mouseEvent;

    if (accepted)
        return;

    //QDeclarativeItem::mouseMoveEvent(event);
    QSGPaintedItem::mouseMoveEvent(event);
}

void QDeclarative3DGraphicsGeoMap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
//    qWarning() << "hover enter";
}

void QDeclarative3DGraphicsGeoMap::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!mapData_)
        return;

    QList<QGeoMapObject*> objectsThen = mapData_->mapObjectsAtScreenPosition(event->lastPos());
    QList<QGeoMapObject*> objectsNow = mapData_->mapObjectsAtScreenPosition(event->pos());

    QSet<QGeoMapObject*> enter = objectsNow.toSet();
    enter -= objectsThen.toSet();

    for (int i = 0; i < objectsNow.size(); ++i) {
        if (!enter.contains(objectsNow.at(i)))
            continue;

        QDeclarativeGeoMapObject* mapObject = objectMap_.value(objectsNow.at(i), 0);
        if (mapObject)
            mapObject->enterEvent();
    }

    QSet<QGeoMapObject*> exit = objectsThen.toSet();
    exit -= objectsNow.toSet();

    for (int i = 0; i < objectsThen.size(); ++i) {
        if (!exit.contains(objectsThen.at(i)))
            continue;

        QDeclarativeGeoMapObject* mapObject = objectMap_.value(objectsThen.at(i), 0);
        if (mapObject)
            mapObject->exitEvent();
    }

    QSet<QGeoMapObject*> move = objectsNow.toSet();
    move += objectsThen.toSet();

    QList<QGeoMapObject*> objects = mapData_->mapObjectsInViewport();

    QDeclarativeGeoMapMouseEvent *mouseEvent = createMapMouseEvent(event);

    for (int i = 0; i < objects.size(); ++i) {
        if (!move.contains(objects.at(i)))
            continue;

        QDeclarativeGeoMapObject* mapObject = objectMap_.value(objects.at(i), 0);
        if (mapObject)
            mapObject->moveEvent(mouseEvent);
    }

    /*
    QList<QGeoMapObject*> objects = mapData_->mapObjectsAtScreenPosition(event->pos());

    for (int i = objects.size() - 1; i >= 0; --i) {
        QDeclarativeGeoMapObject* mapObject = objectMap_.value(objects.at(i), 0);
        if (mapObject)
            mapObject->moveEvent(mouseEvent);
    }
    */

    for (int i = 0; i < mouseAreas_.size(); ++i)
        mouseAreas_.at(i)->moveEvent(mouseEvent);

    delete mouseEvent;
}

void QDeclarative3DGraphicsGeoMap::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
//    qWarning() << "hover leave";
}

void QDeclarative3DGraphicsGeoMap::internalCenterChanged(const QGeoCoordinate &coordinate)
{
    emit declarativeCenterChanged(new QDeclarativeCoordinate(coordinate, this));
}

void QDeclarative3DGraphicsGeoMap::internalMapTypeChanged(QGraphicsGeoMap::MapType mapType)
{
    emit mapTypeChanged(QDeclarative3DGraphicsGeoMap::MapType(mapType));
}

void QDeclarative3DGraphicsGeoMap::internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    emit connectivityModeChanged(QDeclarative3DGraphicsGeoMap::ConnectivityMode(connectivityMode));
}

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

/*!
    \qmlmethod Map::removeMapObject(MapObject)

    Removes the given MapObject from the Map. If the MapObject does not
    exist, function does nothing.

    As an example, consider you have a MapCircle presenting your current position:
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic map position marker definition

    You can remove it from the Map element:
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic remove MapObject


*/

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

void QDeclarative3DGraphicsGeoMap::setActiveMouseArea(QDeclarativeGeoMapMouseArea *area)
{
    activeMouseArea_ = area;
}

QDeclarativeGeoMapMouseArea* QDeclarative3DGraphicsGeoMap::activeMouseArea() const
{
    return activeMouseArea_;
}

// This function is strictly for testing purposes
int QDeclarative3DGraphicsGeoMap::testGetDeclarativeMapObjectCount()
{
    return objectMap_.values().count();
}

#include "moc_qdeclarative3dgraphicsgeomap_p.cpp"

QTM_END_NAMESPACE
