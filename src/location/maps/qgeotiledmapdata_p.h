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
#ifndef QGEOTILEDMAPDATA_P_H
#define QGEOTILEDMAPDATA_P_H

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

#include <QObject>

#include "qgeomapdata_p.h"
#include "qgeocameradata_p.h"
#include "qgeomaptype.h"

QT_BEGIN_NAMESPACE

class QGeoTileSpec;
class QGeoTileCache;
class QGeoTiledMapDataPrivate;
class QGeoTiledMappingManagerEngine;
class MapItem;

class QPointF;

class Q_LOCATION_EXPORT QGeoTiledMapData : public QGeoMapData
{
    Q_OBJECT
public:
    QGeoTiledMapData(QGeoTiledMappingManagerEngine *engine, QObject *parent);
    virtual ~QGeoTiledMapData();

    QGeoTileCache* tileCache();

    void paintGL(QGLPainter *painter);

    void tileFetched(const QGeoTileSpec &spec);

    QGeoCoordinate screenPositionToCoordinate(const QPointF &pos, bool clipToViewport = true) const;
    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate, bool clipToViewport = true) const;

    void updateTileRequests(const QSet<QGeoTileSpec> &tilesAdded, const QSet<QGeoTileSpec> &tilesRemoved);

protected:
    void mapResized(int width, int height);
    void changeCameraData(const QGeoCameraData &oldCameraData);
    void changeActiveMapType(const QGeoMapType mapType);

protected Q_SLOTS:
    virtual void evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles);

private:
    QGeoTiledMapDataPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoTiledMapData)
    Q_DISABLE_COPY(QGeoTiledMapData)

};

QT_END_NAMESPACE

#endif // QGEOMAP_P_H
