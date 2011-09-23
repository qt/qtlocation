/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "spheregeometry_p.h"

#include "qgeocoordinate.h"

#include <QVector2D>
#include <QVector3D>
#include <cmath>

#include <QDebug>

SphereGeometry::SphereGeometry()
        : radius_(20000.0),
        zoomLevel_(4) {}

SphereGeometry::SphereGeometry(double radius, int zoomLevel)
        : radius_(radius),
        zoomLevel_(zoomLevel) {}

void SphereGeometry::setRadius(double radius)
{
    radius_ = radius;
}

double SphereGeometry::radius() const
{
    return radius_;
}

//void SphereGeometry::setZoomLevel(int zoomLevel)
//{
//    zoomLevel_ = zoomLevel;
//}

//int SphereGeometry::zoomLevel() const
//{
//    return zoomLevel_;
//}

bool SphereGeometry::operator == (const SphereGeometry &rhs) const
{
    return ((radius_ == rhs.radius_) && (zoomLevel_ == rhs.zoomLevel_));
}

QVector3D SphereGeometry::coordToPoint(const QGeoCoordinate &coord) const
{
    double r = radius_ + coord.altitude();

    const double pi = 3.14159265358;
    double lonRad = pi * -1.0 * (coord.longitude() + 90.0) / 180.0;
    double latRad = pi * (90.0 - coord.latitude()) / 180.0;

    return QVector3D(r * cos(lonRad) * sin(latRad),
                     r * cos(latRad),
                     r * sin(lonRad) * sin(latRad));
}

QVector3D SphereGeometry::coordToPoint(double lat, double lon, double alt) const
{
    double r = radius_ + alt;

    const double pi = 3.14159265358;
    double lonRad = pi * -1.0 * (lon + 90.0) / 180.0;
    double latRad = pi * (90.0 - lat) / 180.0;

    return QVector3D(r * cos(lonRad) * sin(latRad),
                     r * cos(latRad),
                     r * sin(lonRad) * sin(latRad));
}

qreal realmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

QVector3D SphereGeometry::mercatorToPoint(double fx, double fy) const
{
    const double pi = 3.14159265358;

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

    return coordToPoint(lat, lng);
}

QVector3D SphereGeometry::pointToCoord(const QVector3D &point) const
{
    const double pi = 3.14159265358;

    double alt = point.length();
    double lat = 180.0 * acos(point.y() / alt) / pi;
    double lon = 270.0 - (180.0 * atan2(point.z(), point.x()) / pi);

    if (lon < -180.0)
        lon += 360.0;
    if (lon >= 180.0)
        lon -= 360.0;

    return QVector3D(90.0 - lat, lon, alt);
}

QVector2D SphereGeometry::coordToMercator(const QVector2D &coord) const
{
    const double pi = 3.14159265358;

    double lon = coord.y() / 360.0 + 0.5;

    double lat = coord.x();
    lat = 0.5 - (log(tan((pi / 4.0) + (pi / 2.0) * lat / 180.0)) / pi) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    return QVector2D(lon, lat);
}

QVector2D SphereGeometry::pointToMercator(const QVector3D &point) const
{
    return coordToMercator(pointToCoord(point).toVector2D());
}

QVector2D SphereGeometry::coordToTile(const QVector2D &coord, int zoom, bool roundUp) const
{
    QVector2D p = coordToMercator(coord);

    int z = 1 << zoom;
    int x = 0;
    int y = 0;

    if (p.y() == 1.0)
        y = z - 1;
    else
        y = static_cast<int>(z * p.y()) % z;

    if ((qAbs(p.x()) < 1e-6) || (qAbs(p.x() - 1) < 1e-6))
        if (roundUp)
            x = z - 1;
        else
            x = 0;
    else
        x = static_cast<int>(z * p.x()) % z;

    return QVector2D(x, y);
}

QVector2D SphereGeometry::pointToTile(const QVector3D &point, int zoom, bool roundUp) const
{
    return coordToTile(pointToCoord(point).toVector2D(), zoom, roundUp);
}
