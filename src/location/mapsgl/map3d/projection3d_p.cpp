/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
#include "projection3d_p.h"

#include "qgeocoordinate.h"

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <qnumeric.h>

#include <cmath>

Projection3D::Projection3D(double radius)
    : radius_(radius) {}

Projection3D::~Projection3D() {}

double Projection3D::radius() const
{
    return radius_;
}

QVector3D Projection3D::coordToPoint(const QGeoCoordinate &coord) const
{
    double r = radius_ + coord.altitude();

    const double pi = M_PI;
    double lonRad = pi * -1.0 * (coord.longitude() + 90.0) / 180.0;
    double latRad = pi * (90.0 - coord.latitude()) / 180.0;

    return QVector3D(r * cos(lonRad) * sin(latRad),
                     r * cos(latRad),
                     r * sin(lonRad) * sin(latRad));
}

QGeoCoordinate Projection3D::pointToCoord(const QVector3D &point) const
{
    const double pi = M_PI;

    double alt = point.length();
    double lat = 180.0 * acos(point.y() / alt) / pi;
    double lon = 270.0 - (180.0 * atan2(point.z(), point.x()) / pi);

    if (lon < -180.0)
        lon += 360.0;
    if (lon >= 180.0)
        lon -= 360.0;

    return QGeoCoordinate(90.0 - lat, lon, alt);
}

QGeoCoordinate Projection3D::interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress)
{
    if (start == end) {
        if (progress < 0.5) {
            return start;
        } else {
            return end;
        }
    }

    QGeoCoordinate s2 = start;
    s2.setAltitude(0.0);
    QGeoCoordinate e2 = end;
    e2.setAltitude(0.0);
    QVector3D s = coordToPoint(s2).normalized();
    QVector3D e = coordToPoint(e2).normalized();

    double dot = QVector3D::dotProduct(s, e);
    const double pi = M_PI;
    double angle = 180.0 * acos(dot) / pi;
    if (qFuzzyCompare(qAbs(dot), 1.0) || qIsNaN(angle)) {  // qIsNan is extra check, fuzzy compare is sometimes too 'loose'
        if (progress < 0.5) {
            return start;
        } else {
            return end;
        }
    }

    QVector3D axis = QVector3D::crossProduct(s, e);
    QMatrix4x4 mRot;
    mRot.rotate(angle * progress, axis);
    QVector3D p = mRot * s;

    QGeoCoordinate result = pointToCoord(p);
    result.setAltitude((1.0 - progress) * start.altitude() + progress * end.altitude());
    return result;
}

