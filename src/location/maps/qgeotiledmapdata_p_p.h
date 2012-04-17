/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QGEOTILEDMAPDATA_P_P_H
#define QGEOTILEDMAPDATA_P_P_H

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
#include <QMatrix4x4>
#include <QString>
#include <QSharedPointer>

#include "qgeocameradata_p.h"
#include "qgeomaptype.h"

#include "qdoublevector3d_p.h"

QT_BEGIN_NAMESPACE

class QGeoTile;
class QGeoTileCache;
class QGeoTileSpec;
class QGeoMapController;
class QGeoProjection;

class QGeoCameraTiles;
class QGeoMapImages;
class QGeoMapGeometry;

class QGLSceneNode;

class QGeoTiledMapDataPrivate
{
public:
    QGeoTiledMapDataPrivate(QGeoTiledMapData *parent, QGeoTiledMappingManagerEngine *engine);
    ~QGeoTiledMapDataPrivate();

    QGeoTileCache* tileCache();

    void paintGL(QGLPainter *painter);

    void changeCameraData(const QGeoCameraData &oldCameraData);
    void changeActiveMapType(const QGeoMapType mapType);
    void resized(int width, int height);

    QGeoCoordinate screenPositionToCoordinate(const QPointF &pos) const;
    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;

    void tileFetched(const QGeoTileSpec &spec);
    QSet<QGeoTileSpec> visibleTiles();

    QGeoTiledMappingManagerEngine *engine() const;

private:
    QGeoTiledMapData *map_;
    QGeoTileCache* cache_;
    QGeoTiledMappingManagerEngine *engine_;

    QSet<QGeoTileSpec> visibleTiles_;

    QGeoCameraTiles *cameraTiles_;
    QGeoMapGeometry *mapGeometry_;
    QGeoMapImages *mapImages_;

    Q_DISABLE_COPY(QGeoTiledMapDataPrivate)
};

QT_END_NAMESPACE

#endif // QGEOTILEDMAP_P_P_H
