/***************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecirclemapitem_p.h"
#include "qdeclarativecirclemapitem_p_p.h"
#include "rhi/qdeclarativecirclemapitem_rhi_p.h"

#include <QtCore/QScopedValueRollback>
#include <QPen>
#include <qgeocircle.h>

#include <QtGui/private/qtriangulator_p.h>
#include <QtLocation/private/qgeomap_p.h>
#include <QtPositioning/private/qlocationutils_p.h>
#include <QtPositioning/private/qclipperutils_p.h>

#include <qmath.h>
#include <algorithm>

QT_BEGIN_NAMESPACE

/*!
    \qmltype MapCircle
    \instantiates QDeclarativeCircleMapItem
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.5

    \brief The MapCircle type displays a geographic circle on a Map.

    The MapCircle type displays a geographic circle on a Map, which
    consists of all points that are within a set distance from one
    central point. Depending on map projection, a geographic circle
    may not always be a perfect circle on the screen: for instance, in
    the Mercator projection, circles become ovoid in shape as they near
    the poles. To display a perfect screen circle around a point, use a
    MapQuickItem containing a relevant Qt Quick type instead.

    By default, the circle is displayed as a 1 pixel black border with
    no fill. To change its appearance, use the color, border.color
    and border.width properties.

    Internally, a MapCircle is implemented as a many-sided polygon. To
    calculate the radius points it uses a spherical model of the Earth,
    similar to the atDistanceAndAzimuth method of the \l {coordinate}
    type. These two things can occasionally have implications for the
    accuracy of the circle's shape, depending on position and map
    projection.

    \note Dragging a MapCircle (through the use of \l MouseArea)
    causes new points to be generated at the same distance (in meters)
    from the center. This is in contrast to other map items which store
    their dimensions in terms of latitude and longitude differences between
    vertices.

    \section2 Performance

    MapCircle performance is almost equivalent to that of a MapPolygon with
    the same number of vertices. There is a small amount of additional
    overhead with respect to calculating the vertices first.

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
            center {
                latitude: -27.5
                longitude: 153.0
            }
            radius: 5000.0
            color: 'green'
            border.width: 3
        }
    }
    \endcode

    \image api-mapcircle.png
*/

/*!
    \qmlproperty bool QtLocation::MapCircle::autoFadeIn

    This property holds whether the item automatically fades in when zooming into the map
    starting from very low zoom levels. By default this is \c true.
    Setting this property to \c false causes the map item to always have the opacity specified
    with the \l QtQuick::Item::opacity property, which is 1.0 by default.

    \since 5.14
*/

struct Vertex
{
    QVector2D position;
};

QGeoMapCircleGeometry::QGeoMapCircleGeometry()
{
}

