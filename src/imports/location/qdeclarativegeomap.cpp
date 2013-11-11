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

#include "qdeclarativegeomap_p.h"
#include "qdeclarativegeomapmousearea_p.h"
#include "error_messages.h"

#include "qdeclarativecirclemapitem_p.h"
#include "qdeclarativegeomapquickitem_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include <QtCore/QCoreApplication>
#include <QtCore/qnumeric.h>
#include <QThread>

#include "qgeotilecache_p.h"
#include "qgeocameradata_p.h"
#include "qgeocameracapabilities_p.h"
#include "qgeomapcontroller_p.h"
#include "mapnode_p.h"
#include <cmath>

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeomappingmanager_p.h>

#include <QtQml/QQmlContext>
#include <QtQml/qqmlinfo.h>
#include <QModelIndex>
#include <QtQuick/QQuickWindow>
#include <QtGui/QGuiApplication>
#include <QCoreApplication>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Map
    \instantiates QDeclarativeGeoMap
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0

    \brief The Map type displays a map.

    The Map type is used to display a map or image of the Earth, with
    the capability to also display interactive objects tied to the map's
    surface.

    There are a variety of different ways to visualize the Earth's surface
    in a 2-dimensional manner, but all of them involve some kind of projection:
    a mathematical relationship between the 3D coordinates (latitude, longitude
    and altitude) and 2D coordinates (X and Y in pixels) on the screen.

    Different sources of map data can use different projections, and from the
    point of view of the Map type, we treat these as one replaceable unit:
    the Map plugin. A Map plugin consists of a data source, as well as all other
    details needed to display its data on-screen.

    The current Map plugin in use is contained in the \l plugin property of
    the Map item. In order to display any image in a Map item, you will need
    to set this property. See the \l Plugin type for a description of how
    to retrieve an appropriate plugin for use.

    The geographic region displayed in the Map item is referred to as its
    viewport, and this is defined by the properties \l center, and
    \l zoomLevel. The \l center property contains a \l {coordinate}
    specifying the center of the viewport, while \l zoomLevel controls the scale of the
    map. See each of these properties for further details about their values.

    When the map is displayed, each possible geographic coordinate that is
    visible will map to some pixel X and Y coordinate on the screen. To perform
    conversions between these two, Map provides the \l toCoordinate and
    \l toScreenPosition functions, which are of general utility.

    \section2 Map Objects

    Map related objects can be declared within the body of a Map object in Qt Quick and will
    automatically appear on the Map. To add objects programmatically, first be
    sure they are created with the Map as their parent (for example in an argument to
    Component::createObject), and then call the \l addMapItem method on the Map.
    A corresponding \l removeMapItem method also exists to do the opposite and
    remove an object from the Map.

    Moving Map objects around, resizing them or changing their shape normally
    does not involve any special interaction with Map itself -- changing these
    details about a map object will automatically update the display.

    \section2 Interaction

    The Map type includes support for pinch and flick gestures to control
    zooming and panning. These are enabled by default, and available at any
    time by using the \l gesture object. The actual GestureArea is constructed
    specially at startup and cannot be replaced or destroyed. Its properties
    can be altered, however, to control its behavior.

    Mouse and touch interaction with Maps and map objects is slightly different
    to ordinary QML types. In a Map or Map object, you will need to make use
    of the \l MapMouseArea type instead of the normal Qt Quick MouseArea.
    MapMouseArea is, in almost all respects, a drop-in replacement for
    MouseArea, but the documentation for that type should be referred to for
    further details.

    \section2 Performance

    Maps are rendered using OpenGL (ES) and the Qt Scene Graph stack, and as
    a result perform quite well where GL accelerated hardware is available.

    For "online" Map plugins, network bandwidth and latency can be major
    contributors to the user's perception of performance. Extensive caching is
    performed to mitigate this, but such mitigation is not always perfect. For
    "offline" plugins, the time spent retrieving the stored geographic data
    and rendering the basic map features can often play a dominant role. Some
    offline plugins may use hardware acceleration themselves to (partially)
    avert this.

    In general, large and complex Map items such as polygons and polylines with
    large numbers of vertices can have an adverse effect on UI performance.
    Further, more detailed notes on this are in the documentation for each
    map item type.

    \section2 Example Usage

    The following snippet shows a simple Map and the necessary Plugin type
    to use it. The map is centered near Brisbane, Australia, zoomed out to the
    minimum zoom level, with gesture interaction enabled.

    \code
    Plugin {
        id: somePlugin
        // code here to choose the plugin as necessary
    }

    Map {
        id: map

        plugin: somePlugin

        center {
            latitude: -27
            longitude: 153
        }
        zoomLevel: map.minimumZoomLevel

        gesture.enabled: true
    }
    \endcode

    \image api-map.png
