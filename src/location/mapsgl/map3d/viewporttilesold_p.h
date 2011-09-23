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
#ifndef VIEWPORTTILESOLD_P_H
#define VIEWPORTTILESOLD_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "viewporttiles_p_p.h"

#include "frustum_p.h"

QT_BEGIN_NAMESPACE

class ViewportCamera;

class Arc;

class ViewportTilesPrivateOld : public ViewportTilesPrivate {
public:
    ViewportTilesPrivateOld();
    ~ViewportTilesPrivateOld();

    QList<TileSpec> visibleTiles(const ViewportCamera &viewportCamera) const;

private:
    void calculateVisibleTiles();

    class IntersectionPoint {
    public:
        Frustum::Planes planes;
        QVector3D point;
        bool operator == (const IntersectionPoint &rhs) const {
            return ((planes == rhs.planes) && (point == rhs.point));
        }
    };

    class LengthSorter {
    public:
        IntersectionPoint base;
        bool operator()(const IntersectionPoint &lhs, const IntersectionPoint &rhs);
    };

    QList<Arc> pointIntersections(const Frustum &frustum, int *linesIntersected = 0) const;

    Sphere sphereFromArcs(QList<Arc> &arcs) const;

    Sphere sphereForPole(const QList<Arc> &Arcs, int zeroIndex1) const;
    Sphere sphereForDateline(const QList<Arc> &arcs, int zeroIndex1, int zeroIndex2) const;

    static QList<IntersectionPoint> lineIntersectsSphere(double radius, const QVector3D &start, const QVector3D &end, Frustum::Planes planesStart, Frustum::Planes planesEnd);
    static QVector3D interpolate(const QVector3D &start, const QVector3D &end, double n);

    QList<int> addDatelineCrossings(QList<Arc> &points) const;

    QPair<QVector3D, double> tileInfo(const TileSpec &spec) const;
    QList<TileSpec> visibleTilesFromPlanes(const ViewportCamera &viewportCamera) const;
};

QT_END_NAMESPACE

#endif // VIEWPORTTILESOLD_P_H
