/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QPen>
#include "qdeclarativegeomaprouteobject_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapRouteObject::QDeclarativeGeoMapRouteObject(QSGItem *parent)
    : QDeclarativeGeoMapObject(parent), route_(0)
{
    route_ = new QGeoMapRouteObject(); // graphical object
    setMapObject(route_);

    connect(&border_,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&border_,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapRouteObject::~QDeclarativeGeoMapRouteObject()
{
    delete route_;
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

QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapRouteObject::border()
{
    return &border_;
}

void QDeclarativeGeoMapRouteObject::borderColorChanged(const QColor &color)
{
    QPen p = route_->pen();
    p.setColor(color);
    route_->setPen(p);
}

void QDeclarativeGeoMapRouteObject::borderWidthChanged(int width)
{
    QPen p = route_->pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    route_->setPen(p);
}

#include "moc_qdeclarativegeomaprouteobject_p.cpp"

QT_END_NAMESPACE
