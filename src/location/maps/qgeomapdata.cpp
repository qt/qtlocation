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

#include "qgeomapdata.h"
#include "qgeomapdata_p.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>

#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeomapobject.h"
#include "qgeomappixmapobject.h"
#include "qgeomapgroupobject.h"
#include "qgeomaptextobject.h"
#include "qgeomappingmanagerengine.h"
#include "qgeomapoverlay.h"

#include "qgeomapobjectengine_p.h"
#include "qgeomapobject_p.h"
#include "projwrapper_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapData

    \brief The QGeoMapData class are used as a bridge between QGraphicsGeoMap and
    QGeoMappingManager.


    \inmodule QtLocation
    \since 1.1

    \ingroup maps-impl

    Instances of QGeoMapData are created with
    QGeoMappingManager::createMapData(), and are used internally by
    QGraphicsGeoMap to manage the state of the map and the associated
    QGeoMapObject instances.

    Plugin implementers will need to provide implementations of
    coordinateToScreenPosition(const QGeoCoordinate &coordinate) and
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition).

    The other virtual functions can be overridden. If the screen position to
    coordinate tranformations are expensive then overriding these functions may
    allow optimizations based on caching parts of the geometry information.

    Subclasses should override createMapObjectInfo() so that QGeoMapObjectInfo
    instances will be created for each QGeoMapObject type in order to provide
    the QGeoMapData subclass specific behaviours for the map objects.
 */

/*!
    Constructs a new map data object, which makes use of the functionality provided by \a engine.
*/
QGeoMapData::QGeoMapData(QGeoMappingManagerEngine *engine)
    : d_ptr(new QGeoMapDataPrivate(this, engine))
{
    if (engine->supportedConnectivityModes().length() > 0)
        setConnectivityMode(engine->supportedConnectivityModes().at(0));
    else
        setConnectivityMode(QGraphicsGeoMap::NoConnectivity);
}

/*!
  \internal
*/
QGeoMapData::QGeoMapData(QGeoMapDataPrivate *dd) : d_ptr(dd) {}

/*!
    Destroys this map data object.
*/
QGeoMapData::~QGeoMapData()
{
    Q_D(QGeoMapData);
    delete d;
}

/*!
    This function is run after the QGeoMapData instance has been
    constructed.

    Any subclasses which override this function should make sure that
    QGeoMapData::init() is called within the body of the overriding function.
*/
void QGeoMapData::init()
{
    d_ptr->containerObject = new QGeoMapGroupObject();
    d_ptr->containerObject->setMapData(this);
}

/*!
    Returns the mapping engine that this map data object is associated with.
*/
QGeoMappingManagerEngine* QGeoMapData::engine() const
{
    return d_ptr->engine;
}

/*!
    Returns the QGeoMapObject which acts as the parent to all QGeoMapObject
    instances which are added to the map by the user.
*/
QGeoMapObject* QGeoMapData::containerObject()
{
    return d_ptr->containerObject;
}

/*!
    Sets the size of the map viewport to \a size.

    The size will be adjusted by the associated QGraphicsGeoMap as it resizes.
*/
void QGeoMapData::setWindowSize(const QSizeF &size)
{
    if (d_ptr->windowSize == size)
        return;

    d_ptr->windowSize = size;

    if (!d_ptr->blockPropertyChangeSignals)
        emit windowSizeChanged(d_ptr->windowSize);
}

/*!
    \property QGeoMapData::windowSize
    Returns the size of the map viewport.

    The size will be adjusted by the associated QGraphicsGeoMap as it resizes.
*/
QSizeF QGeoMapData::windowSize() const
{
    return d_ptr->windowSize;
}

