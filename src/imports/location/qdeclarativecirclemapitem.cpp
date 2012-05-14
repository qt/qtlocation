/***************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecirclemapitem_p.h"
#include "qdeclarativegeomapquickitem_p.h"
#include "qdeclarativepolygonmapitem_p.h"
#include "qgeocameracapabilities_p.h"
#include "qgeoprojection_p.h"
#include <cmath>
#include <QPen>
#include <QPainter>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapCircle QDeclarativeCircleMapItem
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0

    \brief The MapCircle element displays a geographic circle on a Map.

    The MapCircle element displays a geographic circle on a Map, which
    consists of all points that are within a set distance from one
    central point. Depending on map projection, a geographic circle
    may not always be a perfect circle on the screen: for instance, in
    the Mercator projection, circles become ovoid in shape as they near
    the poles. To display a perfect screen circle around a point, use a
    MapQuickItem containing a relevant QML element instead.

    By default, the circle is displayed as a 1 pixel black border with
    no fill. To change its appearance, use the color, border.color
    and border.width properties.

    Internally, a MapCircle is implemented as a many-sided polygon. To
    calculate the radius points it uses a spherical model of the Earth,
    similar to the atDistanceAndAzimuth method of the QML Coordinate
    element. These two things can occasionally have implications for the
    accuracy of the circle's shape, depending on position and map
    projection.

    \section2 Performance

    MapCircle performance is almost equivalent to that of a MapPolygon with
    125 vertices. There is a small amount of additional overhead with
    respect to calculating the vertices first.

    Like the other map objects, MapCircle is normally drawn without a smooth
    appearance. Setting the opacity property will force the object to be
    blended, which decreases performance considerably depending on the graphics
    hardware in use.

    \section2 Example Usage

    The following snippet shows a map containing a MapCircle, centered at
    the coordinate (-27, 153) with a radius of 5km. The circle is
    filled in green, with a 3 pixel black border.

    \code
    Map {
        MapCircle {
            center: Coordinate { latitude: -27; longitude: 153.0 }
            radius: 5000.0
            color: 'green'
            border.width: 3
        }
    }
    \endcode

    \image ../../../doc/src/images/api-mapcircle.png
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vertex
{
    QVector2D position;
};

static const qreal qgeocoordinate_EARTH_MEAN_RADIUS = 6371.0072;

inline static qreal qgeocoordinate_degToRad(qreal deg)
{
    return deg * M_PI / 180;
}
inline static qreal qgeocoordinate_radToDeg(qreal rad)
{
    return rad * 180 / M_PI;
}

static void calculatePeripheralPoints(QList<QGeoCoordinate>& path, const QGeoCoordinate& center, qreal distance, int steps)
{
    // get angular distance in radians
    distance = distance / (qgeocoordinate_EARTH_MEAN_RADIUS * 1000);

    // We are using horizontal system, we have radius (distance)
    // projected onto Celestial sphere.
    // This way we know the altitude in horizontal system => h = 90 - r;
    // We can now "spin" around with azimuth as a step to get all the points from
    // peripheral of the given circle.
    // To get geographical position we need to change from horizontal system
    // to equatorial system.

    // get location
    qreal lat = qgeocoordinate_degToRad(center.latitude());
    qreal lon = qgeocoordinate_degToRad(center.longitude());

    // precalculate
    qreal cos_h = sin(distance);
    qreal sin_h = cos(distance);
    qreal cos_phi = cos(lat), sin_phi = sin(lat);
    qreal sin_phi_x_sin_h = sin_phi * sin_h;
    qreal cos_phi_x_cos_h = cos_phi * cos_h;
    qreal sin_phi_x_cos_h = sin_phi * cos_h;
    qreal cos_phi_x_sin_h = cos_phi * sin_h;

    for (int i = 0; i < steps; ++i) {

        qreal a = 2 * M_PI * i / steps;
        qreal sin_delta = sin_phi_x_sin_h - cos_phi_x_cos_h * cos(a);
        qreal cos_delta_x_cos_tau = cos_phi_x_sin_h + sin_phi_x_cos_h * cos(a);
        qreal cos_delta_x_sin_tau = -sin(a) * cos_h;
        // get the hour angle (use Cartesian to polar conversion)
        qreal tau = atan2(cos_delta_x_sin_tau, cos_delta_x_cos_tau);
        qreal cos_delta = sqrt(cos_delta_x_sin_tau
                               * cos_delta_x_sin_tau + cos_delta_x_cos_tau
                               * cos_delta_x_cos_tau);
        // get declination ( use Cartesian to polar conversion )
        qreal delta = atan2(sin_delta, cos_delta);
        // get right ascension from tau , use a greenwich star time of 0
        qreal alpha = lon - tau;
        qreal lat2 = qgeocoordinate_radToDeg(delta);
        qreal lon2 = qgeocoordinate_radToDeg(alpha);
        if (lon2 < -180.0) {
            lon2 += 360.0;
        } else if (lon2 > 180.0) {
            lon2 -= 360.0;
        }
        path << QGeoCoordinate(lat2, lon2, center.altitude());
    }
}

QDeclarativeCircleMapItem::QDeclarativeCircleMapItem(QQuickItem *parent):
    QDeclarativeGeoMapItemBase(parent),
    center_(0),
    color_(Qt::transparent),
    radius_(0),
    dirtyMaterial_(true)
{
    setFlag(ItemHasContents, true);
    QObject::connect(&border_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(updateMapItemAssumeDirty()));
    QObject::connect(&border_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(updateMapItemAssumeDirty()));
}

QDeclarativeCircleMapItem::~QDeclarativeCircleMapItem()
{
}

/*!
    \qmlproperty int MapCircle::border.width
    \qmlproperty color MapCircle::border.color

    These properties hold the width and color used to draw the border of the circle.
    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.
    For no line, use a width of 0 or a transparent color.
*/
QDeclarativeMapLineProperties *QDeclarativeCircleMapItem::border()
{
    return &border_;
}

