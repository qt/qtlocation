/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOMAPOBJECTENGINE_P_H
#define QGEOMAPOBJECTENGINE_P_H

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

#include "qmobilityglobal.h"
#include "qgeomapdata_p.h"
#include "qgeomapdata.h"
#include "qgeomapobject_p.h"
#include "qgeomapobject.h"

#include <QHash>
#include <QList>

class QTransform;
class QGraphicsItem;
class QGraphicsScene;

QTM_BEGIN_NAMESPACE

class QGeoMapObjectEngine : public QObject
{
    Q_OBJECT
public:
    QGeoMapObjectEngine(QGeoMapData *mapData, QGeoMapDataPrivate *mapDataP);
    virtual ~QGeoMapObjectEngine();

    QGeoMapData *md;
    QGeoMapDataPrivate *mdp;

    double exactMappingTolerance;

    /*** Lookup hashes and scenes ***/

    QHash<const QGeoMapObject*, QGraphicsItem*> latLonExact;
    QHash<const QGeoMapObject*, QGraphicsItem*> pixelExact;

    QHash<const QGeoMapObject*, QTransform> latLonTrans;
    QGraphicsScene *latLonScene;
    QHash<QGraphicsItem*, QGeoMapObject*> latLonItems;
    QHash<QGeoMapObject*, QGraphicsItem*> latLonItemsRev;

    QHash<const QGeoMapObject*, QTransform> pixelTrans;
    QGraphicsScene *pixelScene;
    QHash<QGraphicsItem*, QGeoMapObject*> pixelItems;
    QHash<QGeoMapObject*, QGraphicsItem*> pixelItemsRev;

    /*** Object management ***/

    virtual void addObject(QGeoMapObject *object);
    virtual void removeObject(QGeoMapObject *object);

    /*** Updating and validity ***/

    virtual void updateTransforms();

    QList<QGeoMapObject*> objectsForPixelUpdate;
    QList<QGeoMapObject*> objectsForLatLonUpdate;

    virtual void updatePixelsForGroup(QGeoMapGroupObject *group);
    virtual void updateLatLonsForGroup(QGeoMapGroupObject *group);

    virtual void updateLatLonTransform(QGeoMapObject *object);
    virtual void updatePixelTransform(QGeoMapObject *object);

    virtual void rebuildScenes();

    virtual void invalidateObject(QGeoMapObject *obj);
    virtual void invalidateZoomDependents();
    virtual void invalidatePixelsForViewport(bool updateNow=true);
    virtual void trimPixelTransforms();

public:

    /*** Exact transform functions ***/

    // these are virtual so subclasses can add new types
    // of exact transform
    virtual bool exactMetersToSeconds(const QGeoCoordinate &origin,
                                      QGeoMapObject *object,
                                      QGraphicsItem *item,
                                      QList<QPolygonF> &polys);
    virtual bool exactSecondsToSeconds(const QGeoCoordinate &origin,
                                       QGeoMapObject *object,
                                       QGraphicsItem *item,
                                       QList<QPolygonF> &polys);

    virtual void exactPixelMap(const QGeoCoordinate &origin,
                               QGeoMapObject *object,
                               QList<QPolygonF> &polys);

    /*** Bilinear transforms ***/

    void bilinearMetersToSeconds(const QGeoCoordinate &origin,
                                 QGraphicsItem *item,
                                 QPolygonF &local,
                                 QTransform &latLon);
    void bilinearPixelsToSeconds(const QGeoCoordinate &origin,
                                 QGraphicsItem *item,
                                 QPolygonF &local,
                                 QTransform &latLon);
    void bilinearSecondsToScreen(const QGeoCoordinate &origin,
                                 QGeoMapObject *object,
                                 QList<QPolygonF> &polys);

    /*** Pixel shift transforms ***/

    void pixelShiftToScreen(const QGeoCoordinate &origin,
                            QGeoMapObject *object,
                            QList<QPolygonF> &polys);

    /*** Misc ***/
    virtual QGraphicsItem* graphicsItemFromMapObject(QGeoMapObject *object);

private:
    /*** Internal helpers ***/
    QPolygonF latLonViewport();
    QPolygonF polyToScreen(const QPolygonF &poly);

    Q_DISABLE_COPY(QGeoMapObjectEngine)
};

QTM_END_NAMESPACE

#endif // QGEOMAPOBJECTENGINE_P_H
