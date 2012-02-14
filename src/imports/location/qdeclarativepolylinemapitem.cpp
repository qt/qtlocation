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

#include "qdeclarativepolylinemapitem_p.h"
#include <QDeclarativeInfo>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <qnumeric.h>

#include <QtGui/private/qvectorpath_p.h>
#include <QtGui/private/qopengltriangulatingstroker_p.h>
#include <QtGui/private/qtriangulator_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapPolyline QDeclarativePolylineMapItem
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

    \brief The MapPolyline element displays a polyline on a map.

    The MapPolyline element displays a polyline on a map, specified in terms
    of an ordered list of \l Coordinate elements. The Coordinate elements
    on the path can be changed after being added to the Polyline, and these
    changes will be reflected in the next frame on the display. Coordinates can
    also be added and removed at any time using the \l addCoordinate and
    \l removeCoordinate methods.

    By default, the polyline is displayed as a 1-pixel thick black line. This
    can be changed using the \l line.width and \l line.color properties.

    \section2 Performance

    MapPolylines have a rendering cost that is O(n) with respect to the number
    of vertices. This means that the per frame cost of having a Polyline on
    the Map grows in direct proportion to the number of points in the Polyline.

    Like the other map objects, MapPolyline is normally drawn without a smooth
    appearance. Setting the \l opacity property will force the object to be
    blended, which decreases performance considerably depending on the hardware
    in use.

    \bold{NOTE:} MapPolylines are implemented using the OpenGL GL_LINES
    primitive. There have been occasional reports of issues and rendering
    inconsistencies on some (particularly quite old) platforms. No workaround
    is yet available for these issues.

    \section2 Example Usage

    The following snippet shows a MapPolyline with 4 points, making a shape
    like the top part of a "question mark" (?), near Brisbane, Australia.
    The line drawn is 3 pixels in width and green in color.

    \code
    Map {
        MapPolyline {
            line.width: 3
            line.color: 'green'
            path: [
                Coordinate { latitude: -27; longitude: 153.0 },
                Coordinate { latitude: -27; longitude: 154.1 },
                Coordinate { latitude: -28; longitude: 153.5 },
                Coordinate { latitude: -29; longitude: 153.5 }
            ]
        }
    }
    \endcode

    \image ../../../doc/src/images/api-mappolyline.png
*/

QDeclarativeMapLineProperties::QDeclarativeMapLineProperties(QObject *parent) :
    QObject(parent),
    width_(1.0),
    color_(Qt::black)
{
}

QColor QDeclarativeMapLineProperties::color() const
{
    return color_;
}

void QDeclarativeMapLineProperties::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    emit colorChanged(color_);
}

qreal QDeclarativeMapLineProperties::width() const
{
    return width_;
}

void QDeclarativeMapLineProperties::setWidth(qreal width)
{
    if (width_ == width)
        return;

    width_ = width;
    emit widthChanged(width_);
}

struct Vertex
{
    QVector2D position;
};

QGeoMapPolylineGeometry::QGeoMapPolylineGeometry(QObject *parent) :
    QGeoMapItemGeometry(parent)
{
}

void QGeoMapPolylineGeometry::updateSourcePoints(const QGeoMap &map,
                                                 const QList<QGeoCoordinate> &path)
{
    qreal minX, minY, maxX, maxY;

    if (!sourceDirty_)
        return;

    // clear the old data and reserve enough memory
    srcPoints_.clear();
    srcPoints_.reserve(path.size()*2);
    srcPointTypes_.clear();
    srcPointTypes_.reserve(path.size());

    QPointF origin, lastPoint, lastAddedPoint;
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
            srcOrigin_ = coord;
            origin = point;
            point = QPointF(0,0);

            minX = (maxX = point.x());
            minY = (maxY = point.y());

            srcPoints_ << point.x() << point.y();
            srcPointTypes_ << QPainterPath::MoveToElement;
            lastAddedPoint = point;

        } else {
            point -= origin;

            minX = qMin(point.x(), minX);
            minY = qMin(point.y(), minY);
            maxX = qMax(point.x(), maxX);
            maxY = qMax(point.y(), maxY);

            if ((point - lastAddedPoint).manhattanLength() > 3 ||
                    i == path.size() - 1) {
                srcPoints_ << point.x() << point.y();
                srcPointTypes_ << QPainterPath::LineToElement;
                lastAddedPoint = point;
            }
        }

        lastPoint = point;
    }

    sourceBounds_ = QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

