/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
#include "qgeoprojection_p.h"

#include "qgeocoordinate.h"

#include <qvector2d.h>
#include <qvector3d.h>
#include <QMatrix4x4>
#include <qnumeric.h>

#include <cmath>

QT_BEGIN_NAMESPACE

QGeoProjection::QGeoProjection() {}

QGeoProjection::~QGeoProjection() {}

QVector3D QGeoProjection::mercatorToPoint(const QVector2D &mercator) const
{
    return this->coordToPoint(mercatorToCoord(mercator));
}

QVector2D QGeoProjection::pointToMercator(const QVector3D &point) const
{
    return coordToMercator(this->pointToCoord(point));
}

QVector2D QGeoProjection::coordToMercator(const QGeoCoordinate &coord) const
{
    const double pi = M_PI;

    double lon = coord.longitude() / 360.0 + 0.5;

    double lat = coord.latitude();
    lat = 0.5 - (log(tan((pi / 4.0) + (pi / 2.0) * lat / 180.0)) / pi) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    return QVector2D(lon, lat);
}

double QGeoProjection::realmod(const double a, const double b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<double>(div) * b;
}

QGeoCoordinate QGeoProjection::mercatorToCoord(const QVector2D &mercator) const
{
    const double pi = M_PI;

    double fx = mercator.x();
    double fy = mercator.y();

    if (fy < 0.0)
        fy = 0.0;
    else if (fy > 1.0)
        fy = 1.0;

    double lat;

    if (fy == 0.0)
        lat = 90.0;
    else if (fy == 1.0)
        lat = -90.0;
    else
        lat = (180.0 / pi) * (2.0 * atan(exp(pi * (1.0 - 2.0 * fy))) - (pi / 2.0));

    double lng;
    if (fx >= 0) {
        lng = realmod(fx, 1.0);
    } else {
        lng = realmod(1.0 - realmod(-1.0 * fx, 1.0), 1.0);
    }

    lng = lng * 360.0 - 180.0;

    return QGeoCoordinate(lat, lng, 0.0);
}

QT_END_NAMESPACE