/*!
    \internal
*/
void QGeoMapCircleGeometry::updateScreenPointsInvert(const QList<QDoubleVector2D> &circlePath, const QGeoMap &map)
{
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());
    // Not checking for !screenDirty anymore, as everything is now recalculated.
    clear();
    if (map.viewportWidth() == 0 || map.viewportHeight() == 0 || circlePath.size() < 3) // a circle requires at least 3 points;
        return;

    /*
     * No special case for no tilting as these items are very rare, and usually at most one per map.
     *
     * Approach:
     * 1) subtract the circle from a rectangle filling the whole map, *in wrapped mercator space*
     * 2) clip the resulting geometries against the visible region, *in wrapped mercator space*
     * 3) create a QPainterPath with each of the resulting polygons projected to screen
     * 4) use qTriangulate() to triangulate the painter path
     */

    // 1)
    const double topLati = QLocationUtils::mercatorMaxLatitude();
    const double bottomLati = -(QLocationUtils::mercatorMaxLatitude());
    const double leftLongi = QLocationUtils::mapLeftLongitude(map.cameraData().center().longitude());
    const double rightLongi = QLocationUtils::mapRightLongitude(map.cameraData().center().longitude());

    srcOrigin_ = QGeoCoordinate(topLati,leftLongi);
    const QDoubleVector2D tl = p.geoToWrappedMapProjection(QGeoCoordinate(topLati,leftLongi));
    const QDoubleVector2D tr = p.geoToWrappedMapProjection(QGeoCoordinate(topLati,rightLongi));
    const QDoubleVector2D br = p.geoToWrappedMapProjection(QGeoCoordinate(bottomLati,rightLongi));
    const QDoubleVector2D bl = p.geoToWrappedMapProjection(QGeoCoordinate(bottomLati,leftLongi));

    QList<QDoubleVector2D> fill;
    fill << tl << tr << br << bl;

    QList<QDoubleVector2D> hole;
    for (const QDoubleVector2D &c: circlePath)
        hole << p.wrapMapProjection(c);

    QClipperUtils clipper;
    clipper.addSubjectPath(fill, true);
    clipper.addClipPolygon(hole);
    auto difference = clipper.execute(QClipperUtils::Difference, QClipperUtils::pftEvenOdd,
                                      QClipperUtils::pftEvenOdd);

    // 2)
    QDoubleVector2D lb = p.geoToWrappedMapProjection(srcOrigin_);
    QList<QList<QDoubleVector2D> > clippedPaths;
    const QList<QDoubleVector2D> &visibleRegion = p.visibleGeometry();
    if (visibleRegion.size()) {
        clipper.clearClipper();
        for (const auto &p: difference)
            clipper.addSubjectPath(p, true);
        clipper.addClipPolygon(visibleRegion);
        clippedPaths = clipper.execute(QClipperUtils::Intersection, QClipperUtils::pftEvenOdd,
                                       QClipperUtils::pftEvenOdd);

        // 2.1) update srcOrigin_ with the point with minimum X/Y
        lb = QDoubleVector2D(qInf(), qInf());
        for (const QList<QDoubleVector2D> &path: clippedPaths) {
            for (const QDoubleVector2D &p: path) {
                if (p.x() < lb.x() || (p.x() == lb.x() && p.y() < lb.y())) {
                    lb = p;
                }
            }
        }
        if (qIsInf(lb.x()))
            return;

        // Prevent the conversion to and from clipper from introducing negative offsets which
        // in turn will make the geometry wrap around.
        lb.setX(qMax(tl.x(), lb.x()));
        srcOrigin_ = p.mapProjectionToGeo(p.unwrapMapProjection(lb));
    } else {
        clippedPaths = difference;
    }

    //3)
    const QDoubleVector2D origin = p.wrappedMapProjectionToItemPosition(lb);

    QPainterPath ppi;
    for (const QList<QDoubleVector2D> &path: clippedPaths) {
        QDoubleVector2D lastAddedPoint;
        for (qsizetype i = 0; i < path.size(); ++i) {
            QDoubleVector2D point = p.wrappedMapProjectionToItemPosition(path.at(i));
            //point = point - origin; // Do this using ppi.translate()

            if (i == 0) {
                ppi.moveTo(point.toPointF());
                lastAddedPoint = point;
            } else if ((point - lastAddedPoint).manhattanLength() > 3 || i == path.size() - 1) {
                ppi.lineTo(point.toPointF());
                lastAddedPoint = point;
            }
        }
        ppi.closeSubpath();
    }
    ppi.translate(-1 * origin.toPointF());

    QTriangleSet ts = qTriangulate(ppi);
    qreal *vx = ts.vertices.data();

    screenIndices_.reserve(ts.indices.size());
    screenVertices_.reserve(ts.vertices.size());

    if (ts.indices.type() == QVertexIndexVector::UnsignedInt) {
        const quint32 *ix = reinterpret_cast<const quint32 *>(ts.indices.data());
        for (qsizetype i = 0; i < (ts.indices.size()/3*3); ++i)
            screenIndices_ << ix[i];
    } else {
        const quint16 *ix = reinterpret_cast<const quint16 *>(ts.indices.data());
        for (qsizetype i = 0; i < (ts.indices.size()/3*3); ++i)
            screenIndices_ << ix[i];
    }
    for (qsizetype i = 0; i < (ts.vertices.size()/2*2); i += 2)
        screenVertices_ << QPointF(vx[i], vx[i + 1]);

    screenBounds_ = ppi.boundingRect();
    sourceBounds_ = screenBounds_;
}

struct CircleBackendSelector
{
    CircleBackendSelector()
    {
        backend = (qgetenv("QTLOCATION_OPENGL_ITEMS").toInt()) ? QDeclarativeCircleMapItem::OpenGL : QDeclarativeCircleMapItem::Software;
    }
    QDeclarativeCircleMapItem::Backend backend = QDeclarativeCircleMapItem::Software;
};