////////////////////////////////////////////////////////////////////////////
/* Polyline clip */

enum ClipPointType {
    InsidePoint  = 0x00,
    LeftPoint    = 0x01,
    RightPoint   = 0x02,
    BottomPoint  = 0x04,
    TopPoint     = 0x08
};

static inline int clipPointType(qreal x, qreal y, const QRectF &rect)
{
    int type = InsidePoint;
    if (x < rect.left())
        type |= LeftPoint;
    else if (x > rect.right())
        type |= RightPoint;
    if (y < rect.top())
        type |= TopPoint;
    else if (y > rect.bottom())
        type |= BottomPoint;
    return type;
}

static void clipSegmentToRect(qreal x0, qreal y0, qreal x1, qreal y1,
                              const QRectF &clipRect,
                              QVector<qreal> &outPoints,
                              QVector<QPainterPath::ElementType> &outTypes)
{
    int type0 = clipPointType(x0, y0, clipRect);
    int type1 = clipPointType(x1, y1, clipRect);
    bool accept = false;

    while (true) {
        if (!(type0 | type1)) {
            accept = true;
            break;
        } else if (type0 & type1) {
            break;
        } else {
            qreal x, y;
            int outsideType = type0 ? type0 : type1;

            if (outsideType & BottomPoint) {
                x = x0 + (x1 - x0) * (clipRect.bottom() - y0) / (y1 - y0);
                y = clipRect.bottom() - 0.1;
            } else if (outsideType & TopPoint) {
                x = x0 + (x1 - x0) * (clipRect.top() - y0) / (y1 - y0);
                y = clipRect.top() + 0.1;
            } else if (outsideType & RightPoint) {
                y = y0 + (y1 - y0) * (clipRect.right() - x0) / (x1 - x0);
                x = clipRect.right() - 0.1;
            } else if (outsideType & LeftPoint) {
                y = y0 + (y1 - y0) * (clipRect.left() - x0) / (x1 - x0);
                x = clipRect.left() + 0.1;
            }

            if (outsideType == type0) {
                x0 = x;
                y0 = y;
                type0 = clipPointType(x0, y0, clipRect);
            } else {
                x1 = x;
                y1 = y;
                type1 = clipPointType(x1, y1, clipRect);
            }
        }
    }

    if (accept) {
        if (outPoints.size() >= 2) {
            qreal lastX, lastY;
            lastY = outPoints.at(outPoints.size()-1);
            lastX = outPoints.at(outPoints.size()-2);

            if (!qFuzzyCompare(lastY, y0) || !qFuzzyCompare(lastX, x0)) {
                outTypes << QPainterPath::MoveToElement;
                outPoints << x0 << y0;
            }
        } else {
            outTypes << QPainterPath::MoveToElement;
            outPoints << x0 << y0;
        }

        outTypes << QPainterPath::LineToElement;
        outPoints << x1 << y1;
    }
}

static void clipPathToRect(const QVector<qreal> &points,
                           const QVector<QPainterPath::ElementType> &types,
                           const QRectF &clipRect,
                           QVector<qreal> &outPoints,
                           QVector<QPainterPath::ElementType> &outTypes)
{
    outPoints.clear();
    outPoints.reserve(points.size());
    outTypes.clear();
    outTypes.reserve(types.size());

    qreal lastX, lastY;
    for (int i = 0; i < types.size(); i++) {
        if (i > 0 && types[i] != QPainterPath::MoveToElement) {
            qreal x = points[i*2], y = points[i*2+1];
            clipSegmentToRect(lastX, lastY, x, y, clipRect, outPoints, outTypes);
        }

        lastX = points[i*2];
        lastY = points[i*2+1];
    }
}