void QDeclarativeCircleMapItem::updateMapItemAssumeDirty()
{
    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
    updateMapItem();
}

void QDeclarativeCircleMapItem::setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
        updateMapItem();
    }
}

/*!
    \qmlproperty Coordinate MapCircle::center

    This property holds the central point about which the circle is defined.

    \sa radius
*/
void QDeclarativeCircleMapItem::setCenter(QDeclarativeCoordinate *center)
{
    if (center_ == center)
        return;
    if (center_)
        center_->disconnect(this);
    center_ = center;

    if (center_) {
        connect(center_, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(updateMapItemAssumeDirty()));
    }

    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
    updateMapItem();
    emit centerChanged(center_);
}

QDeclarativeCoordinate* QDeclarativeCircleMapItem::center()
{
    return center_;
}

/*!
    \qmlproperty color MapCircle::color

    This property holds the fill color of the circle when drawn. For no fill,
    use a transparent color.
*/
void QDeclarativeCircleMapItem::setColor(const QColor &color)
{
    if (color_ == color)
        return;
    color_ = color;
    dirtyMaterial_ = true;
    updateMapItem();
    emit colorChanged(color_);
}

QColor QDeclarativeCircleMapItem::color() const
{
    return color_;
}

/*!
    \qmlproperty qreal MapCircle::radius

    This property holds the radius of the circle, in meters on the ground.

    \sa center
*/
void QDeclarativeCircleMapItem::setRadius(qreal radius)
{
    if (radius_ == radius)
        return;

    radius_ = radius;
    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
    updateMapItem();
    emit radiusChanged(radius);
}

qreal QDeclarativeCircleMapItem::radius() const
{
    return radius_;
}

/*!
  \qmlproperty real MapCircle::opacity

  This property holds the opacity of the item.  Opacity is specified as a
  number between 0 (fully transparent) and 1 (fully opaque).  The default is 1.

  If an item's opacity is set to 0, the item will no longer receive mouse
  events. Similarly, setting the visible property to false stops mouse events.
*/

/*!
    \internal
*/
QSGNode* QDeclarativeCircleMapItem::updateMapItemPaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapPolygonNode *node = static_cast<MapPolygonNode*>(oldNode);

    if (!node)
        node = new MapPolygonNode();

    //TODO: update only material
    if (geometry_.isScreenDirty() || borderGeometry_.isScreenDirty() || dirtyMaterial_) {
        node->update(color_, border_.color(), &geometry_, &borderGeometry_);
        geometry_.markClean();
        borderGeometry_.markClean();
        dirtyMaterial_ = false;
    }
    return node;
}

/*!
    \internal
*/
void QDeclarativeCircleMapItem::updateMapItem()
{
    if (!map() || !center() || !center()->coordinate().isValid())
        return;

    if (geometry_.isSourceDirty()) {
        circlePath_.clear();
        calculatePeripheralPoints(circlePath_, center_->coordinate(),
                                  radius_, 125);
    }

    geometry_.updateSourcePoints(*map(), circlePath_);
    geometry_.updateScreenPoints(*map());

    if (border_.color() != Qt::transparent && border_.width() > 0) {
        QList<QGeoCoordinate> closedPath = circlePath_;
        closedPath << closedPath.first();
        borderGeometry_.updateSourcePoints(*map(), closedPath);
        borderGeometry_.updateScreenPoints(*map(), border_.width());

        QList<QGeoMapItemGeometry*> geoms;
        geoms << &geometry_ << &borderGeometry_;
        QRectF combined = QGeoMapItemGeometry::translateToCommonOrigin(geoms);

        setWidth(combined.width());
        setHeight(combined.height());
    } else {
        borderGeometry_.clear();

        setWidth(geometry_.screenBoundingBox().width());
        setHeight(geometry_.screenBoundingBox().height());
    }

    setPositionOnMap(circlePath_.at(0), geometry_.firstPointOffset());
    update();
}

/*!
    \internal
*/
void QDeclarativeCircleMapItem::afterViewportChanged(const QGeoMapViewportChangeEvent &event)
{
    // if the scene is tilted, we must regenerate our geometry every frame
    if (map()->cameraCapabilities().supportsTilting()
            && (event.cameraData.tilt() > 0.1
                || event.cameraData.tilt() < -0.1)) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
    }

    // if the scene is rolled, we must regen too
    if (map()->cameraCapabilities().supportsRolling()
            && (event.cameraData.roll() > 0.1
                || event.cameraData.roll() < -0.1)) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
    }

    // otherwise, only regen on rotate, resize and zoom
    if (event.bearingChanged || event.mapSizeChanged || event.zoomLevelChanged) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
    }

    geometry_.markScreenDirty();
    borderGeometry_.markScreenDirty();
    updateMapItem();
}

/*!
    \internal
*/
void QDeclarativeCircleMapItem::dragEnded()
{
    QPointF newPoint = QPointF(x(),y()) + QPointF(width(), height()) / 2;
    QGeoCoordinate newCoordinate = map()->screenPositionToCoordinate(newPoint, false);
    if (newCoordinate.isValid()) {
        internalCoordinate_.setCoordinate(newCoordinate);
        setCenter(&internalCoordinate_);
    }
}

/*!
    \internal
*/
bool QDeclarativeCircleMapItem::contains(const QPointF &point)
{
    return (geometry_.contains(point) || borderGeometry_.contains(point));
}

//////////////////////////////////////////////////////////////////////

QT_END_NAMESPACE