/*!
    Sets the zoom level of the map to \a zoomLevel.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    If \a zoomLevel is less than minimumZoomLevel() then minimumZoomLevel()
    will be used, and if \a zoomLevel is  larger than
    maximumZoomLevel() then maximumZoomLevel() will be used.
*/
void QGeoMapData::setZoomLevel(qreal zoomLevel)
{
    zoomLevel = qMin(zoomLevel, d_ptr->engine->maximumZoomLevel());
    zoomLevel = qMax(zoomLevel, d_ptr->engine->minimumZoomLevel());

    if (d_ptr->zoomLevel == zoomLevel)
        return;

    d_ptr->zoomLevel = zoomLevel;

    if (!d_ptr->blockPropertyChangeSignals)
        emit zoomLevelChanged(d_ptr->zoomLevel);
}

/*!
    \property QGeoMapData::zoomLevel

    Returns the zoom level of the map.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMapData::zoomLevel() const
{
    return d_ptr->zoomLevel;
}

/*!
    \property QGeoMapData::supportsBearing

    Returns whether bearing is supported by this engine.
*/
bool QGeoMapData::supportsBearing() const
{
    return d_ptr->engine->supportsBearing();
}

/*!
    Sets the bearing of the map to \a bearing.

    Value in degrees in the range of 0-360. 0 being equivalent to 0 degrees from
    north.
*/
void QGeoMapData::setBearing(qreal bearing)
{
    if (!supportsBearing())
        return;

    bearing = qMin(bearing, qreal(360.0));
    bearing = qMax(bearing, qreal(0.0));

    if (d_ptr->bearing == bearing)
        return;

    d_ptr->bearing = bearing;

    if (!d_ptr->blockPropertyChangeSignals)
        emit bearingChanged(d_ptr->bearing);
}

/*!
    \property QGeoMapData::bearing

    Returns the current bearing of the map.

    Value in degrees in the range of 0-360. 0 being equivalent to 0 degrees from
    north.
*/
qreal QGeoMapData::bearing() const
{
    return d_ptr->bearing;
}

/*!
    \property QGeoMapData::supportsTilting

    Returns whether tilting is supported by this engine.
*/
bool QGeoMapData::supportsTilting() const
{
    return d_ptr->engine->supportsTilting();
}

/*!
    \property QGeoMapData::minimumTilt

    Returns minimum tilt supported by this engine.
*/
qreal QGeoMapData::minimumTilt() const
{
    return d_ptr->engine->minimumTilt();
}

/*!
    \property QGeoMapData::maximumTilt

    Returns maximum tilt supported by this engine.
*/
qreal QGeoMapData::maximumTilt() const
{
    return d_ptr->engine->maximumTilt();
}

/*!
    Sets the tilt of the map to \a tilt.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.

    If \a tilt is less than minimumTilt() then minimumTilt()
    will be used, and if \a tilt is  larger than
    maximumTilt() then maximumTilt() will be used.
*/
void QGeoMapData::setTilt(qreal tilt)
{
    if (!supportsTilting())
        return;

    tilt = qMin(tilt, d_ptr->engine->maximumTilt());
    tilt = qMax(tilt, d_ptr->engine->minimumTilt());

    if (d_ptr->tilt == tilt)
        return;

    d_ptr->tilt = tilt;

    if (!d_ptr->blockPropertyChangeSignals)
        emit tiltChanged(d_ptr->tilt);
}

/*!
    \property QGeoMapData::tilt

    Returns the current tilt of the map.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
qreal QGeoMapData::tilt() const
{
    return d_ptr->tilt;
}

/*!
    Pans the map view \a dx pixels in the x direction and \a dy pixels
    in the y direction.

    The x and y axes are specified in Graphics View Framework coordinates.
    By default this will mean that positive values of \a dx move the
    viewed area to the right and that positive values of \a dy move the
    viewed area down.

    Subclasses should call QGeoMapData::setCenter() when the pan has completed.
*/
void QGeoMapData::pan(int dx, int dy)
{
    QPointF pos = coordinateToScreenPosition(center());
    setCenter(screenPositionToCoordinate(QPointF(pos.x() + dx, pos.y() + dy)));
}