void QGeoMapPolylineGeometry::updateScreenPoints(const QGeoMap &map,
                                                 qreal strokeWidth)
{
    if (!screenDirty_)
        return;

    QPointF origin = map.coordinateToScreenPosition(srcOrigin_, false);

    if (!qIsFinite(origin.x()) || !qIsFinite(origin.y()))
        return;

    // Create the viewport rect in the same coordinate system
    // as the actual points
    QRectF viewport(0, 0, map.width(), map.height());
    viewport.adjust(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
    viewport.translate(-1 * origin);

    // Perform clipping to the viewport limits
    QVector<qreal> points;
    QVector<QPainterPath::ElementType> types;
    clipPathToRect(srcPoints_, srcPointTypes_, viewport, points, types);

    QVectorPath vp(points.data(), types.size(), types.data());
    QTriangulatingStroker ts;
    ts.process(vp, QPen(QBrush(Qt::black), strokeWidth), viewport);

    // Nothing is on the screen
    if (ts.vertexCount() == 0)
        return;

    // QTriangulatingStroker#vertexCount is actually the length of the array,
    // not the number of vertices
    screenTriangles_.clear();
    screenTriangles_.reserve(ts.vertexCount());

    screenOutline_ = QPainterPath();

    QPolygonF tri;
    const float *vs = ts.vertices();
    for (int i = 0; i < ts.vertexCount()/2*2; i+=2) {
        screenTriangles_ << vs[i] << vs[i+1];

        if (!qIsFinite(vs[i]) || !qIsFinite(vs[i+1]))
            break;

        tri << QPointF(vs[i], vs[i+1]);
        if (tri.size() == 4) {
            tri.remove(0);
            screenOutline_.addPolygon(tri);
        }
    }

    QRectF bb = screenOutline_.boundingRect();
    screenBounds_ = bb;
    firstPointOffset_ = QPointF(0,0);
    this->translate(-1 * bb.topLeft());
}

QDeclarativePolylineMapItem::QDeclarativePolylineMapItem(QQuickItem *parent) :
    QDeclarativeGeoMapItemBase(parent),
    zoomLevel_(0.0),
    dirtyMaterial_(true)
{
    setFlag(ItemHasContents, true);
    QObject::connect(&line_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(updateAfterLinePropertiesChanged()));
    QObject::connect(&line_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(updateAfterLinePropertiesChanged()));
}

QDeclarativePolylineMapItem::~QDeclarativePolylineMapItem()
{
}

void QDeclarativePolylineMapItem::updateAfterLinePropertiesChanged()
{
    // mark dirty just in case we're a width change
    geometry_.markSourceDirty();
    updateMapItem();
}

void QDeclarativePolylineMapItem::updateAfterCoordinateChanged()
{
    QDeclarativeCoordinate *coord = qobject_cast<QDeclarativeCoordinate*>(QObject::sender());
    if (coord) {
        // TODO: maybe use a QHash instead of indexOf here?
        int idx = this->coordPath_.indexOf(coord);
        path_.replace(idx, coord->coordinate());
        geometry_.markSourceDirty();
        updateMapItem();
    }
}

void QDeclarativePolylineMapItem::setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map){
        QObject::connect(map, SIGNAL(cameraDataChanged(QGeoCameraData)), this, SLOT(handleCameraDataChanged(QGeoCameraData)));
        geometry_.markSourceDirty();
        updateMapItem();
    }
}

/*!
    \qmlproperty list<Coordinate> MapPolyline::path

    This property holds the ordered list of coordinates which
    define the polyline.
*/

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativePolylineMapItem::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this, 0, path_append, path_count,
                                                            path_at, path_clear);
}

void QDeclarativePolylineMapItem::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *property, QDeclarativeCoordinate *coordinate)
{
    QDeclarativePolylineMapItem* item = qobject_cast<QDeclarativePolylineMapItem*>(
                property->object);

    QObject::connect(coordinate, SIGNAL(coordinateChanged(QGeoCoordinate)),
                     item, SLOT(updateAfterCoordinateChanged()));

    item->coordPath_.append(coordinate);
    item->path_.append(coordinate->coordinate());
    item->geometry_.markSourceDirty();
    item->updateMapItem();
    emit item->pathChanged();
}

int QDeclarativePolylineMapItem::path_count(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    return qobject_cast<QDeclarativePolylineMapItem*>(property->object)->coordPath_.count();
}

QDeclarativeCoordinate* QDeclarativePolylineMapItem::path_at(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property, int index)
{
    return qobject_cast<QDeclarativePolylineMapItem*>(property->object)->coordPath_.at(index);
}

void QDeclarativePolylineMapItem::path_clear(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    QDeclarativePolylineMapItem* item = qobject_cast<QDeclarativePolylineMapItem*>(
                property->object);
    qDeleteAll(item->coordPath_);
    item->coordPath_.clear();
    item->path_.clear();
    item->geometry_.markSourceDirty();
    item->updateMapItem();
    emit item->pathChanged();
}

