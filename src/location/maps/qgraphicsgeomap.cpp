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

#include "qgraphicsgeomap.h"
#include "qgraphicsgeomap_p.h"

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgeomapobject.h"
#include "qgeomapoverlay.h"

#include "qgeoserviceprovider.h"
#include "qgeomappingmanager.h"
#include "qgeomapdata.h"

#include <QGraphicsSceneResizeEvent>
#include <QTimer>

#include <QNetworkProxyFactory>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QGraphicsGeoMap
    \brief The QGraphicsGeoMap class is used to display a map and manager the
    interactions between the user and the map.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps-mapping

    Most of the functionality is provided by QGeoMappingManager, which
    handles most aspects of the display.

    The map viewport can be panned, the zoom level can be changed and the
    center coordinate of the viewport can be set explicitly.

    The screenPositionToCoordinate() and coordinateToScreenPoisition()
    functions can be used to convert between positions on the screen and
    global coordinates.

    Mouse and keyboard events should be handled by subclassing QGraphicsGeoMap
    and providing implementations of the event handling functions present in
    QGraphicsWidget.
*/

/*!
\enum QGraphicsGeoMap::MapType

Describes a type of map data.

\value NoMap
Indicates a lack of map valid data.

\value StreetMap
The map data is a graphical representation of streets and building boundaries.

\value SatelliteMapDay
The map data is composed of images collected by satellites during the daytime.

\value SatelliteMapNight
The map data is composed of images collected by satellites during the nighttime.

\value TerrainMap
The map data is a graphical representation of terrain features.  This may also
include some of the information provided by QGraphicsGeoMap::StreetMap.
*/

/*!
\enum QGraphicsGeoMap::ConnectivityMode

Describes the method of obtaining the mapping data.

\value NoConnectivity
There is no map data.

\value OfflineMode
The map data will come from an offline source.

\value OnlineMode
The map data will come from an online source.

\value HybridMode
The map data will come from a combination of offline and online sources.
*/

/*!
    Creates a new mapping widget, with the mapping operations managed by
    \a manager, and the specified \a parent.

    Note that the \a manager will typically be accessed from an instance of
    QGeoServiceProvider:
    \code
        QGeoServiceProvider serviceProvider("nokia");
        QGeoMappingManager *manager = serviceProvider.mappingManager();
        QGraphicsGeoMap *widget = new QGraphicsGeoMap(manager);
    \endcode
*/
QGraphicsGeoMap::QGraphicsGeoMap(QGeoMappingManager *manager, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
      d_ptr(new QGraphicsGeoMapPrivate())
{
    Q_ASSERT(manager != 0);
    d_ptr->manager = manager;

    d_ptr->mapData = d_ptr->manager->createMapData();
    d_ptr->mapData->init();

    connect(d_ptr->mapData,
            SIGNAL(updateMapDisplay(QRectF)),
            this,
            SLOT(updateMapDisplay(QRectF)));

    setMapType(QGraphicsGeoMap::StreetMap);
    d_ptr->mapData->setWindowSize(size());

    connect(d_ptr->mapData,
            SIGNAL(zoomLevelChanged(qreal)),
            this,
            SIGNAL(zoomLevelChanged(qreal)));
    connect(d_ptr->mapData,
            SIGNAL(bearingChanged(qreal)),
            this,
            SIGNAL(bearingChanged(qreal)));
    connect(d_ptr->mapData,
            SIGNAL(tiltChanged(qreal)),
            this,
            SIGNAL(tiltChanged(qreal)));
    connect(d_ptr->mapData,
            SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)),
            this,
            SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)));
    connect(d_ptr->mapData,
            SIGNAL(centerChanged(QGeoCoordinate)),
            this,
            SIGNAL(centerChanged(QGeoCoordinate)));
    connect(d_ptr->mapData,
            SIGNAL(connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)),
            this,
            SIGNAL(connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)));

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    setMinimumSize(QSizeF(0, 0));
    setPreferredSize(QSizeF(500, 500));
}

/*!
    Destroys this map widget.
*/
QGraphicsGeoMap::~QGraphicsGeoMap()
{
    delete d_ptr;
}

/*!
  \reimp
*/
void QGraphicsGeoMap::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (d_ptr->mapData) {
        d_ptr->mapData->setWindowSize(event->newSize());
    }
}