Q_GLOBAL_STATIC(CircleBackendSelector, mapCircleBackendSelector)

QDeclarativeCircleMapItem::QDeclarativeCircleMapItem(QQuickItem *parent)
:   QDeclarativeGeoMapItemBase(parent), m_border(this), m_color(Qt::transparent), m_dirtyMaterial(true),
    m_updatingGeometry(false)
  , m_d(new QDeclarativeCircleMapItemPrivateCPU(*this))
{
    // ToDo: handle envvar, and switch implementation.
    m_itemType = QGeoMap::MapCircle;
    setFlag(ItemHasContents, true);
    QObject::connect(&m_border, &QDeclarativeMapLineProperties::colorChanged,
                     this, &QDeclarativeCircleMapItem::onLinePropertiesChanged);
    QObject::connect(&m_border, &QDeclarativeMapLineProperties::widthChanged,
                     this, &QDeclarativeCircleMapItem::onLinePropertiesChanged);

    // assume that circles are not self-intersecting
    // to speed up processing
    // FIXME: unfortunately they self-intersect at the poles due to current drawing method
    // so the line is commented out until fixed
    //geometry_.setAssumeSimple(true);
    setBackend(mapCircleBackendSelector->backend);
}

QDeclarativeCircleMapItem::~QDeclarativeCircleMapItem()
{
}

/*!
    \qmlpropertygroup Location::MapCircle::border
    \qmlproperty int MapCircle::border.width
    \qmlproperty color MapCircle::border.color

    This property is part of the border group property.
    The border property holds the width and color used to draw the border of the circle.
    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.
    For no line, use a width of 0 or a transparent color.
*/
QDeclarativeMapLineProperties *QDeclarativeCircleMapItem::border()
{
    return &m_border;
}

void QDeclarativeCircleMapItem::markSourceDirtyAndUpdate()
{
    m_d->markSourceDirtyAndUpdate();
}

void QDeclarativeCircleMapItem::onLinePropertiesChanged()
{
    m_d->onLinePropertiesChanged();
}

void QDeclarativeCircleMapItem::setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map)
        m_d->onMapSet();
}

/*!
    \qmlproperty coordinate MapCircle::center

    This property holds the central point about which the circle is defined.

    \sa radius
*/
void QDeclarativeCircleMapItem::setCenter(const QGeoCoordinate &center)
{
    if (m_circle.center() == center)
        return;

    possiblySwitchBackend(m_circle.center(), m_circle.radius(), center, m_circle.radius());
    m_circle.setCenter(center);
    m_d->onGeoGeometryChanged();
    emit centerChanged(center);
}

QGeoCoordinate QDeclarativeCircleMapItem::center()
{
    return m_circle.center();
}

/*!
    \qmlproperty color MapCircle::color

    This property holds the fill color of the circle when drawn. For no fill,
    use a transparent color.
*/
void QDeclarativeCircleMapItem::setColor(const QColor &color)
{
    if (m_color == color)
        return;
    m_color = color;
    m_dirtyMaterial = true;
    update();
    emit colorChanged(m_color);
}

QColor QDeclarativeCircleMapItem::color() const
{
    return m_color;
}

/*!
    \qmlproperty real MapCircle::radius

    This property holds the radius of the circle, in meters on the ground.

    \sa center
*/
void QDeclarativeCircleMapItem::setRadius(qreal radius)
{
    if (m_circle.radius() == radius)
        return;

    possiblySwitchBackend(m_circle.center(), m_circle.radius(), m_circle.center(), radius);
    m_circle.setRadius(radius);
    m_d->onGeoGeometryChanged();
    emit radiusChanged(radius);
}

qreal QDeclarativeCircleMapItem::radius() const
{
    return m_circle.radius();
}

/*!
  \qmlproperty real MapCircle::opacity

  This property holds the opacity of the item.  Opacity is specified as a
  number between 0 (fully transparent) and 1 (fully opaque).  The default is 1.

  An item with 0 opacity will still receive mouse events. To stop mouse events, set the
  visible property of the item to false.
*/

/*!
    \internal
*/
QSGNode *QDeclarativeCircleMapItem::updateMapItemPaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    return m_d->updateMapItemPaintNode(oldNode, data);
}