/*!
    \qmlmethod MapPolyline::addCoordinate(Coordinate)

    Adds coordinate to the path.

    \sa removeCoordinate path
*/

void QDeclarativePolylineMapItem::addCoordinate(QDeclarativeCoordinate* coordinate)
{
    coordPath_.append(coordinate);
    path_.append(coordinate->coordinate());

    QObject::connect(coordinate, SIGNAL(coordinateChanged(QGeoCoordinate)),
                     this, SLOT(updateAfterCoordinateChanged()));

    geometry_.markSourceDirty();
    updateMapItem();
    emit pathChanged();
}

/*!
    \qmlmethod MapPolyline::removeCoordinate(Coordinate)

    Remove coordinate from the path. If there are multiple instances of the
    same coordinate, the one added last is removed.

    \sa addCoordinate path
*/

void QDeclarativePolylineMapItem::removeCoordinate(QDeclarativeCoordinate* coordinate)
{
    int index = coordPath_.lastIndexOf(coordinate);

    if (index == -1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolylineMapItem.");
        return;
    }

    if (path_.count() < index + 1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolylineMapItem.");
        return;
    }
    coordPath_.removeAt(index);
    path_.removeAt(index);

    QObject::disconnect(coordinate, SIGNAL(coordinateChanged(QGeoCoordinate)),
                        this, SLOT(updateAfterCoordinateChanged()));

    geometry_.markSourceDirty();
    updateMapItem();
    emit pathChanged();
}

/*!
    \qmlproperty int MapPolyline::line.width
    \qmlproperty color MapPolyline::line.color

    These properties hold the width and color used to draw the line.

    The width is in pixels and is independent of the zoom level of the map.
    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeMapLineProperties *QDeclarativePolylineMapItem::line()
{
    return &line_;
}

void QDeclarativePolylineMapItem::handleCameraDataChanged(const QGeoCameraData& cameraData)
{
    if (cameraData.zoomLevel() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomLevel();
        geometry_.markSourceDirty();
    }

    QSizeF sz = QSizeF(quickMap()->width(), quickMap()->height());
    if (sz != mapSize_) {
        mapSize_ = sz;
        geometry_.markSourceDirty();
    }

    geometry_.markScreenDirty();
    updateMapItem();
}

void QDeclarativePolylineMapItem::updateMapItem()
{
    if (!map() || path_.count() == 0)
        return;

    geometry_.updateSourcePoints(*map(), path_);
    geometry_.updateScreenPoints(*map(), line_.width());
    setWidth(geometry_.screenBoundingBox().width());
    setHeight(geometry_.screenBoundingBox().height());

    setPositionOnMap(path_.at(0), geometry_.firstPointOffset());
    update();
}

QSGNode* QDeclarativePolylineMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapPolylineNode *node = static_cast<MapPolylineNode*>(oldNode);

    if (!node) {
        node = new MapPolylineNode();
    }

    //TODO: update only material
    if (geometry_.isScreenDirty() || dirtyMaterial_) {
        node->update(line_.color(), &geometry_);
        geometry_.markClean();
    }
    return node;
}

bool QDeclarativePolylineMapItem::contains(QPointF point)
{
    return geometry_.contains(point);
}

//////////////////////////////////////////////////////////////////////

MapPolylineNode::MapPolylineNode() :
    geometry_(QSGGeometry::defaultAttributes_Point2D(),0)
{
    geometry_.setDrawingMode(GL_TRIANGLE_STRIP);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);
}


MapPolylineNode::~MapPolylineNode()
{
}

void MapPolylineNode::update(const QColor& fillColor,
                             const QGeoMapItemGeometry* shape)
{
    if (shape->size() == 0)
        return;

    QSGGeometry *fill = QSGGeometryNode::geometry();

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(shape->size());

    Vertex *vertices = (Vertex *)fill->vertexData();

    for (int i = 0; i < shape->size(); ++i) {
        vertices[fillVertexCount++].position = shape->vertex(i);
    }

    Q_ASSERT(fillVertexCount == fill->vertexCount());

    markDirty(DirtyGeometry);

    if (fillColor != fill_material_.color()) {
        fill_material_.setColor(fillColor);
        setMaterial(&fill_material_);
    }
}

QT_END_NAMESPACE
