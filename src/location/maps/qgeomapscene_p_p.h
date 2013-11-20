/****************************************************************************
**
** Copyright (C) 2014 BlackBerry Limited and/or its subsidiary(-ies).
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOMAPSCENE_P_P_H
#define QGEOMAPSCENE_P_P_H

#ifndef NO_QT3D_RENDERER
#include <Qt3D/qgeometrydata.h>
#else
#include <QSGNode>
#endif

#include "qgeocameradata_p.h"
#include "qgeomapscene_p.h"

#ifndef NO_QT3D_RENDERER
class QGLLightParameters;
#endif

class QGeoMapScenePrivate
{
public:
    explicit QGeoMapScenePrivate(QGeoMapScene *scene);
    ~QGeoMapScenePrivate();

    QSize screenSize_; // in pixels
    int tileSize_; // the pixel resolution for each tile
    QGeoCameraData cameraData_;
    QSet<QGeoTileSpec> visibleTiles_;

#ifndef NO_QT3D_RENDERER
    QGLCamera *camera_;
    QGLSceneNode *sceneNode_;
    QGLLightParameters* light_;
#endif

    // scales up the tile geometry and the camera altitude, resulting in no visible effect
    // other than to control the accuracy of the render by keeping the values in a sensible range
    double scaleFactor_;

    // rounded down, positive zoom is zooming in, corresponding to reduced altitude
    int intZoomLevel_;

    // mercatorToGrid transform
    // the number of tiles in each direction for the whole map (earth) at the current zoom level.
    // it is 1<<zoomLevel
    int sideLength_;

#ifndef NO_QT3D_RENDERER
    QHash<QGeoTileSpec, QGLSceneNode *> nodes_;
#else
    QHash<QGeoTileSpec, QSGNode *> nodes_;
#endif
    QHash<QGeoTileSpec, QSharedPointer<QGeoTileTexture> > textures_;
    QList<QSharedPointer<QGeoTileTexture> > newUploads_;

    // tilesToGrid transform
    int minTileX_; // the minimum tile index, i.e. 0 to sideLength which is 1<< zoomLevel
    int minTileY_;
    int maxTileX_;
    int maxTileY_;
    int tileZ_;    // caches the zoom level for this tile set
    int tileXWrapsBelow_; // the wrap point as a tile index

    // cameraToGrid transform
    double mercatorCenterX_; // center of camera in grid space (0 to sideLength)
    double mercatorCenterY_;
    double mercatorWidth_;   // width of camera in grid space (0 to sideLength)
    double mercatorHeight_;

    // screenToWindow transform
    double screenOffsetX_; // in pixels
    double screenOffsetY_; // in pixels
    // cameraToScreen transform
    double screenWidth_; // in pixels
    double screenHeight_; // in pixels

    bool useVerticalLock_;
    bool verticalLock_;
    bool linearScaling_;

    void addTile(const QGeoTileSpec &spec, QSharedPointer<QGeoTileTexture> texture);

    QDoubleVector2D screenPositionToMercator(const QDoubleVector2D &pos) const;
    QDoubleVector2D mercatorToScreenPosition(const QDoubleVector2D &mercator) const;

    void setVisibleTiles(const QSet<QGeoTileSpec> &tiles);
    void removeTiles(const QSet<QGeoTileSpec> &oldTiles);
    void updateTiles(const QSet<QGeoTileSpec> &tiles);
#ifndef NO_QT3D_RENDERER
    QGeometryData buildGeometry(const QGeoTileSpec &spec);
    QGLSceneNode *buildSceneNodeFromGeometry(const QGeometryData &geom);
    void paintGL(QGLPainter *painter);
#endif
    void setTileBounds(const QSet<QGeoTileSpec> &tiles);
    void setupCamera();
    void setScalingOnTextures();

private:
    QGeoMapScene *q_ptr;
    Q_DECLARE_PUBLIC(QGeoMapScene)
};


#endif // QGEOMAPSCENE_P_P_H
