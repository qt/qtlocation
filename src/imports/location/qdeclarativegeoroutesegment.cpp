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

#include "qdeclarativegeoroutesegment_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeGeoRouteSegment::QDeclarativeGeoRouteSegment(QObject *parent)
    : QObject(parent)
{
    maneuver_ = new QDeclarativeGeoManeuver(this);
}

QDeclarativeGeoRouteSegment::QDeclarativeGeoRouteSegment(const QGeoRouteSegment &segment,
                                                         QObject *parent)
    : QObject(parent),
      segment_(segment)
{
    maneuver_ = new QDeclarativeGeoManeuver(segment_.maneuver(), this);

    for (int i = 0; i < segment_.path().size(); ++i)
        path_.append(new QDeclarativeCoordinate(segment_.path().at(i), this));
}

QDeclarativeGeoRouteSegment::~QDeclarativeGeoRouteSegment() {}

int QDeclarativeGeoRouteSegment::travelTime() const
{
    return segment_.travelTime();
}

qreal QDeclarativeGeoRouteSegment::distance() const
{
    return segment_.distance();
}

QDeclarativeGeoManeuver* QDeclarativeGeoRouteSegment::maneuver() const
{
    return maneuver_;
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoRouteSegment::path()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
                                                            0,
                                                            path_append,
                                                            path_count,
                                                            path_at,
                                                            path_clear);
}

void QDeclarativeGeoRouteSegment::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate)
{
    //static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->segment_.path().append(coordinate->coordinate());
    static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.append(coordinate);
}

int QDeclarativeGeoRouteSegment::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.count();
}

QDeclarativeCoordinate* QDeclarativeGeoRouteSegment::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.at(index);
}

void QDeclarativeGeoRouteSegment::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    //static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->segment_.path().clear();
    static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.clear();
}

#include "moc_qdeclarativegeoroutesegment_p.cpp"

QTM_END_NAMESPACE