/*!
    Centers the map viewport on the coordinate \a center.
*/
void QGeoMapData::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->center == center)
        return;

    d_ptr->center = center;

    if (!d_ptr->blockPropertyChangeSignals)
        emit centerChanged(d_ptr->center);
}

/*!
    \property QGeoMapData::center

    Returns the coordinate of the point in the center of the map viewport.
*/
QGeoCoordinate QGeoMapData::center() const
{
    return d_ptr->center;
}

/*!
    Changes the type of map data to display to \a mapType.
*/
void QGeoMapData::setMapType(QGraphicsGeoMap::MapType mapType)
{
    if (d_ptr->mapType == mapType)
        return;

    d_ptr->mapType = mapType;

    if (!d_ptr->blockPropertyChangeSignals)
        emit mapTypeChanged(mapType);
}

/*!
    \property QGeoMapData::mapType

    Returns the type of map data which is being displayed.
*/
QGraphicsGeoMap::MapType QGeoMapData::mapType() const
{
    return d_ptr->mapType;
}

/*!
    Changes the connectivity mode of this map to \a connectivityMode
*/
void QGeoMapData::setConnectivityMode(QGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    if (d_ptr->connectivityMode == connectivityMode)
        return;

    d_ptr->connectivityMode = connectivityMode;

    if (!d_ptr->blockPropertyChangeSignals)
        emit connectivityModeChanged(connectivityMode);
}

/*!
    \property QGeoMapData::connectivityMode

    Returns the connectivity mode for this map.
*/
QGraphicsGeoMap::ConnectivityMode QGeoMapData::connectivityMode() const
{
    return d_ptr->connectivityMode;
}

/*!
    Returns the map objects associated with this map.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjects() const
{
    return d_ptr->containerObject->childObjects();
}

/*!
    Adds \a mapObject to the list of map objects managed by this map.

    The children objects are drawn in order of the QGeoMapObject::zValue()
    value.  Children objects having the same z value will be drawn
    in the order they were added.

    The map will take ownership of the \a mapObject.
*/
void QGeoMapData::addMapObject(QGeoMapObject *mapObject)
{
    d_ptr->addObject(mapObject);
}

/*!
    Removes \a mapObject from the list of map objects managed by this map.
    The map will release ownership of the \a mapObject.
*/
void QGeoMapData::removeMapObject(QGeoMapObject *mapObject)
{
    d_ptr->removeObject(mapObject);
}

/*!
    Clears the map objects associated with this map.

    The map objects will be deleted.
*/
void QGeoMapData::clearMapObjects()
{
    d_ptr->clearObjects();
}

/*!
    \fn QGeoBoundingBox QGeoMapData::viewport() const
    Returns a bounding box corresponding to the physical area displayed
    in the viewport of the map.

    The bounding box which is returned is defined by the upper left and
    lower right corners of the visible area of the map.
*/

/*!
    \fn void QGeoMapData::fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter)

    Attempts to fit the bounding box \a bounds into the viewport of the map.

    This method will change the zoom level to the maximum zoom level such
    that all of \a bounds is visible within the resulting viewport.

    If \a preserveViewportCenter is false the map will be centered on the
    bounding box \a bounds before the zoom level is changed, otherwise the
    center of the map will not be changed.
*/