/*!
    \internal
*/
void QDeclarativeCircleMapItem::updatePolish()
{
    if (!map() || map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
        return;
    m_d->updatePolish();
}

/*!
    \internal

    The OpenGL backend doesn't do circles crossing poles yet.
    So if that backend is selected and the circle crosses the poles, use the CPU backend instead.
*/
void QDeclarativeCircleMapItem::possiblySwitchBackend(const QGeoCoordinate &oldCenter, qreal oldRadius, const QGeoCoordinate &newCenter, qreal newRadius)
{
    if (m_backend != QDeclarativeCircleMapItem::OpenGL)
        return;

    // if old does not cross and new crosses, move to CPU.
    if (!QDeclarativeCircleMapItemPrivate::crossEarthPole(oldCenter, oldRadius)
            && !QDeclarativeCircleMapItemPrivate::crossEarthPole(newCenter, newRadius)) {
        std::unique_ptr<QDeclarativeCircleMapItemPrivate> d(static_cast<QDeclarativeCircleMapItemPrivate *>(new QDeclarativeCircleMapItemPrivateCPU(*this)));
        std::swap(m_d, d);
    } else if (QDeclarativeCircleMapItemPrivate::crossEarthPole(oldCenter, oldRadius)
               && !QDeclarativeCircleMapItemPrivate::crossEarthPole(newCenter, newRadius)) { // else if old crosses and new does not cross, move back to OpenGL
        std::unique_ptr<QDeclarativeCircleMapItemPrivate> d(static_cast<QDeclarativeCircleMapItemPrivate *>(new QDeclarativeCircleMapItemPrivateOpenGL(*this)));
        std::swap(m_d, d);
    }
}

/*!
    \internal
*/
void QDeclarativeCircleMapItem::afterViewportChanged(const QGeoMapViewportChangeEvent &event)
{
    if (event.mapSize.isEmpty())
        return;

    m_d->afterViewportChanged();
}

/*!
    \internal
*/
bool QDeclarativeCircleMapItem::contains(const QPointF &point) const
{
    return m_d->contains(point);
    //
}

const QGeoShape &QDeclarativeCircleMapItem::geoShape() const
{
    return m_circle;
}

void QDeclarativeCircleMapItem::setGeoShape(const QGeoShape &shape)
{
    if (shape == m_circle)
        return;

    const QGeoCircle circle(shape); // if shape isn't a circle, circle will be created as a default-constructed circle
    const bool centerHasChanged = circle.center() != m_circle.center();
    const bool radiusHasChanged = circle.radius() != m_circle.radius();
    possiblySwitchBackend(m_circle.center(), m_circle.radius(), circle.center(), circle.radius());
    m_circle = circle;

    m_d->onGeoGeometryChanged();
    if (centerHasChanged)
        emit centerChanged(m_circle.center());
    if (radiusHasChanged)
        emit radiusChanged(m_circle.radius());
}

/*!
    \internal
    \qmlproperty MapCircle.Backend QtLocation::MapCircle::backend

    This property holds which backend is in use to render the map item.
    Valid values are \b MapCircle.Software and \b{MapCircle.OpenGL}.
    The default value is \b{MapCircle.Software}.

    \note \b{The release of this API with Qt 5.15 is a Technology Preview}.
    Ideally, as the OpenGL backends for map items mature, there will be
    no more need to also offer the legacy software-projection backend.
    So this property will likely disappear at some later point.
    To select OpenGL-accelerated item backends without using this property,
    it is also possible to set the environment variable \b QTLOCATION_OPENGL_ITEMS
    to \b{1}.
    Also note that all current OpenGL backends won't work as expected when enabling
    layers on the individual item, or when running on OpenGL core profiles greater than 2.x.

    \since 5.15
*/

QDeclarativeCircleMapItem::Backend QDeclarativeCircleMapItem::backend() const
{
    return m_backend;
}

void QDeclarativeCircleMapItem::setBackend(QDeclarativeCircleMapItem::Backend b)
{
    if (b == m_backend)
        return;
    m_backend = b;
    std::unique_ptr<QDeclarativeCircleMapItemPrivate> d(
            (m_backend == Software) ? static_cast<QDeclarativeCircleMapItemPrivate *>(
                    new QDeclarativeCircleMapItemPrivateCPU(*this))
                                    : static_cast<QDeclarativeCircleMapItemPrivate *>(
                                            new QDeclarativeCircleMapItemPrivateOpenGL(*this)));
    std::swap(m_d, d);
    m_d->onGeoGeometryChanged();
    emit backendChanged();
}

/*!
    \internal
*/
void QDeclarativeCircleMapItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (!map() || !m_circle.isValid() || m_updatingGeometry || newGeometry == oldGeometry) {
        QDeclarativeGeoMapItemBase::geometryChange(newGeometry, oldGeometry);
        return;
    }

    QDoubleVector2D newPoint = QDoubleVector2D(x(),y()) + QDoubleVector2D(width(), height()) * 0.5;
    QGeoCoordinate newCoordinate = map()->geoProjection().itemPositionToCoordinate(newPoint, false);
    if (newCoordinate.isValid())
        setCenter(newCoordinate); // ToDo: this is incorrect. setting such center might yield to another geometry changed.

    // Not calling QDeclarativeGeoMapItemBase::geometryChange() as it will be called from a nested
    // call to this function.
}

