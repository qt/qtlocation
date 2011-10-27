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
#ifndef MAP_P_H
#define MAP_P_H

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

#include <QList>
#include <QSet>

#include "cameradata.h"
#include "frustum_p.h"

#include "mapitem.h"

#include <QSharedPointer>

class QGeoMappingManager;

class Tile;
class TileCache;
class TileSpec;
class Map;
class MapSphere;
class Projection;
struct MapItemGLResources;

class QGLCamera;
class QGLSceneNode;
class QGLPainter;

class MapPrivate
{
public:
    MapPrivate(Map *parent, TileCache *cache);
    virtual ~MapPrivate();

    void setMappingManager(QGeoMappingManager *manager);

    QGLCamera* glCamera() const;
    void paintGL(QGLPainter *painter);

    QSharedPointer<Projection> projection() const;

    void setCameraData(const CameraData &cameraData);
    CameraData cameraData() const;

    void setAutoUpdate(bool autoUpdate);
    bool autoUpdate() const;

    void resize(int width, int height);

    virtual QGLSceneNode* createTileSpecNode(const TileSpec &tileSpec) = 0;
    QGLSceneNode* createTileNode(const Tile &tile);

    virtual QRect specToRect(const TileSpec &tileSpec) const = 0;

    void update();

    int numMapItems() const;
    QList<MapItem*> mapItems() const;
    QList<MapItem*> mapItemsAt(const QPoint &point) const;
    QList<MapItem*> mapItemsWithin(const QRect &rect) const;
    void addMapItem(MapItem *item);
    void removeMapItem(MapItem *item);
    void clearMapItems();

    virtual QGeoCoordinate screenPositionToCoordinate(const QPointF &pos) const = 0;
    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const = 0;

    QVector2D pointToTile(const QVector3D &point, int zoom, bool roundUp = false) const;
    QVector3D tileXIntersectToPoint(int zoomLevel, int x) const;
    QVector3D tileYIntersectToPoint(int zoomLevel, int y) const;

    virtual void updateMapItemSceneNode(MapItem *item) = 0;

protected:
    Frustum frustum() const;

    void setProjection(const QSharedPointer<Projection> &projection);

    int width() const;
    int height() const;
    double aspectRatio() const;

    QGeoMappingManager* manager() const;

private:
    virtual void updateGlCamera(QGLCamera* glCamera) = 0;
    virtual void updateFrustum(Frustum &frustum) = 0;
    virtual QList<TileSpec> updateVisibleTiles() = 0;

    bool autoUpdate_;

    int width_;
    int height_;
    double aspectRatio_;

    QGeoMappingManager *manager_;

    QSharedPointer<Projection> projection_;

    QGLCamera *glCamera_;

    CameraData cameraData_;
    Frustum frustum_;
    QList<TileSpec> visibleTiles_;

    MapSphere *sphere_;
};

#endif // MAP_P_H
