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
#ifndef MAP3D_P_H
#define MAP3D_P_H

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

#include "map_p.h"
#include "viewport_p.h"

QT_BEGIN_NAMESPACE

class QGeometryData;

class Map3DPrivate : public MapPrivate
{
public:
    Map3DPrivate(Map *parent, TileCache *cache, double radius);
    virtual ~Map3DPrivate();

    virtual QRect specToRect(const TileSpec &tileSpec) const;
    virtual QGLSceneNode* createTileSpecNode(const TileSpec &tileSpec);

    virtual void updateGlCamera(QGLCamera* glCamera);
    virtual void updateFrustum(Frustum &frustum);
    virtual QList<TileSpec> updateVisibleTiles();

    virtual QGeoCoordinate screenPositionToCoordinate(const QPointF &pos) const;
    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;

private:
    QGeometryData generateTileGeometryData(int x, int y, int tileZoom, int geomZoom) const;

    int minZoom_;
    double radius_;
    Viewport viewport_;
};

QT_END_NAMESPACE

#endif // MAP3D_P_H