*/

QDeclarativeGeoMap::QDeclarativeGeoMap(QQuickItem *parent)
        : QQuickItem(parent),
        plugin_(0),
        serviceProvider_(0),
        mappingManager_(0),
        zoomLevel_(8.0),
        bearing_(0.0),
        tilt_(0.0),
        activeMapType_(0),
        componentCompleted_(false),
        mappingManagerInitialized_(false),
        window_(0),
        touchTimer_(-1),
        map_(0)
{
    QLOC_TRACE0;
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    setFlags(QQuickItem::ItemHasContents | QQuickItem::ItemClipsChildrenToShape);
    setFiltersChildMouseEvents(true);

    connect(this, SIGNAL(childrenChanged()), this, SLOT(onMapChildrenChanged()), Qt::QueuedConnection);

    // Create internal flickable and pinch area.
    gestureArea_ = new QDeclarativeGeoMapGestureArea(this, this);
}

QDeclarativeGeoMap::~QDeclarativeGeoMap()
{
    if (!mapViews_.isEmpty())
        qDeleteAll(mapViews_);
    // remove any map items associations
    for (int i = 0; i < mapItems_.count(); ++i) {
        if (mapItems_.at(i))
            mapItems_.at(i).data()->setMap(0,0);
    }
    mapItems_.clear();

    delete copyrightsWPtr_.data();
    copyrightsWPtr_.clear();
}

/*!
    \internal
*/
void QDeclarativeGeoMap::onMapChildrenChanged()
{
    if (!componentCompleted_)
        return;

    int maxChildZ = 0;
    QObjectList kids = children();
    bool foundCopyrights = false;

    for (int i = 0; i < kids.size(); ++i) {
        QDeclarativeGeoMapCopyrightNotice *copyrights = qobject_cast<QDeclarativeGeoMapCopyrightNotice *>(kids.at(i));
        if (copyrights) {
            foundCopyrights = true;
        } else {
            QDeclarativeGeoMapItemBase *mapItem = qobject_cast<QDeclarativeGeoMapItemBase *>(kids.at(i));
            if (mapItem) {
                if (mapItem->z() > maxChildZ)
                    maxChildZ = mapItem->z();
            }
        }
    }

    QDeclarativeGeoMapCopyrightNotice *copyrights = copyrightsWPtr_.data();
    // if copyrights object not found within the map's children
    if (!foundCopyrights) {
        // if copyrights object was deleted!
        if (!copyrights) {
            // create a new one and set its parent, re-assign it to the weak pointer, then connect the copyrights-change signal
            copyrightsWPtr_ = new QDeclarativeGeoMapCopyrightNotice(this);
            copyrights = copyrightsWPtr_.data();
            connect(map_,
                    SIGNAL(copyrightsChanged(const QImage &, const QPoint &)),
                    copyrights,
                    SLOT(copyrightsChanged(const QImage &, const QPoint &)));
        } else {
            // just re-set its parent.
            copyrights->setParent(this);
        }
    }

    // put the copyrights notice object at the highest z order
    copyrights->setCopyrightsZ(maxChildZ + 1);
}

/*!
    \internal
*/
void QDeclarativeGeoMap::pluginReady()
{
    serviceProvider_ = plugin_->sharedGeoServiceProvider();
    mappingManager_ = serviceProvider_->mappingManager();

    if (!mappingManager_ || serviceProvider_->error() != QGeoServiceProvider::NoError) {
        QString msg = QCoreApplication::translate(CONTEXT_NAME, PLUGIN_DOESNOT_SUPPORT_MAPPING).arg(serviceProvider_->errorString());
        qmlInfo(this) << msg;
        return;
    }

    if (!mappingManager_->isInitialized())
        connect(mappingManager_, SIGNAL(initialized()), this, SLOT(mappingManagerInitialized()));
    else
        mappingManagerInitialized();

    // make sure this is only called once
    disconnect(this, SLOT(pluginReady()));
}