QDeclarativeCircleMapItemPrivate::~QDeclarativeCircleMapItemPrivate() {}

QDeclarativeCircleMapItemPrivateCPU::~QDeclarativeCircleMapItemPrivateCPU() {}

bool QDeclarativeCircleMapItemPrivate::preserveCircleGeometry (QList<QDoubleVector2D> &path,
                                    const QGeoCoordinate &center, qreal distance, const QGeoProjectionWebMercator &p)
{
    // if circle crosses north/south pole, then don't preserve circular shape,
    if ( crossEarthPole(center, distance)) {
        updateCirclePathForRendering(path, center, distance, p);
        return false;
    }
    return true;
}

/*
 * A workaround for circle path to be drawn correctly using a polygon geometry
 * This method generates a polygon like
 *  _____________
 *  |           |
 *   \         /
 *    |       |
 *   /         \
 *  |           |
 *  -------------
 *
 * or a polygon like
 *
 *  ______________
 *  |    ____    |
 *   \__/    \__/
 */
void QDeclarativeCircleMapItemPrivate::updateCirclePathForRendering(QList<QDoubleVector2D> &path,
                                                             const QGeoCoordinate &center,
                                                             qreal distance, const QGeoProjectionWebMercator &p)
{
    const qreal poleLat = 90;
    const qreal distanceToNorthPole = center.distanceTo(QGeoCoordinate(poleLat, 0));
    const qreal distanceToSouthPole = center.distanceTo(QGeoCoordinate(-poleLat, 0));
    bool crossNorthPole = distanceToNorthPole < distance;
    bool crossSouthPole = distanceToSouthPole < distance;

    QList<qsizetype> wrapPathIndex;
    QDoubleVector2D prev = p.wrapMapProjection(path.at(0));

    for (qsizetype i = 1; i <= path.count(); ++i) {
        const auto index = i % path.count();
        const QDoubleVector2D point = p.wrapMapProjection(path.at(index));
        double diff = qAbs(point.x() - prev.x());
        if (diff > 0.5) {
            continue;
        }
    }

    // find the points in path where wrapping occurs
    for (qsizetype i = 1; i <= path.count(); ++i) {
        const auto index = i % path.count();
        const QDoubleVector2D point = p.wrapMapProjection(path.at(index));
        if ((qAbs(point.x() - prev.x())) >= 0.5) {
            wrapPathIndex << index;
            if (wrapPathIndex.size() == 2 || !(crossNorthPole && crossSouthPole))
                break;
        }
        prev = point;
    }
    // insert two additional coords at top/bottom map corners of the map for shape
    // to be drawn correctly
    if (wrapPathIndex.size() > 0) {
        qreal newPoleLat = 0; // 90 latitude
        QDoubleVector2D wrapCoord = path.at(wrapPathIndex[0]);
        if (wrapPathIndex.size() == 2) {
            QDoubleVector2D wrapCoord2 = path.at(wrapPathIndex[1]);
            if (wrapCoord2.y() < wrapCoord.y())
                newPoleLat = 1; // -90 latitude
        } else if (center.latitude() < 0) {
            newPoleLat = 1; // -90 latitude
        }
        for (qsizetype i = 0; i < wrapPathIndex.size(); ++i) {
            const qsizetype index = wrapPathIndex[i] == 0 ? 0 : wrapPathIndex[i] + i*2;
            const qsizetype prevIndex = (index - 1) < 0 ? (path.count() - 1) : index - 1;
            QDoubleVector2D coord0 = path.at(prevIndex);
            QDoubleVector2D coord1 = path.at(index);
            coord0.setY(newPoleLat);
            coord1.setY(newPoleLat);
            path.insert(index ,coord1);
            path.insert(index, coord0);
            newPoleLat = 1.0 - newPoleLat;
        }
    }
}

