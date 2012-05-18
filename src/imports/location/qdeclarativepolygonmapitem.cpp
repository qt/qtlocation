/****************************************************************************
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

#include "qdeclarativepolygonmapitem_p.h"
#include "qgeocameracapabilities_p.h"
#include "qlocationutils_p.h"
#include <QtGui/private/qtriangulator_p.h>
#include <QtQml/QQmlInfo>
#include <QPainter>
#include <QPainterPath>
#include <qnumeric.h>

/* poly2tri triangulator includes */
#include "../../3rdparty/poly2tri/common/shapes.h"
#include "../../3rdparty/poly2tri/sweep/cdt.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapPolygon QDeclarativePolygonMapItem
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0

    \brief The MapPolygon element displays a polygon on a Map

    The MapPolygon element displays a polygon on a Map, specified in terms of
    an ordered list of \l Coordinate elements. For best appearance and results,
    polygons should be simple (not self-intersecting).

    The Coordinate elements on the path can be changed after being added to
    the Polygon, and these changes will be reflected in the next frame on the
    display. Coordinates can also be added and removed at any time using
    the \l addCoordinate and \l removeCoordinate methods.

    For drawing rectangles with "straight" edges (same latitude across one
    edge, same latitude across the other), the \l MapRectangle element provides
    a simpler, two-point API.

    By default, the polygon is displayed as a 1 pixel black border with no
    fill. To change its appearance, use the \l color, \l border.color and
    \l border.width properties.

    \note Since MapPolygons are geographic items, dragging a MapPolygon
    (through the use of \l MapMouseArea) causes in its vertices to be
    recalculated in the geographic coordinate space. The edges retain the
    same geographic length but remain straight. Apparent stretching of the item
    occurs when dragged to a different latitude.

    \section2 Performance

    MapPolygons have a rendering cost that is O(n) with respect to the number
    of vertices. This means that the per frame cost of having a Polygon on the
    Map grows in direct proportion to the number of points on the Polygon. There
    is an additional triangulation cost (approximately O(n log n)) which is
    currently paid with each frame, but in future may be paid only upon adding
    or removing points.

    Like the other map objects, MapPolygon is normally drawn without a smooth
    appearance. Setting the \l opacity property will force the object to be
    blended, which decreases performance considerably depending on the hardware
    in use.

    \section2 Example Usage

    The following snippet shows a MapPolygon being used to display a triangle,
    with three vertices near Brisbane, Australia. The triangle is filled in
    green, with a 1 pixel black border.

    \code
    Map {
        MapPolygon {
            color: 'green'
            path: [
                Coordinate { latitude: -27; longitude: 153.0 },
                Coordinate { latitude: -27; longitude: 154.1 },
                Coordinate { latitude: -28; longitude: 153.5 }
            ]
        }
    }
    \endcode

    \image ../../../doc/src/images/api-mappolygon.png
*/

struct Vertex
{
    QVector2D position;
};

QGeoMapPolygonGeometry::QGeoMapPolygonGeometry(QObject *parent) :
    QGeoMapItemGeometry(parent), assumeSimple_(false)
{
}

/*!
    \internal
*/
void QGeoMapPolygonGeometry::updateSourcePoints(const QGeoMap &map,
                                                const QList<QGeoCoordinate> &path)
{
    if (!sourceDirty_)
        return;

    // build the actual path
    QPointF origin;
    QPointF lastPoint;
    srcPath_ = QPainterPath();
    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        QPointF point = map.coordinateToScreenPosition(coord, false);

        // We can get NaN if the map isn't set up correctly, or the projection
        // is faulty -- probably best thing to do is abort
        if (!qIsFinite(point.x()) || !qIsFinite(point.y()))
            return;

        if (i == 0) {
            origin = point;
            srcOrigin_ = coord;
            srcPath_.moveTo(point - origin);
            lastPoint = point;
        } else {
            const QPointF diff = (point - lastPoint);
            if (diff.x() * diff.x() + diff.y() * diff.y() >= 3.0) {
                srcPath_.lineTo(point - origin);
                lastPoint = point;
            }
        }
    }
    srcPath_.closeSubpath();

    if (!assumeSimple_)
        srcPath_ = srcPath_.simplified();

    sourceBounds_ = srcPath_.boundingRect();
}