/*!
  \reimp
*/
QPainterPath QGraphicsGeoMap::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/*!
  \reimp
*/
void QGraphicsGeoMap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (d_ptr->mapData)
        d_ptr->mapData->paint(painter, option);
}

void QGraphicsGeoMap::updateMapDisplay(const QRectF &target)
{
    update(target);
}

/*!
    \property QGraphicsGeoMap::minimumZoomLevel
    \brief This property holds the minimum zoom level supported by the
    QGeoMappingManager associated with this widget.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGraphicsGeoMap::minimumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->minimumZoomLevel();

    return -1;
}

/*!
    \property QGraphicsGeoMap::maximumZoomLevel
    \brief This property holds the maximum zoom level supported by the
    QGeoMappingManager associated with this widget.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGraphicsGeoMap::maximumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->maximumZoomLevel();

    return -1;
}

/*!
    \property QGraphicsGeoMap::zoomLevel
    \brief This property holds the zoom level of the map.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    If zoomLevel is less than minimumZoomLevel then minimumZoomLevel
    will be used, and if zoomLevel is  larger than
    maximumZoomLevel then maximumZoomLevel will be used.
*/
void QGraphicsGeoMap::setZoomLevel(qreal zoomLevel)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setZoomLevel(zoomLevel);
}

qreal QGraphicsGeoMap::zoomLevel() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->zoomLevel();

    return -1;
}
/*!
    \property QGraphicsGeoMap::supportsBearing
    \brief This property holds whether bearing is supported by the
    QGeoMappingManager associated with this widget.
*/
bool QGraphicsGeoMap::supportsBearing() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->supportsBearing();

    return false;
}

/*!
    \property QGraphicsGeoMap::bearing
    \brief This property holds the bearing of the map.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
void QGraphicsGeoMap::setBearing(qreal bearing)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setBearing(bearing);
}

qreal QGraphicsGeoMap::bearing() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->bearing();

    return 0;
}

/*!
    \property QGraphicsGeoMap::supportsTilting
    \brief This property holds whether tilting is supported by the
    QGeoMappingManager associated with this widget.
*/
bool QGraphicsGeoMap::supportsTilting() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->supportsTilting();

    return false;
}

/*!
    \property QGraphicsGeoMap::minimumTilt
    \brief This property holds the minimum tilt supported by the
    QGeoMappingManager associated with this widget.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
qreal QGraphicsGeoMap::minimumTilt() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->minimumTilt();

    return 0;
}

/*!
    \property QGraphicsGeoMap::maximumTilt
    \brief This property holds the maximum tilt supported by the
    QGeoMappingManager associated with this widget.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
qreal QGraphicsGeoMap::maximumTilt() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->maximumTilt();

    return 0;
}

/*!
    \property QGraphicsGeoMap::tilt
    \brief This property holds the tilt of the map.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.

    If \a tilt is less than minimumTilt() then minimumTilt()
    will be used, and if \a tilt is  larger than
    maximumTilt() then maximumTilt() will be used.
*/
void QGraphicsGeoMap::setTilt(qreal tilt)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setTilt(tilt);
}

qreal QGraphicsGeoMap::tilt() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->tilt();

    return 0;
}

/*!
    Pans the map view \a dx pixels in the x direction and \a dy pixels
    in the y direction.

    The x and y axes are specified in Graphics View Framework coordinates.
    By default this will mean that positive values of \a dx move the
    viewed area to the right and that positive values of \a dy move the
    viewed area down.

    After the panning has occurred the centerChanged() signal will be emitted.
*/
void QGraphicsGeoMap::pan(int dx, int dy)
{
    if (d_ptr->mapData) {
        d_ptr->mapData->pan(dx, dy);
        update();
    }
}

/*!
    \property QGraphicsGeoMap::center
    \brief This property holds the coordinate at the center of the map viewport.

    Panning the map can be more efficient than changing the center by small
    increments.
*/
void QGraphicsGeoMap::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setCenter(center);
}

QGeoCoordinate QGraphicsGeoMap::center() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->center();

    return QGeoCoordinate();
}

/*!
    Returns the map types supported by the QGeoMappingManager associated with
    this widget.
*/
QList<QGraphicsGeoMap::MapType> QGraphicsGeoMap::supportedMapTypes() const
{
    if (d_ptr->manager)
        return d_ptr->manager->supportedMapTypes();

    return QList<QGraphicsGeoMap::MapType>();
}