/*!
    \internal
*/
void QDeclarativeGeoMap::componentComplete()
{
    QLOC_TRACE0;

    componentCompleted_ = true;
    populateMap();
    QQuickItem::componentComplete();
}

/*!
    \internal
*/
void QDeclarativeGeoMap::mousePressEvent(QMouseEvent *event)
{
    if (!mouseEvent(event))
        event->ignore();
}

/*!
    \internal
*/
void QDeclarativeGeoMap::mouseMoveEvent(QMouseEvent *event)
{
    if (!mouseEvent(event))
        event->ignore();
}

/*!
    \internal
*/
void QDeclarativeGeoMap::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mouseEvent(event))
        event->ignore();
}

/*!
    \internal
    returns whether flickable used the event
*/
bool QDeclarativeGeoMap::mouseEvent(QMouseEvent *event)
{
    if (!mappingManagerInitialized_)
        return false;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        return gestureArea_->mousePressEvent(event);
    case QEvent::MouseButtonRelease:
        return gestureArea_->mouseReleaseEvent(event);
    case QEvent::MouseMove:
        return gestureArea_->mouseMoveEvent(event);
    default:
        return false;
    }
}


/*!
    \qmlproperty MapGestureArea QtLocation::Map::gesture

    Contains the MapGestureArea created with the Map. This covers pan, flick and pinch gestures.
    Use \c{gesture.enabled: true} to enable basic gestures, or see \l{MapGestureArea} for
    further details.
*/

QDeclarativeGeoMapGestureArea *QDeclarativeGeoMap::gesture()
{
    return gestureArea_;
}

/*!
    \internal
*/
void QDeclarativeGeoMap::itemChange(ItemChange change, const ItemChangeData & data)
{
    QLOC_TRACE0;
    if (change == ItemSceneChange)
        window_ = data.window;
}

/*!
    \internal
*/
void QDeclarativeGeoMap::populateMap()
{
    QObjectList kids = children();
    for (int i = 0; i < kids.size(); ++i) {
        // dispatch items appropriately
        QDeclarativeGeoMapItemView *mapView = qobject_cast<QDeclarativeGeoMapItemView *>(kids.at(i));
        if (mapView) {
            mapViews_.append(mapView);
            setupMapView(mapView);
            continue;
        }
        QDeclarativeGeoMapItemBase *mapItem = qobject_cast<QDeclarativeGeoMapItemBase *>(kids.at(i));
        if (mapItem) {
            addMapItem(mapItem);
        }
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMap::setupMapView(QDeclarativeGeoMapItemView *view)
{
    Q_UNUSED(view)
    view->setMapData(this);
    view->repopulate();
}

/*!
    \internal
*/
QSGNode *QDeclarativeGeoMap::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data)
    if (width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }

    MapNode *node = static_cast<MapNode *>(oldNode);

    if (!node) {
        node = new MapNode(map_);
    }

    if (!mappingManagerInitialized_)
        return 0;

    node->setSize(QSize(width(), height()));
    node->update();

    return node;
}


/*!
    \qmlproperty Plugin QtLocation::Map::plugin

    This property holds the plugin which provides the mapping functionality.

    This is a write-once property. Once the map has a plugin associated with
    it, any attempted modifications of the plugin will be ignored.
*/

void QDeclarativeGeoMap::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_) {
        qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PLUGIN_SET_ONCE);
        return;
    }
    plugin_ = plugin;
    emit pluginChanged(plugin_);

    if (plugin_->isAttached()) {
        pluginReady();
    } else {
        connect(plugin_, SIGNAL(attached()),
                this, SLOT(pluginReady()));
    }
}

