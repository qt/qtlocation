
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

#include "qdeclarativegeomappolylineobject_p.h"

#include <QColor>
#include <QBrush>

#include <QtDeclarative/qdeclarativeinfo.h>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapPolyline

    \brief The MapPolyline element displays a polyline on a map.
    \inherits QGeoMapPolylineObject

    \ingroup qml-location-maps

    The polyline is specified in terms of an ordered list of
    coordinates.  Any invalid coordinates in the list will be ignored.

    If the list contains less than 2 valid coordinates the polyline
    will not be displayed.

    Simplistic example to illustrate, this element could be defined in Map body:
    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Basic MapPolyline

    The MapPolyline element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoMapPolylineObject::QDeclarativeGeoMapPolylineObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent), polyline_(0), componentCompleted_(false)
{
    polyline_ = new QGeoMapPolylineObject();
    setMapObject(polyline_);

    connect(&border_,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&border_,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapPolylineObject::~QDeclarativeGeoMapPolylineObject()
{
    // QML engine will delete the actual declarative coordinates.
    path_.clear();
    delete polyline_;
}

/*!
    \qmlproperty list<Coordinate> MapPolyline::path
    \default

    This property holds the ordered list of coordinates which
    define the polyline.
*/

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoMapPolylineObject::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
            0,
            path_append,
            path_count,
            path_at,
            path_clear);
}

void QDeclarativeGeoMapPolylineObject::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate)
{
    QDeclarativeGeoMapPolylineObject* poly = static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object);
    poly->path_.append(coordinate);
    QList<QGeoCoordinate> p = poly->polyline_->path();
    p.append(coordinate->coordinate());
    poly->polyline_->setPath(p);
    poly->pathPropertyChanged();
}

int QDeclarativeGeoMapPolylineObject::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object)->path_.count();
}

QDeclarativeCoordinate* QDeclarativeGeoMapPolylineObject::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object)->path_.at(index);
}

void QDeclarativeGeoMapPolylineObject::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoMapPolylineObject* poly = static_cast<QDeclarativeGeoMapPolylineObject*>(prop->object);
    QList<QDeclarativeCoordinate*> p = poly->path_;
    qDeleteAll(p);
    p.clear();
    poly->polyline_->setPath(QList<QGeoCoordinate>());
    poly->pathPropertyChanged();
}

void QDeclarativeGeoMapPolylineObject::componentComplete()
{
    // Setup member coordinates
    QObjectList kids = children();
    QList<QGeoCoordinate> path = polyline_->path();
    for (int i = 0; i < kids.size(); ++i) {
        QDeclarativeCoordinate *coordinate = qobject_cast<QDeclarativeCoordinate*>(kids.at(i));
        if (coordinate) {
            path_.append(coordinate);
            path.append(coordinate->coordinate());
        }
    }
    polyline_->setPath(path);
    componentCompleted_ = true;
    QDeclarativeGeoMapObject::componentComplete();
}

void QDeclarativeGeoMapPolylineObject::pathPropertyChanged()
{
    if (componentCompleted_)
        emit pathChanged();
}

/*!
    \qmlproperty int MapPolyline::border.width
    \qmlproperty color MapPolyline::border.color

    These properties hold the width and color used to draw the border of the circle.

    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapPolylineObject::border()
{
    return &border_;
}

void QDeclarativeGeoMapPolylineObject::borderColorChanged(const QColor &color)
{
    QPen p = polyline_->pen();
    p.setColor(color);
    polyline_->setPen(p);
}

void QDeclarativeGeoMapPolylineObject::borderWidthChanged(int width)
{
    QPen p = polyline_->pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    polyline_->setPen(p);
}

/*!
    \qmlmethod MapPolyline::addCoordinate(Coordinate)

    Adds coordinate to the path. The resulting path is derived
    from values at the time of assignment, meaning that later changes
    in values are not reflected in the path.

    A basic example is to draw the path where one has been:

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Wherever I may roam

    \sa removeCoordinate

*/

void QDeclarativeGeoMapPolylineObject::addCoordinate(QDeclarativeCoordinate* coordinate)
{
    path_.append(coordinate);
    QList<QGeoCoordinate> p = polyline_->path();
    p.append(coordinate->coordinate());
    polyline_->setPath(p);
    emit pathChanged();
}

/*!
    \qmlmethod MapPolyline::removeCoordinate(Coordinate)

    Remove coordinate from the path. If there are multiple instances
    of the same coordinate, the one added last is removed. Removed Coordinate is not deleted.

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Removing from polyline

    If more finetuned control is needed, one can also
    iterate and/or use the inherent index property of the path list.

    \snippet doc/src/snippets/declarative/testpolymapobjects.qml Iterating and removing polyline

    \sa addCoordinate

*/

void QDeclarativeGeoMapPolylineObject::removeCoordinate(QDeclarativeCoordinate* coordinate)
{
    int index = path_.lastIndexOf(coordinate);
    if (index == -1) {
        qmlInfo(this) << tr("Cannot remove nonexistent coordinate.");
        return;
    }
    QList<QGeoCoordinate> path = polyline_->path();
    if (path.count() < index + 1) {
        qmlInfo(this) << tr("Cannot remove the coordinate, it does not exist.");
        return;
    }
    path.removeAt(index);
    path_.removeAt(index);
    polyline_->setPath(path);
    emit pathChanged();
}

/*!
    \qmlproperty int MapPolyline::z

    This property holds the z-value of the polyline.

    Map objects are drawn in z-value order, and objects with the
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapPolyline::visible

    This property holds a boolean corresponding to whether or not the
    polyline is visible.
*/

#include "moc_qdeclarativegeomappolylineobject_p.cpp"

QTM_END_NAMESPACE