bool QDeclarativeCircleMapItemPrivate::crossEarthPole(const QGeoCoordinate &center, qreal distance)
{
    qreal poleLat = 90;
    QGeoCoordinate northPole = QGeoCoordinate(poleLat, center.longitude());
    QGeoCoordinate southPole = QGeoCoordinate(-poleLat, center.longitude());
    // approximate using great circle distance
    qreal distanceToNorthPole = center.distanceTo(northPole);
    qreal distanceToSouthPole = center.distanceTo(southPole);
    if (distanceToNorthPole < distance || distanceToSouthPole < distance)
        return true;
    return false;
}

void QDeclarativeCircleMapItemPrivate::calculatePeripheralPoints(QList<QGeoCoordinate> &path,
                                      const QGeoCoordinate &center,
                                      qreal distance,
                                      int steps,
                                      QGeoCoordinate &leftBound)
{
    // Calculate points based on great-circle distance
    // Calculation is the same as GeoCoordinate's atDistanceAndAzimuth function
    // but tweaked here for computing multiple points

    // pre-calculations
    steps = qMax(steps, 3);
    qreal centerLon = center.longitude();
    qreal minLon = centerLon;
    qreal latRad = QLocationUtils::radians(center.latitude());
    qreal lonRad = QLocationUtils::radians(centerLon);
    qreal cosLatRad = std::cos(latRad);
    qreal sinLatRad = std::sin(latRad);
    qreal ratio = (distance / QLocationUtils::earthMeanRadius());
    qreal cosRatio = std::cos(ratio);
    qreal sinRatio = std::sin(ratio);
    qreal sinLatRad_x_cosRatio = sinLatRad * cosRatio;
    qreal cosLatRad_x_sinRatio = cosLatRad * sinRatio;
    int idx = 0;
    for (int i = 0; i < steps; ++i) {
        const qreal azimuthRad = 2 * M_PI * i / steps;
        const qreal resultLatRad = std::asin(sinLatRad_x_cosRatio
                                 + cosLatRad_x_sinRatio * std::cos(azimuthRad));
        const qreal resultLonRad = lonRad + std::atan2(std::sin(azimuthRad) * cosLatRad_x_sinRatio,
                                   cosRatio - sinLatRad * std::sin(resultLatRad));
        const qreal lat2 = QLocationUtils::degrees(resultLatRad);
        qreal lon2 = QLocationUtils::wrapLong(QLocationUtils::degrees(resultLonRad));

        path << QGeoCoordinate(lat2, lon2, center.altitude());
        // Consider only points in the left half of the circle for the left bound.
        if (azimuthRad > M_PI) {
            if (lon2 > centerLon) // if point and center are on different hemispheres
                lon2 -= 360;
            if (lon2 < minLon) {
                minLon = lon2;
                idx = i;
            }
        }
    }
    leftBound = path.at(idx);
}

//////////////////////////////////////////////////////////////////////