/*!
  \property QGraphicsGeoMap::mapType
  \brief This property holds the type of map data displayed by the map.

  Setting mapType to a type not present in supportedMapTypes() will do
  nothing.
*/
void QGraphicsGeoMap::setMapType(QGraphicsGeoMap::MapType mapType)
{
    if (d_ptr->mapData && d_ptr->manager) {
        if (!d_ptr->manager->supportedMapTypes().contains(mapType))
            return;

        d_ptr->mapData->setMapType(mapType);
    }
}

QGraphicsGeoMap::MapType QGraphicsGeoMap::mapType() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapType();

    return QGraphicsGeoMap::NoMap;
}

/*!
    Returns the connectivity modes supported by the QGeoMappingManager associated with
    this widget.
*/
QList<QGraphicsGeoMap::ConnectivityMode> QGraphicsGeoMap::supportedConnectivityModes() const
{
    if (d_ptr->manager)
        return d_ptr->manager->supportedConnectivityModes();

    return QList<QGraphicsGeoMap::ConnectivityMode>();
}

/*!
  \property QGraphicsGeoMap::connectivityMode
  \brief This property holds the connectivity mode used to obtain the map data.

  Setting connectivityMode to a mode not present in supportedConnectivityModes() will do
  nothing.
*/
void QGraphicsGeoMap::setConnectivityMode(QGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    if (d_ptr->mapData && d_ptr->manager) {
        if (!d_ptr->manager->supportedConnectivityModes().contains(connectivityMode))
            return;

        d_ptr->mapData->setConnectivityMode(connectivityMode);
    }
}

QGraphicsGeoMap::ConnectivityMode QGraphicsGeoMap::connectivityMode() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->connectivityMode();

    return QGraphicsGeoMap::NoConnectivity;
}

/*!
    Returns whether custom map objects are supported by this engine.

    Custom map objects are map objects based on QGraphicsItem instances, which
    are hard to support in cases where the map rendering is not being
    performed by the Qt Graphics View framwork.
*/
bool QGraphicsGeoMap::supportsCustomMapObjects() const
{
    if (d_ptr->manager)
        return d_ptr->manager->supportsCustomMapObjects();

    return false;
}

/*!
    Returns the map objects associated with this map.
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjects() const
{
    if (!d_ptr->mapData)
        return QList<QGeoMapObject*>();

    return d_ptr->mapData->mapObjects();
}

/*!
    Adds \a mapObject to the list of map objects managed by this widget.

    If \a mapObject is within the viewport of the map and
    QGeoMapObject::isVisible() returns true then the map will display the map
    object immediately.

    The map will take ownership of the \a mapObject.

    If supportsCustomMapObject() returns false and \a mapObject is a custom map
    object then \a mapObject will not be added to the map.
*/
void QGraphicsGeoMap::addMapObject(QGeoMapObject *mapObject)
{
    if (!mapObject || !d_ptr->mapData)
        return;

    if ((mapObject->type() == QGeoMapObject::CustomType) && !supportsCustomMapObjects())
        return;

    d_ptr->mapData->addMapObject(mapObject);

    this->update();
}

/*!
    Removes \a mapObject from the list of map objects managed by this widget.

    If \a mapObject is within the viewport of the map and
    QGeoMapObject::isVisible() returns true then the map will stop displaying
    the map object immediately.

    The map will release ownership of the \a mapObject.
*/
void QGraphicsGeoMap::removeMapObject(QGeoMapObject *mapObject)
{
    if (!mapObject || !d_ptr->mapData)
        return;

    d_ptr->mapData->removeMapObject(mapObject);

    this->update();
}

/*!
    Clears the map objects associated with this map.

    The map objects will be deleted.
*/
void QGraphicsGeoMap::clearMapObjects()
{
    if (!d_ptr->mapData)
        return;

    d_ptr->mapData->clearMapObjects();
}

/*!
    Returns the map overlays associated with this map.
*/
QList<QGeoMapOverlay*> QGraphicsGeoMap::mapOverlays() const
{
    if (!d_ptr->mapData)
        return QList<QGeoMapOverlay*>();

    return d_ptr->mapData->mapOverlays();
}

/*!
    Adds \a overlay to the list of map overlays associated with this map.

    The overlays will be drawn in the order in which they were added.

    The map will take ownership of \a overlay.
*/
void QGraphicsGeoMap::addMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay || !d_ptr->mapData)
        return;

    d_ptr->mapData->addMapOverlay(overlay);

    this->update();
}

