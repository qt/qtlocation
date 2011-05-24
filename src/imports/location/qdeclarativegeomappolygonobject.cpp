
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "qdeclarativegeomappolygonobject_p.h"

#include <QColor>
#include <QBrush>

#include <QtDeclarative/qdeclarativeinfo.h>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapPolygon

    \brief The MapPolygon element displays a polygon on a map.
    \inherits QGeoMapPolygonObject

    \ingroup qml-location-maps

    The polygon is specified in terms of an ordered list of
    coordinates.  Any invalid coordinates in the list will be ignored.

    If the list contains less than 3 valid coordinates the polygon
    will not be displayed.

    Simplistic example to illustrate, this element could be defined in Map body:
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic MapPolygon

    The MapPolygon element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoMapPolygonObject::QDeclarativeGeoMapPolygonObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent), polygon_(0), componentCompleted_(false)
{
    polygon_ = new QGeoMapPolygonObject();
    setMapObject(polygon_);

    connect(&border_,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&border_,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapPolygonObject::~QDeclarativeGeoMapPolygonObject()
{
    // QML engine deletes coordinates, no need to delete them here.
    path_.clear();
    delete polygon_;
}

/*!
    \qmlproperty list<Coordinate> MapPolygon::path
    \default

    This property holds the ordered list of coordinates which
    define the polygon.
*/

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoMapPolygonObject::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
            0,
            path_append,
            path_count,
            path_at,
            path_clear);
}

void QDeclarativeGeoMapPolygonObject::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate)
{
    QDeclarativeGeoMapPolygonObject* poly = static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object);
    poly->path_.append(coordinate);
    QList<QGeoCoordinate> p = poly->polygon_->path();
    p.append(coordinate->coordinate());
    poly->polygon_->setPath(p);
    poly->pathPropertyChanged();
}

int QDeclarativeGeoMapPolygonObject::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->path_.count();
}

QDeclarativeCoordinate* QDeclarativeGeoMapPolygonObject::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->path_.at(index);
}

void QDeclarativeGeoMapPolygonObject::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoMapPolygonObject* poly = static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object);
    QList<QDeclarativeCoordinate*> p = poly->path_;
    qDeleteAll(p);
    p.clear();
    poly->polygon_->setPath(QList<QGeoCoordinate>());
    poly->pathPropertyChanged();
}

void QDeclarativeGeoMapPolygonObject::pathPropertyChanged()
{
    if (componentCompleted_)
        emit pathChanged();
}

void QDeclarativeGeoMapPolygonObject::componentComplete()
{
    // Setup member coordinates
    QObjectList kids = children();
    QList<QGeoCoordinate> path = polygon_->path();
    for (int i = 0; i < kids.size(); ++i) {
        QDeclarativeCoordinate *coordinate = qobject_cast<QDeclarativeCoordinate*>(kids.at(i));
        if (coordinate) {
            path_.append(coordinate);
            path.append(coordinate->coordinate());
        } else {
            qmlInfo(this) << tr("Member is not a Coordinate");
        }
    }
    polygon_->setPath(path);
    componentCompleted_ = true;
    QDeclarativeGeoMapObject::componentComplete();
}

/*!
    \qmlproperty color MapPolygon::color

    This property holds the color used to fill the circle.

    The default value corresponds to a transparent color.
*/

void QDeclarativeGeoMapPolygonObject::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    QBrush m_brush(color);
    polygon_->setBrush(m_brush);
    emit colorChanged(color_);
}

QColor QDeclarativeGeoMapPolygonObject::color() const
{
    return color_;
}

/*!
    \qmlproperty int MapPolygon::border.width
    \qmlproperty color MapPolygon::border.color

    These properties hold the width and color used to draw the border of the circle.

    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapPolygonObject::border()
{
    return &border_;
}

void QDeclarativeGeoMapPolygonObject::borderColorChanged(const QColor &color)
{
    QPen p = polygon_->pen();
    p.setColor(color);
    polygon_->setPen(p);
}

void QDeclarativeGeoMapPolygonObject::borderWidthChanged(int width)
{
    QPen p = polygon_->pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    polygon_->setPen(p);
}

/*!
    \qmlmethod MapPolygon::addCoordinate(Coordinate)

    Adds coordinate to the path. The resulting path is derived
    from values at the time of assignment, meaning that later changes
    in values are not reflected in the path.

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Adding to polygon

    \sa removeCoordinate

*/

void QDeclarativeGeoMapPolygonObject::addCoordinate(QDeclarativeCoordinate* coordinate)
{
    path_.append(coordinate);
    QList<QGeoCoordinate> p = polygon_->path();
    p.append(coordinate->coordinate());
    polygon_->setPath(p);
    emit pathChanged();
}

/*!
    \qmlmethod MapPolygon::removeCoordinate(Coordinate)

    Remove coordinate from the path. Removed Coordinate is not deleted.
    If there are multiple instances of the same coordinate, the one added
    last is removed.

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Removing from polygon

    If more finetuned control is needed, one can also
    iterate and/or use the inherent index property of the path list.

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Iterating and removing polyline

    \sa addCoordinate

*/

void QDeclarativeGeoMapPolygonObject::removeCoordinate(QDeclarativeCoordinate* coordinate)
{
    int index = path_.lastIndexOf(coordinate);
    if (index == -1) {
        qmlInfo(this) << tr("Cannot remove nonexistent coordinate.");
        return;
    }
    QList<QGeoCoordinate> path = polygon_->path();
    if (path.count() < index + 1) {
        qmlInfo(this) << tr("Cannot remove the coordinate, it does not exist.");
        return;
    }
    path.removeAt(index);
    path_.removeAt(index);
    polygon_->setPath(path);
    emit pathChanged();
}

/*!
    \qmlproperty int MapPolygon::z

    This property holds the z-value of the polygon.

    Map objects are drawn in z-value order, and objects with the
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapPolygon::visible

    This property holds a boolean corresponding to whether or not the
    polygon is visible.
*/

#include "moc_qdeclarativegeomappolygonobject_p.cpp"

QTM_END_NAMESPACE

