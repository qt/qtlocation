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
#ifndef QGEOMAP_P_H
#define QGEOMAP_P_H

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
#include <QVector>
#include <QPair>
#include <QPolygonF>
#include <QSizeF>
#include <QVector3D>
#include <QMatrix4x4>
#include <QString>

#include "qgeocameradata.h"
#include "qgeofrustum_p.h"

#include "qgeomaptype.h"

#include <QSharedPointer>

QT_BEGIN_NAMESPACE

class QGeoMappingManager;

class QGeoTile;
class QGeoTileCache;
class QGeoTileSpec;
class QGeoMap;
class QGeoMapController;
class QGeoMapSphere;
class QGeoProjection;

class QGLCamera;
class QGLSceneNode;
class QGLPainter;

class QGeoMapPrivate;

struct TileMap
{
    TileMap(int minY, int maxY);

    int size;
    int minY;
    int maxY;
    QVector<int> minX;
    QVector<int> maxX;

    void adjust(int tileX, int tileY);
};

class IntersectGenerator
{
public:
    enum Axis {
        XAxis,
        YAxis
    };
    IntersectGenerator(const QGeoMapPrivate *mp,
                        double p1,
                        double p2,
                        int t1,
                        int t2,
                        Axis axis,
                        int zoomLevel);

    bool hasNext() const;
    QPair<double, int> value() const;
    void next();

private:
    void generateValue();

protected:
    const QGeoMapPrivate *mp_;
    Axis axis_;
    int zoomLevel_;

    bool hasNext_;
    QPair<double, int> value_;

    int current_;
    int step_;
    int end_;

    int adjust_;
    double first_;
    double denom_;
};

class QGeoMapPrivate
{
public:
    QGeoMapPrivate(QGeoMap *parent, QGeoTileCache *cache, int maxZoom, int tileSize);
    virtual ~QGeoMapPrivate();

    QGeoTileCache* tileCache();

    void setMappingManager(QGeoMappingManager *manager);

    QGeoMapController* mapController();

    QGLCamera* glCamera() const;
    void paintGL(QGLPainter *painter);

    void setCameraData(const QGeoCameraData &cameraData);
    QGeoCameraData cameraData() const;

    void resize(int width, int height);
    int width() const;
    int height() const;
    double aspectRatio() const;

    QGLSceneNode* createTileSpecNode(const QGeoTileSpec &tileSpec);
    QGLSceneNode* createTileNode(const QGeoTile &tile);

    QRect specToRect(const QGeoTileSpec &tileSpec) const;

    void update();

    const QGeoMapType activeMapType() const;
    void setActiveMapType(const QGeoMapType mapType);

    QGeoCoordinate screenPositionToCoordinate(const QPointF &pos) const;
    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;

    QVector2D pointToTile(const QVector3D &point, int zoom, bool roundUp = false) const;
    QVector3D tileXIntersectToPoint(int zoomLevel, int x) const;
    QVector3D tileYIntersectToPoint(int zoomLevel, int y) const;

    void tileFetched(const QGeoTileSpec &spec);

private:
    void updateGlCamera(QGLCamera* glCamera);
    void updateFrustum(QGeoFrustum &frustum);
    QList<QGeoTileSpec> updateVisibleTiles();

    int width_;
    int height_;
    double aspectRatio_;

    QGeoMap *map_;
    QGeoTileCache* cache_;
    QGeoMappingManager *manager_;
    QString pluginString_;
    QGeoMapController *controller_;

    QSharedPointer<QGeoProjection> projection_;

    QGLCamera *glCamera_;

    QGeoCameraData cameraData_;
    QGeoFrustum frustum_;
    QList<QGeoTileSpec> visibleTiles_;

    QGeoMapSphere *sphere_;
    QGeoMapType activeMapType_;

    // from map2d_p.h

    void tilesFromLine(const QVector3D &p1,
                       const QVector3D &p2,
                       const QVector2D &t1,
                       const QVector2D &t2,
                       int zoomLevel,
                       TileMap &map) const;

    QList<QGeoTileSpec> tilesFromPoints(const QVector<QVector3D> &points, bool roundUp) const;

    QPair<QList<QVector3D>,QList<QVector3D> > clipPolygonToMap(const QList<QVector3D> &points) const;

    class LengthSorter {
    public:
        QVector3D base;
        bool operator()(const QVector3D &lhs, const QVector3D &rhs) {
            return (lhs - base).lengthSquared() < (rhs - base).lengthSquared();
        }
    };

    QList<QVector3D> pointsOnLineWithX(const QVector3D &p1, const QVector3D &p2, double x) const;
    QList<QVector3D> pointsOnLineWithY(const QVector3D &p1, const QVector3D &p2, double y) const;
    QList<QVector3D> pointsOnLineWithZ(const QVector3D &p1, const QVector3D &p2, double z) const;

    QPair<QList<QVector3D>,QList<QVector3D> > splitPolygonX(const QList<QVector3D> &points, double x) const;
    QPair<QList<QVector3D>,QList<QVector3D> > splitPolygonY(const QList<QVector3D> &points, double y) const;

    int maxZoom_;
    int tileSize_;

    double baseHeight_;
    double sideLength_;
    QPolygonF screenPoly_;
    QPolygonF screenPolyLeft_;
    QPolygonF screenPolyRight_;

    QSizeF viewSize_;
    QVector3D eye_;
    QMatrix4x4 projectionMatrix_;
};

QT_END_NAMESPACE

#endif // QGEOMAP_P_H