/*!
    Removes \a overlay from the list of map overlays associated with this map.

    The map will release ownership of \a overlay.
*/
void QGraphicsGeoMap::removeMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay || !d_ptr->mapData)
        return;

    d_ptr->mapData->removeMapOverlay(overlay);

    this->update();
}

/*!
    Clears the map overlays associated with this map.

    The map overlays will be deleted.
*/
void QGraphicsGeoMap::clearMapOverlays()
{
    if (!d_ptr->mapData)
        return;

    d_ptr->mapData->clearMapOverlays();
}

/*!
    Returns a bounding box corresponding to the physical area displayed
    in the viewport of the map.

    The bounding box which is returned is defined by the upper left and
    lower right corners of the visible area of the map.
*/
QGeoBoundingBox QGraphicsGeoMap::viewport() const
{
    if (!d_ptr->mapData)
        return QGeoBoundingBox();

    return d_ptr->mapData->viewport();
}

/*!
    Attempts to fit the bounding box \a bounds into the viewport of the map.

    This method will change the zoom level to the maximum zoom level such
    that all of \a bounds is visible within the resulting viewport.

    If \a preserveViewportCenter is false the map will be centered on the
    bounding box \a bounds before the zoom level is changed, otherwise the
    center of the map will not be changed.
*/
void QGraphicsGeoMap::fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter)
{
    if (!d_ptr->mapData)
        return;

    d_ptr->mapData->fitInViewport(bounds, preserveViewportCenter);
}

/*!
    Returns the list of visible map objects managed by this widget which
    contain the point \a screenPosition within their boundaries.
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjectsAtScreenPosition(const QPointF &screenPosition) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsAtScreenPosition(screenPosition);

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of visible map objects managed by this widget which are
    displayed at least partially within the on screen rectangle
    \a screenRect.
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjectsInScreenRect(const QRectF &screenRect) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsInScreenRect(screenRect);

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of visible map objects manager by this widget which
    are displayed at least partially within the viewport of the map.
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjectsInViewport() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsInViewport();

    return QList<QGeoMapObject*>();
}

/*!
    Returns the position on the screen at which \a coordinate is displayed.

    An invalid QPointF will be returned if \a coordinate is invalid or is not
    within the current viewport.
*/
QPointF QGraphicsGeoMap::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->coordinateToScreenPosition(coordinate);

    return QPointF();
}

/*!
    Returns the coordinate corresponding to the point in the viewport at \a
    screenPosition.

    An invalid QGeoCoordinate will be returned if \a screenPosition is invalid
    or is not within the current viewport.
*/
QGeoCoordinate QGraphicsGeoMap::screenPositionToCoordinate(QPointF screenPosition) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->screenPositionToCoordinate(screenPosition);

    return QGeoCoordinate();
}

/*!
\fn void QGraphicsGeoMap::zoomLevelChanged(qreal zoomLevel)

    This signal is emitted when the zoom level of the map changes.

    The new value is \a zoomLevel.
*/

/*!
\fn void QGraphicsGeoMap::bearingChanged(qreal bearing)

    This signal is emitted when the bearing of the map changes.

    The new value is \a bearing.
*/

/*!
\fn void QGraphicsGeoMap::tiltChanged(qreal tilt)

    This signal is emitted when the tilt of the map changes.

    The new value is \a tilt.
*/

/*!
\fn void QGraphicsGeoMap::centerChanged(const QGeoCoordinate &coordinate)

    This signal is emitted when the center of the map changes.

    The new value is \a coordinate.
*/

/*!
\fn void QGraphicsGeoMap::mapTypeChanged(QGraphicsGeoMap::MapType mapType)

    This signal is emitted when the map type changes.

    The new value is \a mapType.
*/

/*!
\fn void QGraphicsGeoMap::connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)

    This signal is emitted when the connectivity mode used to fetch the
    map data changes.

    The new value is \a connectivityMode.
*/

/*******************************************************************************
*******************************************************************************/

QGraphicsGeoMapPrivate::QGraphicsGeoMapPrivate()
      : manager(0),
      mapData(0),
      panActive(false) {}

QGraphicsGeoMapPrivate::~QGraphicsGeoMapPrivate()
{
    if (mapData)
        delete mapData;
}

#include "moc_qgraphicsgeomap.cpp"

QTM_END_NAMESPACE
