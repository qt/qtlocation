/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QPen>
#include "qdeclarativegeomaprouteobject_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapRouteObject::QDeclarativeGeoMapRouteObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent), route_(0)
{
    route_ = new QGeoMapRouteObject(); // graphical object
    setMapObject(route_);
}

QDeclarativeGeoMapRouteObject::~QDeclarativeGeoMapRouteObject()
{
    delete route_;
}

void QDeclarativeGeoMapRouteObject::setColor(const QColor &color)
{
    if (color_ == color)
        return;
    color_ = color;
    QPen pen(color);
    route_->setPen(pen);
    emit colorChanged();
}

QColor QDeclarativeGeoMapRouteObject::color() const
{
    return color_;
}

void QDeclarativeGeoMapRouteObject::setRoute(QDeclarativeGeoRoute* route)
{
    if (route == declarativeRoute_)
        return;
    declarativeRoute_ = route;
    if (declarativeRoute_) {
        route_->setRoute(declarativeRoute_->route_);
    } else {
        route_->setRoute(QGeoRoute());
    }
    emit routeChanged();
}

QDeclarativeGeoRoute* QDeclarativeGeoMapRouteObject::route()
{
    return declarativeRoute_;
}

void QDeclarativeGeoMapRouteObject::setDetailLevel(quint32 detailLevel)
{
    if (detailLevel == route_->detailLevel())
        return;
    route_->setDetailLevel(detailLevel);
    emit detailLevelChanged();
}

quint32 QDeclarativeGeoMapRouteObject::detailLevel() const
{
    return route_->detailLevel();
}

#include "moc_qdeclarativegeomaprouteobject_p.cpp"

QT_END_NAMESPACE