/*!
    \internal
    this function will only be ever called once
*/
void QDeclarativeGeoMap::mappingManagerInitialized()
{
    mappingManagerInitialized_ = true;

    map_ = mappingManager_->createMap(this);
    gestureArea_->setMap(map_);
    gestureArea_->zoomLevelLimits(mappingManager_->cameraCapabilities().minimumZoomLevel(),
                                  mappingManager_->cameraCapabilities().maximumZoomLevel());

    map_->setActiveMapType(QGeoMapType());

    copyrightsWPtr_ = new QDeclarativeGeoMapCopyrightNotice(this);
    connect(map_,
            SIGNAL(copyrightsChanged(const QImage &, const QPoint &)),
            copyrightsWPtr_.data(),
            SLOT(copyrightsChanged(const QImage &, const QPoint &)));

    QGeoCameraCapabilities capabilities = mappingManager_->cameraCapabilities();
    if (zoomLevel_ < capabilities.minimumZoomLevel())
        setZoomLevel(capabilities.minimumZoomLevel());
    else if (zoomLevel_ > capabilities.maximumZoomLevel())
        setZoomLevel(capabilities.maximumZoomLevel());
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

    AnimatableCoordinate acenter = map_->mapController()->center();
    acenter.setCoordinate(center_);
    map_->mapController()->setCenter(acenter);
    map_->mapController()->setZoom(zoomLevel_);
    map_->mapController()->setBearing(bearing_);
    map_->mapController()->setTilt(tilt_);

    QList<QGeoMapType> types = mappingManager_->supportedMapTypes();
    for (int i = 0; i < types.size(); ++i) {
        QDeclarativeGeoMapType *type = new QDeclarativeGeoMapType(types[i], this);
        supportedMapTypes_.append(type);
    }

    if (!supportedMapTypes_.isEmpty()) {
        QDeclarativeGeoMapType *type = supportedMapTypes_.at(0);
        activeMapType_ = type;
        map_->setActiveMapType(type->mapType());
    }

    // Map tiles are built in this call
    map_->resize(width(), height());
    // This prefetches a buffer around the map
    map_->cameraStopped();
    map_->update();

    emit minimumZoomLevelChanged();
    emit maximumZoomLevelChanged();
    emit supportedMapTypesChanged();
    emit activeMapTypeChanged();

    // Any map items that were added before the plugin was ready
    // need to have setMap called again
    foreach (const QPointer<QDeclarativeGeoMapItemBase> &item, mapItems_) {
        if (item)
            item.data()->setMap(this, map_);
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMap::updateMapDisplay(const QRectF &target)
{
    Q_UNUSED(target);
    QQuickItem::update();
}


/*!
    \internal
*/
QDeclarativeGeoServiceProvider *QDeclarativeGeoMap::plugin() const
{
    return plugin_;
}

/*!
    \qmlproperty real QtLocation::Map::minimumZoomLevel

    This property holds the minimum valid zoom level for the map.

    The minimum zoom level is defined by the \l plugin used.
    If a plugin supporting mapping is not set, -1.0 is returned.
*/

qreal QDeclarativeGeoMap::minimumZoomLevel() const
{
    if (mappingManager_ && mappingManagerInitialized_)
        return mappingManager_->cameraCapabilities().minimumZoomLevel();
    else
        return -1.0;
}

/*!
\qmlproperty real QtLocation::Map::maximumZoomLevel

    This property holds the maximum valid zoom level for the map.

    The maximum zoom level is defined by the \l plugin used.
    If a plugin supporting mapping is not set, -1.0 is returned.
*/

qreal QDeclarativeGeoMap::maximumZoomLevel() const
{
    if (mappingManager_ && mappingManagerInitialized_)
        return mappingManager_->cameraCapabilities().maximumZoomLevel();
    else
        return -1.0;
}

/*!
    \internal
*/
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

/*!
    \qmlproperty real Map::bearing

    This property holds the current bearing (starting from 0 and increasing
    clockwise to 359,9 degrees) pointing up.

    For example setting bearing to 10 will set bearing 10 to point up, which
    visually looks like rotating the map counter-clockwise.

    You can also assign negative values, which will internally get
    translated into positive bearing (for example -10 equals 350). This is primarily for
    convenience (for example you can decrement bearing without worrying about it).
    Assigning values greater than abs(360) will be mod'd (for example 365 will result
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

/*!
    \qmlproperty real Map::tilt

    This property holds the current tilt (starting from 0 and increasing to 85 degrees).

    The tilt gives the map a 2.5D feel. Certain map objects may be rendered
    in 3D if the tilt is different from 0.

    The default value is 0 corresponding to no tilt.
*/
qreal QDeclarativeGeoMap::tilt() const
{
    if (!mappingManagerInitialized_)
        return tilt_;
    return map_->mapController()->tilt();
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

/*!
    \qmlproperty real QtLocation::Map::zoomLevel

    This property holds the zoom level for the map.

    Larger values for the zoom level provide more detail. Zoom levels
    are always non-negative. The default value is 8.0.
*/
void QDeclarativeGeoMap::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel_ == zoomLevel || zoomLevel < 0)
        return;

    if (mappingManagerInitialized_) {
        QGeoCameraCapabilities capabilities = mappingManager_->cameraCapabilities();
            if ((zoomLevel < capabilities.minimumZoomLevel() ||
                    zoomLevel > capabilities.maximumZoomLevel())) {
                return;
            }
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
\qmlproperty coordinate QtLocation::Map::center

    This property holds the coordinate which occupies the center of the
    mapping viewport.

    The default value is an arbitrary valid coordinate.
*/
void QDeclarativeGeoMap::setCenter(const QGeoCoordinate &center)
{
    if (center == center_)
        return;

    center_ = center;

    if (center_.isValid() && mappingManagerInitialized_) {
        AnimatableCoordinate acoord = map_->mapController()->center();
        acoord.setCoordinate(center_);
        map_->mapController()->setCenter(acoord);
        update();
    } else {
        emit centerChanged(center_);
    }
}

QGeoCoordinate QDeclarativeGeoMap::center() const
{
    if (mappingManagerInitialized_)
        return map_->mapController()->center().coordinate();
    else
        return center_;
}

/*!
    \internal
*/
void QDeclarativeGeoMap::mapZoomLevelChanged(qreal zoom)
{
    if (zoom == zoomLevel_)
        return;
    zoomLevel_ = zoom;
    emit zoomLevelChanged(zoomLevel_);
}

/*!
    \internal
*/
void QDeclarativeGeoMap::mapTiltChanged(qreal tilt)
{
    if (tilt == zoomLevel_)
        return;
    tilt_ = tilt;
    emit tiltChanged(tilt_);
}

/*!
    \internal
*/
void QDeclarativeGeoMap::mapBearingChanged(qreal bearing)
{
    if (bearing == bearing_)
        return;
    bearing_ = bearing;
    emit bearingChanged(bearing_);
}

/*!
    \internal
*/
void QDeclarativeGeoMap::mapCenterChanged(AnimatableCoordinate center)
{
    emit centerChanged(center.coordinate());
}

/*!
    \qmlproperty list<MapType> QtLocation::Map::supportedMapTypes

    This read-only property holds the set of \l{MapType}{map types} supported by this map.

    \sa activeMapType
*/
QQmlListProperty<QDeclarativeGeoMapType> QDeclarativeGeoMap::supportedMapTypes()
{
    return QQmlListProperty<QDeclarativeGeoMapType>(this, supportedMapTypes_);
}

/*!
    \qmlmethod QtLocation::Map::toCoordinate(QPointF screenPosition)

    Returns the coordinate which corresponds to the screen position
    \a screenPosition.

    Returns an invalid coordinate if \a screenPosition is not within
    the current viewport.
*/

QGeoCoordinate QDeclarativeGeoMap::toCoordinate(const QPointF &screenPosition) const
{
    if (map_)
        return map_->screenPositionToCoordinate(screenPosition);
    else
        return QGeoCoordinate();
}

/*!
\qmlmethod QtLocation::Map::toScreenPosition(coordinate coordinate)

    Returns the screen position which corresponds to the coordinate
    \a coordinate.

    Returns an invalid QPointF if \a coordinate is not within the
    current viewport.
*/

QPointF QDeclarativeGeoMap::toScreenPosition(const QGeoCoordinate &coordinate) const
{
    if (map_)
        return map_->coordinateToScreenPosition(coordinate);
    else
        return QPointF(qQNaN(), qQNaN());
}

/*!
    \qmlmethod void QtLocation::Map::pan(int dx, int dy)

    Starts panning the map by \a dx pixels along the x-axis and
    by \a dy pixels along the y-axis.

    Positive values for \a dx move the map right, negative values left.
    Positive values for \a dy move the map down, negative values up.

    During panning the \l center, \l tilt, \l bearing, and \l zoomLevel may change.
*/
void QDeclarativeGeoMap::pan(int dx, int dy)
{
    if (!mappingManagerInitialized_)
        return;
    map_->mapController()->pan(dx, dy);
}


/*!
    \qmlmethod void QtLocation::Map::cameraStopped()

    Optional hint that allows the map to prefetch during this idle period
*/
void QDeclarativeGeoMap::cameraStopped()
{
    if (!mappingManagerInitialized_)
        return;
    map_->cameraStopped();
}

/*!
    \internal
*/
void QDeclarativeGeoMap::touchEvent(QTouchEvent *event)
{
    if (!mappingManagerInitialized_) {
        event->ignore();
        return;
    }
    QLOC_TRACE0;
    event->accept();
    gestureArea_->touchEvent(event);
}

/*!
    \internal
*/
void QDeclarativeGeoMap::wheelEvent(QWheelEvent *event)
{
    QLOC_TRACE0;
    event->accept();
    emit wheelAngleChanged(event->angleDelta());
}

/*!
    \internal
*/
bool QDeclarativeGeoMap::childMouseEventFilter(QQuickItem *item, QEvent *event)
{
    Q_UNUSED(item)
    QLOC_TRACE0;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        return gestureArea_->filterMapChildMouseEvent(static_cast<QMouseEvent *>(event));
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        return gestureArea_->filterMapChildTouchEvent(static_cast<QTouchEvent *>(event));
    default:
        return false;
    }
}

/*!
    \qmlmethod QtLocation::Map::addMapItem(MapItem item)

    Adds the given \a item to the Map (for example MapQuickItem, MapCircle). If the object
    already is on the Map, it will not be added again.

    As an example, consider the case where you have a MapCircle representing your current position:

    \snippet declarative/maps.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/maps.qml Map addMapItem MapCircle at current position

    \note MapItemViews cannot be added with this method.

    \sa mapItems, removeMapItem, clearMapItems
*/

void QDeclarativeGeoMap::addMapItem(QDeclarativeGeoMapItemBase *item)
{
    QLOC_TRACE0;
    if (!item || item->quickMap())
        return;
    updateMutex_.lock();
    item->setParentItem(this);
    if (map_)
        item->setMap(this, map_);
    mapItems_.append(item);
    emit mapItemsChanged();
    updateMutex_.unlock();
}

/*!
    \qmlproperty list<MapItem> QtLocation::Map::mapItems

    Returns the list of all map items in no particular order.
    These items include items that were declared statically as part of
    the type declaration, as well as dynamical items (\l addMapItem,
    \l MapItemView).

    \sa addMapItem, removeMapItem, clearMapItems
*/

QList<QObject *> QDeclarativeGeoMap::mapItems()
{
    QList<QObject *> ret;
    foreach (const QPointer<QDeclarativeGeoMapItemBase> &ptr, mapItems_) {
        if (ptr)
            ret << ptr.data();
    }
    return ret;
}

/*!
    \qmlmethod void QtLocation::Map::removeMapItem(MapItem item)

    Removes the given \a item from the Map (for example MapQuickItem, MapCircle). If
    the MapItem does not exist or was not previously added to the map, the
    method does nothing.

    \sa mapItems, addMapItem, clearMapItems
*/
void QDeclarativeGeoMap::removeMapItem(QDeclarativeGeoMapItemBase *ptr)
{
    QLOC_TRACE0;
    if (!ptr || !map_)
        return;
    QPointer<QDeclarativeGeoMapItemBase> item(ptr);
    if (!mapItems_.contains(item))
        return;
    updateMutex_.lock();
    item.data()->setParentItem(0);
    item.data()->setMap(0, 0);
    // these can be optimized for perf, as we already check the 'contains' above
    mapItems_.removeOne(item);
    emit mapItemsChanged();
    updateMutex_.unlock();
}

/*!
    \qmlmethod void QtLocation::Map::clearMapItems()

    Removes all items from the map.

    \sa mapItems, addMapItem, removeMapItem
*/
void QDeclarativeGeoMap::clearMapItems()
{
    QLOC_TRACE0;
    if (mapItems_.isEmpty())
        return;
    updateMutex_.lock();
    for (int i = 0; i < mapItems_.count(); ++i) {
        if (mapItems_.at(i)) {
            mapItems_.at(i).data()->setParentItem(0);
            mapItems_.at(i).data()->setMap(0, 0);
        }
    }
    mapItems_.clear();
    emit mapItemsChanged();
    updateMutex_.unlock();
}

/*!
    \qmlproperty MapType QtLocation::Map::activeMapType

    \brief Access to the currently active \l{MapType}{map type}.

    This property can be set to change the active \l{MapType}{map type}.
    See the \l{Map::supportedMapTypes}{supportedMapTypes} property for possible values.

    \sa MapType
*/
void QDeclarativeGeoMap::setActiveMapType(QDeclarativeGeoMapType *mapType)
{
    activeMapType_ = mapType;
    map_->setActiveMapType(mapType->mapType());
    emit activeMapTypeChanged();
}

QDeclarativeGeoMapType * QDeclarativeGeoMap::activeMapType() const
{
    return activeMapType_;
}

/*!
    \internal
*/
void QDeclarativeGeoMap::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (!mappingManagerInitialized_)
        return;

    map_->resize(newGeometry.width(), newGeometry.height());
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

/*!
    \qmlmethod QtLocation::Map::fitViewportToMapItems()

    Fits the current viewport to the boundary of all map items. The camera is positioned
    in the center of the map items, and at the largest integral zoom level possible which
    allows all map items to be visible on screen

*/
void QDeclarativeGeoMap::fitViewportToMapItems()
{
    if (!mappingManagerInitialized_)
        return;
    fitViewportToMapItemsRefine(true);
}

/*!
    \internal
*/
void QDeclarativeGeoMap::fitViewportToMapItemsRefine(bool refine)
{
    if (mapItems_.size() == 0)
        return;

    qreal minX = 0;
    qreal maxX = 0;
    qreal minY = 0;
    qreal maxY = 0;
    qreal topLeftX = 0;
    qreal topLeftY = 0;
    qreal bottomRightX = 0;
    qreal bottomRightY = 0;

    // find bounds of all map items
    QGeoCoordinate geoCenter;
    QPointF centerPt;
    int itemCount = 0;
    for (int i = 0; i < mapItems_.count(); ++i) {
        if (!mapItems_.at(i))
            continue;
        QDeclarativeGeoMapItemBase *item = mapItems_.at(i).data();

        // skip quick items in the first pass and refine the fit later
        if (refine) {
            QDeclarativeGeoMapQuickItem *quickItem =
                    qobject_cast<QDeclarativeGeoMapQuickItem*>(item);
            if (quickItem)
                    continue;
        }

        // account for the special case - circle
        QDeclarativeCircleMapItem *circleItem =
                qobject_cast<QDeclarativeCircleMapItem *>(item);

        if ((!circleItem || !circleItem->center().isValid()) && !item)
            continue;
        if (circleItem) {
            geoCenter = circleItem->center();
            centerPt = map_->coordinateToScreenPosition(geoCenter, false);
            topLeftX = centerPt.x() - circleItem->width() / 2.0;
            topLeftY = centerPt.y() - circleItem->height() / 2.0;
            bottomRightX = centerPt.x() + circleItem->width() / 2.0;
            bottomRightY = centerPt.y() + circleItem->height() / 2.0;
        } else if (item) {
            topLeftX = item->position().x();
            topLeftY = item->position().y();
            bottomRightX = topLeftX + item->width();
            bottomRightY = topLeftY + item->height();
            ++itemCount;
        }
        if (itemCount == 0) {
            minX = topLeftX;
            maxX = bottomRightX;
            minY = topLeftY;
            maxY = bottomRightY;
        } else {
            minX = qMin(minX, topLeftX);
            maxX = qMax(maxX, bottomRightX);
            minY = qMin(minY, topLeftY);
            maxY = qMax(maxY, bottomRightY);
        }
        ++itemCount;
    }

    if (itemCount == 0)
        return;

    qreal bboxWidth = maxX - minX;
    qreal bboxHeight = maxY - minY;
    qreal bboxCenterX = minX + (bboxWidth / 2.0);
    qreal bboxCenterY = minY + (bboxHeight / 2.0);

    // position camera to the center of bounding box
    QGeoCoordinate coordinate;
    coordinate = map_->screenPositionToCoordinate(QPointF(bboxCenterX, bboxCenterY), false);
    AnimatableCoordinate acenter = map_->mapController()->center();
    acenter.setCoordinate(coordinate);
    setCenter(coordinate);

    // adjust zoom
    double bboxWidthRatio = bboxWidth / (bboxWidth + bboxHeight);
    double mapWidthRatio = width() / (width() + height());
    double zoomRatio;

    if (bboxWidthRatio > mapWidthRatio)
        zoomRatio = bboxWidth / width();
    else
        zoomRatio = bboxHeight / height();

    qreal newZoom = log10(zoomRatio) / log10(0.5);
    newZoom = floor(qMax(minimumZoomLevel(), (map_->mapController()->zoom() + newZoom)));
    setZoomLevel(newZoom);

    // as map quick items retain the same screen size after the camera zooms in/out
    // we refine the viewport again to achieve better results
    if (refine)
        fitViewportToMapItemsRefine(false);
}

#include "moc_qdeclarativegeomap_p.cpp"

QT_END_NAMESPACE
