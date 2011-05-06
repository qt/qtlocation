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

#include "qgeomaprouteobject.h"
#include "qgeomaprouteobject_p.h"
#include "qgeocoordinate.h"
#include "qgeoroutesegment.h"

#define DEFAULT_ROUTE_DETAIL_LEVEL 6

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapRouteObject
    \brief The QGeoMapRouteObject class is a QGeoMapObject used to draw
    a route on a map.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps-mapping-objects

    The route is specified by a QGeoRoute object.

    The route might be quite detailed, and so to improve performance the
    detail level can be set with QGeoMapRouteObject::detailLevel.

    The route object draws the route as a series of lines with a minimum
    on-screen harmattan length equal to the detail level.  This is done so that
    the small changes in direction of a road will be visible at high zoom
    levels but will not slow down the rendering at the lower zoom levels.
*/

/*!
    Constructs a new route object.
*/
QGeoMapRouteObject::QGeoMapRouteObject()
    : d_ptr(new QGeoMapRouteObjectPrivate())
{
    setUnits(QGeoMapObject::AbsoluteArcSecondUnit);
    setTransformType(QGeoMapObject::ExactTransform);
}

/*!
    Constructs a new route object for the route \a route.
*/
QGeoMapRouteObject::QGeoMapRouteObject(const QGeoRoute &route)
    : d_ptr(new QGeoMapRouteObjectPrivate())
{
    d_ptr->route = route;
    setUnits(QGeoMapObject::AbsoluteArcSecondUnit);
    setTransformType(QGeoMapObject::ExactTransform);
}

/*!
    Destroys this route object
*/
QGeoMapRouteObject::~QGeoMapRouteObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapRouteObject::type() const
{
    return QGeoMapObject::RouteType;
}

/*!
    \property QGeoMapRouteObject::route
    \brief This property holds the which will be displayed.

    The default value of this property is an empty route.

    If QGeoRoute::path() returns a list of less than 2 valid QGeoCoordinates
    then the route object will not be displayed.
*/
QGeoRoute QGeoMapRouteObject::route() const
{
    return d_ptr->route;
}

void QGeoMapRouteObject::setRoute(const QGeoRoute &route)
{
    d_ptr->route = route;
    emit routeChanged(d_ptr->route);
}

/*!
    \property QGeoMapRouteObject::pen
    \brief This property holds the pen that will be used to draw this object.

    The pen is used to draw the route.

    The pen will be treated like a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
*/
QPen QGeoMapRouteObject::pen() const
{
    return d_ptr->pen;
}

void QGeoMapRouteObject::setPen(const QPen &pen)
{
    QPen newPen = pen;
    newPen.setCosmetic(false);

    if (d_ptr->pen == newPen)
        return;

    d_ptr->pen = newPen;
    emit penChanged(newPen);
}

/*!
    \property QGeoMapRouteObject::detailLevel
    \brief This property holds the level of detail used to draw this object.

    A QGeoRoute instance can contain a large amount of information about the
    path taken by the route. This property is used as a hint to help reduce the
    amount of information that needs to be drawn on the map.

    The path taken by the route is represented as a list of QGeoCoordinate
    instances. This route object will draw lines between these coordinates, but
    will skip members of the list until the manhattan distance between the
    start point and the end point of the line is at least \a detailLevel.

    The default value of this property is 6.
*/
quint32 QGeoMapRouteObject::detailLevel() const
{
    return d_ptr->detailLevel;
}

void QGeoMapRouteObject::setDetailLevel(quint32 detailLevel)
{
    if (d_ptr->detailLevel != detailLevel) {
        d_ptr->detailLevel = detailLevel;
        emit detailLevelChanged(d_ptr->detailLevel);
    }
}

/*!
\fn void QGeoMapRouteObject::routeChanged(const QGeoRoute &route)

    This signal is emitted when the route drawn by this route object
    has changed.

    The new value is \a route.
*/

/*!
\fn void QGeoMapRouteObject::penChanged(const QPen &pen)

    This signal is emitted when the pen used to draw this route object has
    changed.

    The new value is \a pen.
*/

/*!
\fn void QGeoMapRouteObject::detailLevelChanged(quint32 detailLevel)

    This signal is emitted when the level of detail used to draw this
    route object has changed.

    The new value is \a detailLevel.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapRouteObjectPrivate::QGeoMapRouteObjectPrivate()
{
    detailLevel = DEFAULT_ROUTE_DETAIL_LEVEL;
    pen.setCosmetic(true);
}

QGeoMapRouteObjectPrivate::~QGeoMapRouteObjectPrivate() {}

#include "moc_qgeomaprouteobject.cpp"

QTM_END_NAMESPACE