/*!
    Returns the list of visible map objects managed by this map which
    contain the point \a screenPosition within their boundaries.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition) const
{
    QList<QGeoMapObject*> results;

    QGeoCoordinate coord = screenPositionToCoordinate(screenPosition);
    int childObjectCount = d_ptr->containerObject->childObjects().count();
    for (int i = 0; i < childObjectCount; ++i) {
        QGeoMapObject *object = d_ptr->containerObject->childObjects().at(i);
        if (object->contains(coord) && object->isVisible())
            results.append(object);
    }

    return results;
}

/*!
    Returns the list of visible map objects managed by this map which are displayed at
    least partially within the on screen rectangle \a screenRect.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsInScreenRect(const QRectF &screenRect) const
{
    QList<QGeoMapObject*> results;

    QGeoCoordinate topLeft = screenPositionToCoordinate(screenRect.topLeft());
    QGeoCoordinate bottomRight = screenPositionToCoordinate(screenRect.bottomRight());

    QGeoBoundingBox bounds(topLeft, bottomRight);

    int childObjectCount = d_ptr->containerObject->childObjects().count();
    for (int i = 0; i < childObjectCount; ++i) {
        QGeoMapObject *object = d_ptr->containerObject->childObjects().at(i);
        if (bounds.intersects(object->boundingBox()) && object->isVisible())
            results.append(object);
    }

    return results;
}

/*!
    Returns the list of visible map objects manager by this widget which
    are displayed at least partially within the viewport of the map.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsInViewport() const
{
    return this->mapObjectsInScreenRect(QRectF(0.0,
                                               0.0,
                                               d_ptr->windowSize.width(),
                                               d_ptr->windowSize.height()));
}

/*!
\fn QPointF QGeoMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const

    Returns the position on the screen at which \a coordinate is displayed.

    An invalid QPointF will be returned if \a coordinate is invalid or is not
    within the current viewport.
*/

/*!
\fn QGeoCoordinate QGeoMapData::screenPositionToCoordinate(const QPointF &screenPosition) const

    Returns the coordinate corresponding to the point in the viewport at \a
    screenPosition.

    An invalid QGeoCoordinate will be returned if \a screenPosition is invalid
    or is not within the current viewport.
*/

/*!
    Paints the map and everything associated with it on \a painter, using the
    options \a option.

    This will paint the map with paintMap(), then the map overlays with
    QGeoMapOverlay::paint(), then the map objects with paintObjects(), and
    finally paintProviderNotices().
*/
void QGeoMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    paintMap(painter, option);

    for (int i = 0; i < d_ptr->overlays.size(); ++i)
        d_ptr->overlays[i]->paint(painter, option);

    paintObjects(painter, option);

    paintProviderNotices(painter, option);
}

/*!
    Paints the map on \a painter, using the options \a option.

    The map overlays, map objects and the provider notices (such as copyright
    and powered by notices) are painted in separate methods, which are combined
    in the paint() method.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Paints the map objects on \a painter, using the options \a option.

    The default implementation makes use of the coordinateToScreenPosition
    implemented by the subclass to perform object positioning and rendering.

    This implementation should suffice for most common use cases, and supports
    the full range of coordinate systems and transforms available to a
    QGeoMapObject.
*/
void QGeoMapData::paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Paints the provider notices on \a painter, using the options \a option.

    The provider notices are things like the copyright and powered by notices.

    The provider may not want the client developers to be able to move the
    notices from their standard positions and so we have not provided API
    support for specifying the position of the notices at this time.

    If support for hinting at the position of the notices is to be provided by
    plugin parameters, the suggested parameter keys are
    "mapping.notices.copyright.alignment" and
    "mapping.notices.poweredby.alignment", with type Qt::Alignment.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintProviderNotices(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Creates a QGeoMapObjectInfo instance which implements the behaviours o
    the map object \a object which are specific to this QGeoMapData.

    The default implementation returns 0.
*/
QGeoMapObjectInfo *QGeoMapData::createMapObjectInfo(QGeoMapObject *object)
{
    Q_UNUSED(object);
    return 0;
}

/*!
    Returns the map overlays associated with this map.
*/
QList<QGeoMapOverlay*> QGeoMapData::mapOverlays() const
{
    return d_ptr->overlays;
}

/*!
    Adds \a overlay to the list of map overlays associated with this map.

    The overlays will be drawn in the order in which they were added.

    The map will take ownership of \a overlay.
*/
void QGeoMapData::addMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    overlay->setMapData(this);
    d_ptr->overlays.append(overlay);
}

/*!
    Removes \a overlay from the list of map overlays associated with this map.

    The map will release ownership of \a overlay.
*/
void QGeoMapData::removeMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    d_ptr->overlays.removeAll(overlay);
}

