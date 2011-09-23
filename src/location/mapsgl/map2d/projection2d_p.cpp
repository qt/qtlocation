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
#include "projection2d_p.h"

#include "qgeocoordinate.h"

#include <QVector2D>
#include <QVector3D>

Projection2D::Projection2D(double baseHeight, double sideLength)
    : baseHeight_(baseHeight), sideLength_(sideLength) {}

Projection2D::~Projection2D() {}

QVector3D Projection2D::coordToPoint(const QGeoCoordinate &coord) const
{
    QVector2D m = coordToMercator(coord);
    return QVector3D(m.x() * sideLength_, (1.0 - m.y()) * sideLength_, coord.altitude() + baseHeight_);
}

QGeoCoordinate Projection2D::pointToCoord(const QVector3D &point) const
{
    QVector2D m = QVector2D(point.x() / sideLength_, 1.0 - point.y() / sideLength_);
    QGeoCoordinate coord = mercatorToCoord(m);
    coord.setAltitude(point.z() - baseHeight_);
    return coord;
}

QVector3D Projection2D::mercatorToPoint(const QVector2D &mercator) const
{
    return QVector3D(mercator.x() * sideLength_, (1.0 - mercator.y()) * sideLength_, baseHeight_);
}

QVector2D Projection2D::pointToMercator(const QVector3D &point) const
{
    return QVector2D(point.x() / sideLength_, 1.0 - (point.y() / sideLength_));
}

QGeoCoordinate Projection2D::interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress)
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
    QVector3D s = coordToPoint(s2);
    QVector3D e = coordToPoint(e2);

    double x = s.x();

    if (sideLength_ / 2.0 < qAbs(e.x() - s.x())) {
        // handle dateline crossing
    } else {
        x = (1.0 - progress) * s.x() + progress * e.x();
    }

    double y = (1.0 - progress) * s.y() + progress * e.y();

    QGeoCoordinate result = pointToCoord(QVector3D(x, y, 0.0));
    result.setAltitude((1.0 - progress) * start.altitude() + progress * end.altitude());
    return result;

    return start;
}