void QDeclarativeCircleMapItemPrivateCPU::updatePolish()
{
    if (!m_circle.m_circle.isValid()) {
        m_geometry.clear();
        m_borderGeometry.clear();
        m_circle.setWidth(0);
        m_circle.setHeight(0);
        return;
    }

    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_circle.map()->geoProjection());
    QScopedValueRollback<bool> rollback(m_circle.m_updatingGeometry);
    m_circle.m_updatingGeometry = true;

    QList<QDoubleVector2D> circlePath = m_circlePath;

    int pathCount = circlePath.size();
    bool preserve = preserveCircleGeometry(circlePath, m_circle.m_circle.center(),
                                           m_circle.m_circle.radius(), p);
    // using leftBound_ instead of the analytically calculated
    // circle_.boundingGeoRectangle().topLeft());
    // to fix QTBUG-62154
    m_geometry.setPreserveGeometry(true, m_leftBound); // to set the geoLeftBound_
    m_geometry.setPreserveGeometry(preserve, m_leftBound);

    bool invertedCircle = false;
    if (crossEarthPole(m_circle.m_circle.center(), m_circle.m_circle.radius())
        && circlePath.size() == pathCount) {
        // invert fill area for really huge circles
        m_geometry.updateScreenPointsInvert(circlePath, *m_circle.map());
        invertedCircle = true;
    } else {
        m_geometry.updateSourcePoints(*m_circle.map(), circlePath);
        m_geometry.updateScreenPoints(*m_circle.map(), m_circle.m_border.width());
    }

    m_borderGeometry.clear();
    QList<QGeoMapItemGeometry *> geoms;
    geoms << &m_geometry;

    if (m_circle.m_border.color() != Qt::transparent && m_circle.m_border.width() > 0) {
        QList<QDoubleVector2D> closedPath = circlePath;
        closedPath << closedPath.first();

        if (invertedCircle) {
            closedPath = m_circlePath;
            closedPath << closedPath.first();
            std::reverse(closedPath.begin(), closedPath.end());
        }

        m_borderGeometry.setPreserveGeometry(true, m_leftBound);
        m_borderGeometry.setPreserveGeometry(preserve, m_leftBound);

        // Use srcOrigin_ from fill geometry after clipping to ensure that
        // translateToCommonOrigin won't fail.
        const QGeoCoordinate &geometryOrigin = m_geometry.origin();

        m_borderGeometry.srcPoints_.clear();
        m_borderGeometry.srcPointTypes_.clear();

        QDoubleVector2D borderLeftBoundWrapped;
        QList<QList<QDoubleVector2D >> clippedPaths = m_borderGeometry.clipPath(*m_circle.map(),
                                                        closedPath, borderLeftBoundWrapped);
        if (clippedPaths.size()) {
            borderLeftBoundWrapped = p.geoToWrappedMapProjection(geometryOrigin);
            m_borderGeometry.pathToScreen(*m_circle.map(), clippedPaths, borderLeftBoundWrapped);
            m_borderGeometry.updateScreenPoints(*m_circle.map(), m_circle.m_border.width());
            geoms << &m_borderGeometry;
        } else {
            m_borderGeometry.clear();
        }
    }

    QRectF combined = QGeoMapItemGeometry::translateToCommonOrigin(geoms);

    if (invertedCircle || !preserve) {
        m_circle.setWidth(combined.width());
        m_circle.setHeight(combined.height());
    } else {
        m_circle.setWidth(combined.width() + 2 * m_circle.m_border.width()); // ToDo: Fix this!
        m_circle.setHeight(combined.height() + 2 * m_circle.m_border.width());
    }

    // No offsetting here, even in normal case, because first point offset is already translated
    m_circle.setPositionOnMap(m_geometry.origin(), m_geometry.firstPointOffset());
}

QSGNode *QDeclarativeCircleMapItemPrivateCPU::updateMapItemPaintNode(QSGNode *oldNode,
                                                            QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    if (!m_node || !oldNode) { // Apparently the QSG might delete the nodes if they become invisible
        m_node = new MapPolygonNode();
        if (oldNode) {
            delete oldNode;
            oldNode = nullptr;
        }
    } else {
        m_node = static_cast<MapPolygonNode *>(oldNode);
    }

    //TODO: update only material
    if (m_geometry.isScreenDirty() || m_borderGeometry.isScreenDirty() || m_circle.m_dirtyMaterial) {
        m_node->update(m_circle.m_color, m_circle.m_border.color(), &m_geometry, &m_borderGeometry);
        m_geometry.setPreserveGeometry(false);
        m_borderGeometry.setPreserveGeometry(false);
        m_geometry.markClean();
        m_borderGeometry.markClean();
        m_circle.m_dirtyMaterial = false;
    }
    return m_node;
}
bool QDeclarativeCircleMapItemPrivateCPU::contains(const QPointF &point) const
{
    return (m_geometry.contains(point) || m_borderGeometry.contains(point));
}

QT_END_NAMESPACE