/*!
    Clears the map overlays associated with this map.

    The map overlays will be deleted.
*/
void QGeoMapData::clearMapOverlays()
{
    qDeleteAll(d_ptr->overlays);
    d_ptr->overlays.clear();
}


/*!
    Sets whether changes to properties will trigger their corresponding signals to \a block.

    By default the QGeoMapData implementations of the property functions are used
    which cause the property notification signals to be emitted immediately.

    Calling this function with \a block set to true will prevent these
    signals from being called, which will allow a subclass to defer the
    emission of the signal until a later time.

    If this function needs to be called it should be used as soon as possible,
    preferably in the constructor of the QGeoMapData subclass.
*/
void QGeoMapData::setBlockPropertyChangeSignals(bool block)
{
    d_ptr->blockPropertyChangeSignals = block;
}

/*!
\fn void QGeoMapData::windowSizeChanged(const QSizeF &windowSize)

    This signal is emitted when the size of the window which contains
    the map has changed.

    The new value is \a windowSize.
*/

/*!
\fn void QGeoMapData::zoomLevelChanged(qreal zoomLevel)

    This signal is emitted when the zoom level of the map has changed.

    The new value is \a zoomLevel.
*/

/*!
\fn void QGeoMapData::bearingChanged(qreal bearing)

    This signal is emitted when the bearing of the map has changed.

    The new value is \a bearing.
*/

/*!
\fn void QGeoMapData::tiltChanged(qreal tilt)

    This signal is emitted when the tilt of the map has changed.

    The new value is \a tilt.
*/

/*!
\fn void QGeoMapData::centerChanged(const QGeoCoordinate &coordinate)

    This signal is emitted when the center of the map has changed.

    The new value is \a coordinate.
*/

/*!
\fn void QGeoMapData::mapTypeChanged(QGraphicsGeoMap::MapType mapType)

    This signal is emitted when the type of the map has changes.

    The value is \a mapType.
*/

/*!
\fn void QGeoMapData::connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)

    This signal is emitted when the connectivity mode used to fetch the
    map data has changed.

    The new value is \a connectivityMode.
*/

/*!
\fn void QGeoMapData::updateMapDisplay(const QRectF &target)

    This signal is emitted when the region \a target of the window which
    contains the map needs to be updated.


    If \a target is empty then the entire map will be updated.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapDataPrivate::QGeoMapDataPrivate(QGeoMapData *parent, QGeoMappingManagerEngine *engine)
    : QObject(0),
      engine(engine),
      containerObject(0),
      zoomLevel(-1.0),
      shiftSinceLastInval(0, 0),
      windowSize(0, 0),
      bearing(0.0),
      tilt(0.0),
      blockPropertyChangeSignals(false),
      q_ptr(parent)
{}

QGeoMapDataPrivate::~QGeoMapDataPrivate()
{
    if (containerObject)
        delete containerObject;
    qDeleteAll(overlays);
}

void QGeoMapDataPrivate::addObject(QGeoMapObject *object)
{
    containerObject->addChildObject(object);
    //emit q_ptr->updateMapDisplay();
}

void QGeoMapDataPrivate::removeObject(QGeoMapObject *object)
{
    containerObject->removeChildObject(object);
}

void QGeoMapDataPrivate::clearObjects()
{
    foreach (QGeoMapObject *obj, containerObject->childObjects()) {
        this->removeObject(obj);
        delete obj;
    }
}

QPointF QGeoMapDataPrivate::coordinateToScreenPosition(double lon, double lat) const
{
    QGeoCoordinate c(lon, lat);
    return q_ptr->coordinateToScreenPosition(c);
}

void QGeoMapDataPrivate::emitUpdateMapDisplay(const QRectF &target)
{
    emit q_ptr->updateMapDisplay(target);
}


#include "moc_qgeomapdata.cpp"
#include "moc_qgeomapdata_p.cpp"

QTM_END_NAMESPACE
