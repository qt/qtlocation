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
#include <QtGui/private/qtriangulator_p.h>
#include <QDeclarativeInfo>
#include <QPainter>
#include <QPainterPath>
#include <qnumeric.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapPolygon QDeclarativePolygonMapItem
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

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
    QGeoMapItemGeometry(parent)
{
}

void QGeoMapPolygonGeometry::updateSourcePoints(const QGeoMap &map,
                                                const QList<QGeoCoordinate> &path)
{
    if (!sourceDirty_)
        return;

    // build the actual path
    QPointF origin;
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
        } else {
            srcPath_.lineTo(point - origin);
        }
    }
    srcPath_.closeSubpath();

    sourceBounds_ = srcPath_.boundingRect();
}

void QGeoMapPolygonGeometry::updateScreenPoints(const QGeoMap &map)
{
    if (!screenDirty_)
        return;

    QPointF origin = map.coordinateToScreenPosition(srcOrigin_, false);

    // Create the viewport rect in the same coordinate system
    // as the actual points
    QRectF viewport(0, 0, map.width(), map.height());
    viewport.translate(-1 * origin);

    QPainterPath vpPath;
    vpPath.addRect(viewport);

    // get the clipped version of the path
    QPainterPath ppi = srcPath_.intersected(vpPath);

    // Nothing on the screen
    if (ppi.elementCount() == 0)
        return;

    // translate the path into top-left-centric coordinates
    QRectF bb = ppi.boundingRect();
    ppi.translate(-bb.left(), -bb.top());
    firstPointOffset_ = -1 * bb.topLeft();

    screenOutline_ = ppi;

    QTriangleSet ts = qTriangulate(ppi);
    qreal *vx = ts.vertices.data();

    screenTriangles_.clear();
    screenTriangles_.reserve(ts.indices.size());

    if (ts.indices.type() == QVertexIndexVector::UnsignedInt) {
        const quint32 *tx = reinterpret_cast<const quint32*>(ts.indices.data());
        for (int i = 0; i < (ts.indices.size()/3*3); i++) {
            screenTriangles_ << vx[tx[i]*2] << vx[tx[i]*2+1];
        }
    } else {
        const quint16 *tx = reinterpret_cast<const quint16*>(ts.indices.data());
        for (int i = 0; i < (ts.indices.size()/3*3); i++) {
            screenTriangles_ << vx[tx[i]*2] << vx[tx[i]*2+1];
        }
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

void QDeclarativePolygonMapItem::handleBorderUpdated()
{
    borderGeometry_.markSourceDirty();
    updateMapItem();
}

void QDeclarativePolygonMapItem::updateAfterCoordinateChanged()
{
    QDeclarativeCoordinate *coord = qobject_cast<QDeclarativeCoordinate*>(QObject::sender());
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

void QDeclarativePolygonMapItem::setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
        updateMapItem();
    }
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativePolygonMapItem::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this, 0, path_append, path_count,
                                                            path_at, path_clear);
}

/*!
    \qmlproperty list<Coordinate> MapPolygon::path

    This property holds the ordered list of coordinates which
    define the polygon.

    \sa addCoordinate removeCoordinate
*/

void QDeclarativePolygonMapItem::path_append(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property, QDeclarativeCoordinate *coordinate)
{
    QDeclarativePolygonMapItem* item = qobject_cast<QDeclarativePolygonMapItem*>(property->object);
    item->coordPath_.append(coordinate);
    item->path_.append(coordinate->coordinate());

    QObject::connect(coordinate, SIGNAL(coordinateChanged(QGeoCoordinate)),
                     item, SLOT(updateAfterCoordinateChanged()));

    item->geometry_.markSourceDirty();
    item->borderGeometry_.markSourceDirty();
    item->updateMapItem();
    emit item->pathChanged();
}

int QDeclarativePolygonMapItem::path_count(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    return qobject_cast<QDeclarativePolygonMapItem*>(property->object)->coordPath_.count();
}

QDeclarativeCoordinate* QDeclarativePolygonMapItem::path_at(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property, int index)
{
    return qobject_cast<QDeclarativePolygonMapItem*>(property->object)->coordPath_.at(index);
}

void QDeclarativePolygonMapItem::path_clear(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    QDeclarativePolygonMapItem* item = qobject_cast<QDeclarativePolygonMapItem*>(
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

    Adds coordinate to the path.

    \sa removeCoordinate path
*/

void QDeclarativePolygonMapItem::addCoordinate(QDeclarativeCoordinate* coordinate)
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

    Remove coordinate from the path. If there are multiple instances of the
    same coordinate, the one added last is removed.

    \sa addCoordinate path

*/

void QDeclarativePolygonMapItem::removeCoordinate(QDeclarativeCoordinate* coordinate)
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

QSGNode* QDeclarativePolygonMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
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

    setPositionOnMap(path_.at(0), geometry_.firstPointOffset());
    update();
}

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

bool QDeclarativePolygonMapItem::contains(QPointF point)
{
    return (geometry_.contains(point) || borderGeometry_.contains(point));
}

//////////////////////////////////////////////////////////////////////

MapPolygonNode::MapPolygonNode() :
    border_(new MapPolylineNode()),
    geometry_(QSGGeometry::defaultAttributes_Point2D(), 0)
{
    geometry_.setDrawingMode(GL_TRIANGLES);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);

    appendChildNode(border_);
}

MapPolygonNode::~MapPolygonNode()
{
}

void MapPolygonNode::update(const QColor& fillColor, const QColor& borderColor,
                            const QGeoMapItemGeometry *fillShape,
                            const QGeoMapItemGeometry *borderShape)
{
    if (fillShape->size() == 0)
        return;

    QSGGeometry *fill = QSGGeometryNode::geometry();

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(fillShape->size());

    Vertex *vertices = (Vertex *)fill->vertexData();
    for (int i = 0; i < fillShape->size(); ++i)
        vertices[fillVertexCount++].position = fillShape->vertex(i);

    Q_ASSERT(fillVertexCount == fill->vertexCount());
    markDirty(DirtyGeometry);

    if (fillColor != fill_material_.color()) {
        fill_material_.setColor(fillColor);
        setMaterial(&fill_material_);
    }
    border_->update(borderColor, borderShape);
}

QT_END_NAMESPACE
