/****************************************************************************
 **
 ** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
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
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "qdeclarativepolylinemapitem_p.h"
#include <QDeclarativeInfo>
#include <QPainter>

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

void QDeclarativePolylineMapItem::updatePolyline(QPolygonF& points,const QGeoMap& map, const QList<QGeoCoordinate> &path, qreal& w, qreal& h)
{
    qreal minX, maxX, minY, maxY;
    //TODO: dateline handling

    for (int i = 0; i < path.size(); ++i) {

        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        QPointF point = map.coordinateToScreenPosition(coord, false);

        if (i == 0) {
            minX = point.x();
            maxX = point.x();
            minY = point.y();
            maxY = point.y();
        } else {
            minX = qMin(point.x(), minX);
            maxX = qMax(point.x(), maxX);
            minY = qMin(point.y(), minY);
            maxY = qMax(point.y(), maxY);
        }
        points.append(point);
    }

    points.translate(-minX, -minY);

    w = maxX - minX;
    h = maxY - minY;
}

QDeclarativePolylineMapItem::QDeclarativePolylineMapItem(QQuickItem *parent) :
    QDeclarativeGeoMapItemBase(parent),
    zoomLevel_(0.0),
    dirtyGeometry_(true),
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
    dirtyGeometry_ = true;
    updateMapItem();
}

void QDeclarativePolylineMapItem::updateAfterCoordinateChanged()
{
    QDeclarativeCoordinate *coord = qobject_cast<QDeclarativeCoordinate*>(QObject::sender());
    if (coord) {
        // TODO: maybe use a QHash instead of indexOf here?
        int idx = this->coordPath_.indexOf(coord);
        this->path_.replace(idx, coord->coordinate());
        this->dirtyGeometry_ = true;
        this->updateMapItem();
    }
}

void QDeclarativePolylineMapItem::setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map){
        QObject::connect(map, SIGNAL(cameraDataChanged(QGeoCameraData)), this, SLOT(handleCameraDataChanged(QGeoCameraData)));
        dirtyGeometry_ = true;
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
    item->dirtyGeometry_ = true;
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
    item->dirtyGeometry_ = true;
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

    dirtyGeometry_ = true;
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

    dirtyGeometry_ = true;
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

QSGNode* QDeclarativePolylineMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapPolylineNode *node = static_cast<MapPolylineNode*>(oldNode);

    if (!node) {
        node = new MapPolylineNode();
    }

    //TODO: update only material
    if (dirtyGeometry_ || dirtyMaterial_) {
        node->update(line_.color(), polyline_, line_.width());
        dirtyGeometry_ = false;
        dirtyMaterial_ = false;
    }
    return node;
}

void QDeclarativePolylineMapItem::updateMapItem()
{
    if (!map() || path_.count() == 0)
        return;

    if (dirtyGeometry_) {
           qreal h = 0;
           qreal w = 0;
           polyline_.clear();
           updatePolyline(polyline_, *map(), path_, w, h);
           setWidth(w);
           setHeight(h);
       }

    setPositionOnMap(path_.at(0),polyline_.at(0));
    update();
}

void QDeclarativePolylineMapItem::handleCameraDataChanged(const QGeoCameraData& cameraData)
{
    if (cameraData.zoomFactor() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomFactor();
        dirtyGeometry_ = true;
    }
    updateMapItem();
}

bool QDeclarativePolylineMapItem::contains(QPointF point)
{
    // TODO: this currently returns all points _inside_ of the implicitly closed
    // painterpath whereas we are only interested in the outlines of it
    return polyline_.contains(point);
}

//////////////////////////////////////////////////////////////////////

MapPolylineNode::MapPolylineNode() :
    geometry_(QSGGeometry::defaultAttributes_Point2D(),0)
{
    geometry_.setDrawingMode(GL_LINE_STRIP);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);
}


MapPolylineNode::~MapPolylineNode()
{
}

void MapPolylineNode::update(const QColor& fillColor, const QPolygonF& shape, qreal width)
{
    if (shape.size() == 0)
        return;

    QSGGeometry *fill = QSGGeometryNode::geometry();

    fill->setLineWidth(width);

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(shape.size());

    Vertex *vertices = (Vertex *)fill->vertexData();

    for (int i = 0; i < shape.size(); ++i) {
        vertices[fillVertexCount++].position = QVector2D(shape.at(i));
    }

    Q_ASSERT(fillVertexCount == fill->vertexCount());

    markDirty(DirtyGeometry);

    if (fillColor != fill_material_.color()) {
        fill_material_.setColor(fillColor);
        setMaterial(&fill_material_);
    }

    //TODO: implement me : borders , gradient
}

QT_END_NAMESPACE