/*!
    \internal
*/
void QGeoMapPolygonGeometry::updateScreenPoints(const QGeoMap &map)
{
    if (!screenDirty_)
        return;

    if (map.width() == 0 || map.height() == 0) {
        clear();
        return;
    }

    QPointF origin = map.coordinateToScreenPosition(srcOrigin_, false);

    // Create the viewport rect in the same coordinate system
    // as the actual points
    QRectF viewport(0, 0, map.width(), map.height());
    viewport.translate(-1 * origin);

    QPainterPath vpPath;
    vpPath.addRect(viewport);

    QPainterPath ppi;
    if (clipToViewport_)
        ppi = srcPath_.intersected(vpPath); // get the clipped version of the path
    else ppi = srcPath_;

    clear();

    // Nothing on the screen
    if (ppi.elementCount() == 0)
        return;

    // translate the path into top-left-centric coordinates
    QRectF bb = ppi.boundingRect();
    ppi.translate(-bb.left(), -bb.top());
    firstPointOffset_ = -1 * bb.topLeft();

    ppi.closeSubpath();

    screenOutline_ = ppi;

    std::vector<p2t::Point*> curPts;
    curPts.reserve(ppi.elementCount());

    for (int i = 0; i < ppi.elementCount(); ++i) {
        const QPainterPath::Element e = ppi.elementAt(i);
        if (e.isMoveTo() || i == ppi.elementCount() - 1
                || (qAbs(e.x - curPts.front()->x) < 0.1
                    && qAbs(e.y - curPts.front()->y) < 0.1)) {
            if (curPts.size() > 2) {
                p2t::CDT *cdt = new p2t::CDT(curPts);
                cdt->Triangulate();
                std::vector<p2t::Triangle*> tris = cdt->GetTriangles();

                screenVertices_.reserve(screenVertices_.size() + tris.size());

                for (int i = 0; i < tris.size(); ++i) {
                    p2t::Triangle *t = tris.at(i);

                    for (int j = 0; j < 3; ++j) {
                        p2t::Point *p = t->GetPoint(j);
                        screenVertices_ << Point(p->x, p->y);
                    }
                }

                delete cdt;
            }
            curPts.clear();
            curPts.reserve(ppi.elementCount() - i);
            curPts.push_back(new p2t::Point(e.x, e.y));
        } else if (e.isLineTo()) {
            curPts.push_back(new p2t::Point(e.x, e.y));
        } else {
            qWarning("Unhandled element type in polygon painterpath");
        }
    }

    if (curPts.size() > 0) {
        for (int i = 0; i < curPts.size(); ++i)
            delete curPts.at(i);
        curPts.clear();
    }

    screenBounds_ = ppi.boundingRect();

}

QDeclarativePolygonMapItem::QDeclarativePolygonMapItem(QQuickItem *parent) :
    QDeclarativeGeoMapItemBase(parent),
    color_(Qt::transparent),
    dirtyMaterial_(true)
{
    setFlag(ItemHasContents, true);
    QObject::connect(&border_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(handleBorderUpdated()));
    QObject::connect(&border_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(handleBorderUpdated()));
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::handleBorderUpdated()
{
    borderGeometry_.markSourceDirty();
    updateMapItem();
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::updateAfterCoordinateChanged()
{
    QDeclarativeCoordinate *coord = qobject_cast<QDeclarativeCoordinate *>(QObject::sender());
    if (coord) {
        // TODO: maybe use a QHash instead of indexOf here?
        int idx = this->coordPath_.indexOf(coord);
        this->path_.replace(idx, coord->coordinate());
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
        this->updateMapItem();
    }
}

QDeclarativePolygonMapItem::~QDeclarativePolygonMapItem()
{
}

/*!
    \qmlproperty int MapPolygon::border.width
    \qmlproperty color MapPolygon::border.color

    These properties hold the width and color used to draw the border of the circle.
    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.
    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeMapLineProperties *QDeclarativePolygonMapItem::border()
{
    return &border_;
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
        updateMapItem();
    }
}

/*!
    \qmlproperty list<Coordinate> MapPolygon::path

    This property holds the ordered list of coordinates which
    define the polygon.

    \sa addCoordinate, removeCoordinate
*/
QQmlListProperty<QDeclarativeCoordinate> QDeclarativePolygonMapItem::declarativePath()
{
    return QQmlListProperty<QDeclarativeCoordinate>(this, 0, path_append, path_count,
                                                    path_at, path_clear);
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::path_append(
        QQmlListProperty<QDeclarativeCoordinate> *property, QDeclarativeCoordinate *coordinate)
{
    QDeclarativePolygonMapItem *item = qobject_cast<QDeclarativePolygonMapItem *>(property->object);
    item->coordPath_.append(coordinate);
    item->path_.append(coordinate->coordinate());

    QObject::connect(coordinate, SIGNAL(coordinateChanged(QGeoCoordinate)),
                     item, SLOT(updateAfterCoordinateChanged()));

    item->geometry_.markSourceDirty();
    item->borderGeometry_.markSourceDirty();
    item->updateMapItem();
    emit item->pathChanged();
}

/*!
    \internal
*/
int QDeclarativePolygonMapItem::path_count(
        QQmlListProperty<QDeclarativeCoordinate> *property)
{
    return qobject_cast<QDeclarativePolygonMapItem *>(property->object)->coordPath_.count();
}

/*!
    \internal
*/
QDeclarativeCoordinate *QDeclarativePolygonMapItem::path_at(
        QQmlListProperty<QDeclarativeCoordinate> *property, int index)
{
    return qobject_cast<QDeclarativePolygonMapItem *>(property->object)->coordPath_.at(index);
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::path_clear(
        QQmlListProperty<QDeclarativeCoordinate> *property)
{
    QDeclarativePolygonMapItem *item = qobject_cast<QDeclarativePolygonMapItem *>(
                property->object);
    qDeleteAll(item->coordPath_);
    item->coordPath_.clear();
    item->path_.clear();
    item->geometry_.markSourceDirty();
    item->borderGeometry_.markSourceDirty();
    item->updateMapItem();
    emit item->pathChanged();
}

/*!
    \qmlmethod MapPolygon::addCoordinate(Coordinate)

    Adds a coordinate to the path.

    \sa removeCoordinate, path
*/

void QDeclarativePolygonMapItem::addCoordinate(QDeclarativeCoordinate *coordinate)
{
    coordPath_.append(coordinate);
    path_.append(coordinate->coordinate());

    QObject::connect(coordinate, SIGNAL(coordinateChanged(QGeoCoordinate)),
                     this, SLOT(updateAfterCoordinateChanged()));

    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
    updateMapItem();
    emit pathChanged();
}

/*!
    \qmlmethod MapPolygon::removeCoordinate(Coordinate)

    Removes a coordinate from the path. If there are multiple instances of the
    same coordinate, the one added last is removed.

    \sa addCoordinate, path

*/

void QDeclarativePolygonMapItem::removeCoordinate(QDeclarativeCoordinate *coordinate)
{
    int index = coordPath_.lastIndexOf(coordinate);

    if (index == -1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolygonMapItem.");
        return;
    }

    if (path_.count() < index + 1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolygonMapItem.");
        return;
    }
    coordPath_.removeAt(index);
    path_.removeAt(index);

    QObject::disconnect(coordinate, SIGNAL(coordinateChanged(QGeoCoordinate)),
                        this, SLOT(updateAfterCoordinateChanged()));

    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
    updateMapItem();
    emit pathChanged();
}

/*!
    \qmlproperty color MapPolygon::color

    This property holds the color used to fill the polygon.

    The default value is transparent.
*/

QColor QDeclarativePolygonMapItem::color() const
{
    return color_;
}

void QDeclarativePolygonMapItem::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    dirtyMaterial_ = true;
    updateMapItem();
    emit colorChanged(color_);
}

/*!
    \internal
*/
QSGNode *QDeclarativePolygonMapItem::updateMapItemPaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    MapPolygonNode *node = static_cast<MapPolygonNode *>(oldNode);

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
void QDeclarativePolygonMapItem::updateMapItem()
{
    if (!map() || path_.count() == 0)
        return;

    geometry_.updateSourcePoints(*map(), path_);
    geometry_.updateScreenPoints(*map());

    if (border_.color() != Qt::transparent && border_.width() > 0) {
        QList<QGeoCoordinate> closedPath = path_;
        closedPath << closedPath.first();
        borderGeometry_.updateSourcePoints(*map(), closedPath);
        borderGeometry_.updateScreenPoints(*map(), border_.width());

        QList<QGeoMapItemGeometry *> geoms;
        geoms << &geometry_ << &borderGeometry_;
        QRectF combined = QGeoMapItemGeometry::translateToCommonOrigin(geoms);

        setWidth(combined.width());
        setHeight(combined.height());
    } else {
        borderGeometry_.clear();
        setWidth(geometry_.sourceBoundingBox().width());
        setHeight(geometry_.sourceBoundingBox().height());
    }
    setPositionOnMap(path_.at(0), -1 * geometry_.sourceBoundingBox().topLeft());
    update();
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::afterViewportChanged(const QGeoMapViewportChangeEvent &event)
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
bool QDeclarativePolygonMapItem::contains(const QPointF &point)
{
    return (geometry_.contains(point) || borderGeometry_.contains(point));
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::dragStarted()
{
    geometry_.markFullScreenDirty();
    borderGeometry_.markFullScreenDirty();
    updateMapItem();
}

/*!
    \internal
*/
void QDeclarativePolygonMapItem::dragEnded()
{
    QPointF newPoint = QPointF(x(),y()) + geometry_.firstPointOffset();
    QGeoCoordinate newCoordinate = map()->screenPositionToCoordinate(newPoint, false);
    if (newCoordinate.isValid()) {
        double firstLongitude = path_.at(0).longitude();
        double firstLatitude = path_.at(0).latitude();
        double minMaxLatitude = firstLatitude;
        // prevent dragging over valid min and max latitudes
        for (int i = 0; i < path_.count(); ++i) {
            double newLatitude = path_.at(i).latitude()
                    + newCoordinate.latitude() - firstLatitude;
            if (!QLocationUtils::isValidLat(newLatitude)) {
                if (qAbs(newLatitude) > qAbs(minMaxLatitude)) {
                    minMaxLatitude = newLatitude;
                }
            }
        }
        // calculate offset needed to re-position the item within map border
        double offsetLatitude = minMaxLatitude - QLocationUtils::clipLat(minMaxLatitude);
        for (int i = 0; i < path_.count(); ++i) {
            QGeoCoordinate coord = path_.at(i);
            // handle dateline crossing
            coord.setLongitude(QLocationUtils::wrapLong(coord.longitude()
                               + newCoordinate.longitude() - firstLongitude));
            coord.setLatitude(coord.latitude()
                              + newCoordinate.latitude() - firstLatitude - offsetLatitude);
            this->coordPath_.at(i)->setCoordinate(coord);
        }
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
        updateMapItem();
        emit pathChanged();
    }
}

//////////////////////////////////////////////////////////////////////

MapPolygonNode::MapPolygonNode() :
    border_(new MapPolylineNode()),
    geometry_(QSGGeometry::defaultAttributes_Point2D(), 0),
    blocked_(true)
{
    geometry_.setDrawingMode(GL_TRIANGLES);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);

    appendChildNode(border_);
}

MapPolygonNode::~MapPolygonNode()
{
}

/*!
    \internal
*/
bool MapPolygonNode::isSubtreeBlocked() const
{
    return blocked_;
}

/*!
    \internal
*/
void MapPolygonNode::update(const QColor &fillColor, const QColor &borderColor,
                            const QGeoMapItemGeometry *fillShape,
                            const QGeoMapItemGeometry *borderShape)
{
    /* Do the border update first */
    border_->update(borderColor, borderShape);

    /* If we have neither fill nor border with valid points, block the whole
     * tree. We can't just block the fill without blocking the border too, so
     * we're a little conservative here (maybe at the expense of rendering
     * accuracy) */
    if (fillShape->size() == 0) {
        if (borderShape->size() == 0) {
            blocked_ = true;
        } else {
            blocked_ = false;
        }
        return;
    } else {
        blocked_ = false;
    }

    QSGGeometry *fill = QSGGeometryNode::geometry();
    fillShape->allocateAndFill(fill);
    markDirty(DirtyGeometry);

    if (fillColor != fill_material_.color()) {
        fill_material_.setColor(fillColor);
        setMaterial(&fill_material_);
        markDirty(DirtyMaterial);
    }
}
QT_END_NAMESPACE
