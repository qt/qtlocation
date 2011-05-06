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

#include "qgeomapobjectengine_p.h"
#include "qgeomaprouteobject.h"
#include "qgeomapcircleobject.h"
#include "qgeocoordinate_p.h"
#include "qgeoboundingbox.h"
#include "projwrapper_p.h"
#include "qgeotiledmapobjectinfo_p.h"

#include <QTransform>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>

#include <cmath>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*
  A quick note about how the transforms are set up:

  Each QGeoMapObject has its own "local" coordinate system, whether this
  a pixel system or meters or whatever.

  For local systems that are not in pixels, we do a two-stage transform --
  first we transform the object to arc-seconds, then we transform from
  arc-seconds to pixels on the screen. This is necessary as QGeoMapData
  subclasses only provide a mapping from lat/lon coordinates to screen pixels
  and no other source.

  For local systems that are in pixels, we simply translate to the screen
  coordinates. However, we still generate a transform to arc-seconds for these
  objects, to speed up the drawing process.

  QGeoMapDataPrivate has two sets of 3 fields that are used here:
    - xxxTrans
    - xxxScene
    - xxxItems
  (where xxx = [latLon, pixel])

  xxxTrans = a multi-hash of transforms associated with each QGeoMapObject
             (a given map object may appear at more than one coordinate if
              it wraps over the dateline)
  xxxScene = a QGraphicsScene filled with bounding boxes for each object, one
             for each entry in xxxTrans
  xxxItems = a hash taking the items in the xxxScene and giving back the original
             QGeoMapObject they were created for

  the "latLon" entries describe the transformations from local coordinates to
  arc-seconds, and the "pixel" entries describe the transformations from local
  coordinates to pixels on screen.

  Items within latLonScene have their coordinates in arc-seconds, and items
  within pixelScene have theirs in pixels.

*/

QGeoMapObjectEngine::QGeoMapObjectEngine(QGeoMapData *mapData, QGeoMapDataPrivate *mapDataP) :
    md(mapData),
    mdp(mapDataP),
    exactMappingTolerance(1.0),
    latLonScene(new QGraphicsScene),
    pixelScene(new QGraphicsScene)
{
    pixelScene->setItemIndexMethod(QGraphicsScene::NoIndex);
}

QGeoMapObjectEngine::~QGeoMapObjectEngine()
{
    delete pixelScene;
    delete latLonScene;

    latLonTrans.clear();
    latLonItems.clear();
    latLonItemsRev.clear();
    pixelTrans.clear();
    pixelItems.clear();
    pixelItemsRev.clear();

    foreach (QGraphicsItem *i, latLonExact.values())
        delete i;
    latLonExact.clear();

    foreach (QGraphicsItem *i, pixelExact.values())
        delete i;
    pixelExact.clear();
}

/*****************************************************************************
  Object management
 *****************************************************************************/

void QGeoMapObjectEngine::addObject(QGeoMapObject *object)
{
    objectsForLatLonUpdate << object;
    objectsForPixelUpdate << object;
    updateTransforms();
    rebuildScenes();
}

void QGeoMapObjectEngine::removeObject(QGeoMapObject *object)
{
    QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(object);
    if (group) {
        foreach (QGeoMapObject *child, group->childObjects())
            removeObject(child);
    } else {
        QList<QRectF> rectsToUpdate;
        foreach (QGraphicsItem *item, pixelItemsRev.values(object))
            rectsToUpdate << item->boundingRect();

        latLonTrans.remove(object);
        foreach (QGraphicsItem *item, latLonItemsRev.values(object)) {
            latLonItems.remove(item);
            latLonScene->removeItem(item);
            delete item;
        }
        latLonItemsRev.remove(object);

        pixelTrans.remove(object);
        foreach (QGraphicsItem *item, pixelItemsRev.values(object)) {
            pixelItems.remove(item);
            pixelScene->removeItem(item);
            delete item;
        }
        pixelItemsRev.remove(object);

        foreach (QRectF rect, rectsToUpdate)
            mdp->emitUpdateMapDisplay(rect);
    }
}

/*****************************************************************************
  Transform support functions
 *****************************************************************************/

static QGraphicsPolygonItem *polyCopy(const QGraphicsPolygonItem *polyItem)
{
    QGraphicsPolygonItem *pi = new QGraphicsPolygonItem;
    pi->setBrush(polyItem->brush());
    pi->setPen(polyItem->pen());
    pi->setVisible(polyItem->isVisible());
    pi->setFillRule(polyItem->fillRule());
    pi->setOpacity(polyItem->opacity());
    pi->setGraphicsEffect(polyItem->graphicsEffect());
    return pi;
}

static QGraphicsPolygonItem *polyCopy(const QGraphicsEllipseItem *elItem)
{
    QGraphicsPolygonItem *pi = new QGraphicsPolygonItem;
    pi->setBrush(elItem->brush());
    pi->setPen(elItem->pen());
    pi->setVisible(elItem->isVisible());
    pi->setOpacity(elItem->opacity());
    pi->setGraphicsEffect(elItem->graphicsEffect());
    return pi;
}

static QGraphicsPathItem *pathCopy(const QGraphicsPathItem *pathItem)
{
    QGraphicsPathItem *pi = new QGraphicsPathItem;
    pi->setBrush(pathItem->brush());
    pi->setPen(pathItem->pen());
    pi->setVisible(pathItem->isVisible());
    pi->setOpacity(pathItem->opacity());
    pi->setGraphicsEffect(pathItem->graphicsEffect());
    return pi;
}

static QPolygonF approximateCircle(QGraphicsEllipseItem *elItem,
                                   QGeoMapObject *object,
                                   const QGeoCoordinate &center,
                                   const ProjCoordinate &projCenter)
{
    const QRectF rect = elItem->rect();

    const double a = rect.height() / 2.0;
    const double b = rect.width() / 2.0;

    const double asq = a*a;
    const double bsq = b*b;

    QPolygonF secPoly;

    quint32 detail = 150;
    QGeoMapCircleObject *circObj = dynamic_cast<QGeoMapCircleObject*>(object);
    if (circObj)
        detail = circObj->pointCount();

    const double Pi = 3.14159265358;
    const double twopi = 6.283185307179;

    const double dth = twopi / detail;

    // TODO: make the semantics here the same as in normal graphicsview
    double startAngle = elItem->startAngle();
    startAngle /= 16.0;
    startAngle *= twopi;
    startAngle /= 360.0;

    double stopAngle = elItem->startAngle() + elItem->spanAngle();
    stopAngle /= 16.0;
    stopAngle *= twopi;
    stopAngle /= 360.0;

    bool drawToCenter = (elItem->spanAngle() != 360 * 16);

    for (double theta = startAngle; theta < stopAngle; theta += dth) {
        const double top = b*sin(theta);
        const double bottom = a*cos(theta);

        double phi = atan(top / bottom);
        if (bottom < 0)
            phi = phi + Pi;

        const double phiDeg = (360.0 * phi) / twopi;

        const double costh = cos(theta);
        const double sinth = sin(theta);

        const double r = sqrt(asq*costh*costh + bsq*sinth*sinth);

        double x, y;
        QGeoCoordinatePrivate::atDistanceAndAzimuth(center, r, phiDeg,
                                                    &x, &y);
        x *= 3600.0;
        y *= 3600.0;

        secPoly << QPointF(x,y);
    }

    if (drawToCenter)
        secPoly << QPointF(projCenter.x() * 3600.0, projCenter.y() * 3600.0);

    return secPoly;
}

bool QGeoMapObjectEngine::exactMetersToSeconds(const QGeoCoordinate &origin,
                                              QGeoMapObject *object,
                                              QGraphicsItem *item,
                                              QList<QPolygonF> &polys)
{
    QString projStr = "+proj=tmerc +lat_0=%1 +lon_0=%2 +k=1.0 +x_0=0 +y_0=0 +ellps=WGS84";
    projStr = projStr.arg(origin.latitude(), 0, 'f', 12)
                     .arg(origin.longitude(), 0, 'f', 12);

    ProjCoordinateSystem localSys(projStr, false);
    ProjCoordinateSystem wgs84("+proj=latlon +ellps=WGS84");

    QTransform west;
    west.translate(360.0 * 3600.0, 0.0);

    QTransform east;
    east.translate(-360.0 * 3600.0, 0.0);

    foreach (QGraphicsItem *i, latLonExact.values(object))
        delete i;
    latLonExact.remove(object);

    QGraphicsEllipseItem *elItem = dynamic_cast<QGraphicsEllipseItem*>(item);
    if (elItem) {
        QRectF rect = elItem->rect();

        const QPointF cen = rect.center();
        ProjCoordinate c(cen.x(), -1*cen.y(), 0.0, localSys);
        c.convert(wgs84);
        const QGeoCoordinate center = c.toGeoCoordinate();

        QPolygonF wgs = approximateCircle(elItem, object, center, c);

        QGraphicsPolygonItem *pi = polyCopy(elItem);
        pi->setPolygon(wgs);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF westPoly = wgs * west;
        pi = polyCopy(elItem);
        pi->setPolygon(westPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF eastPoly = wgs * east;
        pi = polyCopy(elItem);
        pi->setPolygon(eastPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(item);
    if (polyItem) {
        QPolygonF poly = polyItem->polygon() * polyItem->transform();

        ProjPolygon p(poly, localSys);
        p.scalarMultiply(1, -1, 1);
        p.convert(wgs84);
        QPolygonF wgs = p.toPolygonF(3600.0);

        QGraphicsPolygonItem *pi = polyCopy(polyItem);
        pi->setPolygon(wgs);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF westPoly = wgs * west;
        pi = polyCopy(polyItem);
        pi->setPolygon(westPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF eastPoly = wgs * east;
        pi = polyCopy(polyItem);
        pi->setPolygon(eastPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if (pathItem) {
        QPainterPath path = pathItem->path() * pathItem->transform();

        for (int i = 0; i < path.elementCount(); ++i) {
            QPainterPath::Element e = path.elementAt(i);

            ProjCoordinate c(e.x, -1*e.y, 0.0, localSys);
            Q_ASSERT(c.convert(wgs84));

            path.setElementPositionAt(i, c.x() * 3600.0, c.y() * 3600.0);
        }

        QGraphicsPathItem *pi = pathCopy(pathItem);
        pi->setPath(path);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPainterPath westPath = path * west;
        pi = pathCopy(pathItem);
        pi->setPath(westPath);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPainterPath eastPath = path * east;
        pi = pathCopy(pathItem);
        pi->setPath(eastPath);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    qWarning("QGeoMapData: did not recognize type of exact-transformed"
             "object: type #%d (object not supported for exact transform)",
             item->type());
    return false;
}

bool QGeoMapObjectEngine::exactSecondsToSeconds(const QGeoCoordinate &origin,
                                               QGeoMapObject *object,
                                               QGraphicsItem *item,
                                               QList<QPolygonF> &polys)
{
    QTransform west;
    west.translate(360.0 * 3600.0, 0.0);

    QTransform east;
    east.translate(-360.0 * 3600.0, 0.0);

    QTransform toAbs;
    if (object->units() == QGeoMapObject::RelativeArcSecondUnit) {
        double ox = origin.longitude() * 3600.0;
        double oy = origin.latitude() * 3600.0;
        toAbs.translate(ox, oy);
    }

    foreach (QGraphicsItem *i, latLonExact.values(object))
        delete i;
    latLonExact.remove(object);

    QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(item);
    if (polyItem) {
        if (polyItem->polygon().isEmpty() || polyItem->polygon().size() < 3)
            return false;

        QPolygonF poly = polyItem->polygon() * polyItem->transform();
        poly = poly * toAbs;

        QGraphicsPolygonItem *pi = polyCopy(polyItem);
        pi->setPolygon(poly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF westPoly = poly * west;
        pi = polyCopy(polyItem);
        pi->setPolygon(westPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF eastPoly = poly * east;
        pi = polyCopy(polyItem);
        pi->setPolygon(eastPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if (pathItem) {
        if (pathItem->path().isEmpty() || pathItem->path().elementCount() < 2)
            return false;

        QPainterPath path = pathItem->path() * pathItem->transform();
        path = path * toAbs;

        QGraphicsPathItem *pi = pathCopy(pathItem);
        pi->setPath(path);
        latLonExact.insertMulti(object, pi);
        const QPolygonF originalBounds = pi->boundingRect();
        polys << originalBounds;

        QPainterPath westPath = path * west;
        pi = pathCopy(pathItem);
        pi->setPath(westPath);
        latLonExact.insertMulti(object, pi);
        polys << originalBounds * west;

        QPainterPath eastPath = path * east;
        pi = pathCopy(pathItem);
        pi->setPath(eastPath);
        latLonExact.insertMulti(object, pi);
        polys << originalBounds * east;

        return true;
    }

    qWarning("QGeoMapData: did not recognize type of exact-transformed "
             "object: type #%d (object not supported for exact transform)",
             item->type());
    return false;
}

void QGeoMapObjectEngine::bilinearMetersToSeconds(const QGeoCoordinate &origin,
                                                 QGraphicsItem *item,
                                                 QPolygonF &local,
                                                 QTransform &latLon)
{
    QString projStr = "+proj=tmerc +lat_0=%1 +lon_0=%2 +k=1.0 +x_0=0 +y_0=0 +ellps=WGS84";
    projStr = projStr.arg(origin.latitude(), 0, 'f', 12)
                     .arg(origin.longitude(), 0, 'f', 12);

    ProjCoordinateSystem localSys(projStr, false);
    ProjCoordinateSystem wgs84("+proj=latlon +ellps=WGS84");

    ProjPolygon p(local, localSys);
    if (!p.convert(wgs84)) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: projection is singular");
        return;
    }
    QPolygonF wgs = p.toPolygonF(3600.0);

    // QTransform expects the last vertex (closing vertex) to be dropped
    local.remove(4);
    wgs.remove(4);

    // perform wrapping
    if (wgs.at(2).x() < wgs.at(3).x()) {
        QPointF topRight = wgs.at(1);
        topRight.setX(topRight.x() + 360.0 * 3600.0);
        wgs.replace(1, topRight);

        QPointF bottomRight = wgs.at(2);
        bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);
        wgs.replace(2, bottomRight);
    }

    bool ok = QTransform::quadToQuad(local, wgs, latLon);
    if (!ok) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: could not obtain a transformation matrix");
        return;
    }

    QTransform flip;
    flip.scale(1, -1);

    latLon = flip * item->transform() * latLon;
}

void QGeoMapObjectEngine::bilinearPixelsToSeconds(const QGeoCoordinate &origin,
                                                 QGraphicsItem *item,
                                                 QPolygonF &local,
                                                 QTransform &latLon)
{
    QPointF pixelOrigin = mdp->coordinateToScreenPosition(origin.longitude(),
                                                          origin.latitude());

    QPolygonF wgs;
    foreach (const QPointF &pt, local) {
        const QGeoCoordinate coord =
                md->screenPositionToCoordinate(pt + pixelOrigin);
        const QPointF lpt(coord.longitude() * 3600.0, coord.latitude() * 3600.0);
        wgs.append(lpt);
    }

    // QTransform expects the last vertex (closing vertex) to be dropped
    local.remove(4);
    wgs.remove(4);

    // perform wrapping
    if (wgs.at(2).x() < wgs.at(3).x()) {
        QPointF topRight = wgs.at(1);
        topRight.setX(topRight.x() + 360.0 * 3600.0);
        wgs.replace(1, topRight);

        QPointF bottomRight = wgs.at(2);
        bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);
        wgs.replace(2, bottomRight);
    }

    bool ok = QTransform::quadToQuad(local, wgs, latLon);
    if (!ok) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: could not obtain a transformation matrix");
        return;
    }

    latLon = item->transform() * latLon;
}

void QGeoMapObjectEngine::bilinearSecondsToScreen(const QGeoCoordinate &origin,
                                                 QGeoMapObject *object,
                                                 QList<QPolygonF> &polys)
{
    Q_UNUSED(origin)

    QList<QTransform> latLons = latLonTrans.values(object);

    QGraphicsItem *item = graphicsItemFromMapObject(object);
    if (!item)
        return;

    // compute the transform by linearising from the lat/lon space
    foreach (QTransform latLon, latLons) {
        QTransform pixel;


        QPolygonF local = (item->boundingRect() | item->childrenBoundingRect());
        QPolygonF latLonPoly = latLon.map(local);

        QPolygonF pixelPoly = polyToScreen(latLonPoly);

        // QTransform expects the last vertex (closing vertex) to be dropped
        local.remove(4);
        pixelPoly.remove(4);

        bool ok = QTransform::quadToQuad(local, pixelPoly, pixel);
        if (!ok) {
            qWarning("QGeoMapData: bilinear transform to screen from arc-seconds "
                     "failed: could not compute transformation matrix");
            return;
        }

        pixelTrans.insertMulti(object, pixel);

        polys << pixelPoly;
    }
}

struct PathStep
{
    bool tooClose;
    QPointF pixel;
    QPainterPath::Element e;
};

void QGeoMapObjectEngine::exactPixelMap(const QGeoCoordinate &origin,
                                       QGeoMapObject *object,
                                       QList<QPolygonF> &polys)
{
    Q_UNUSED(origin)

    QList<QGraphicsItem*> latLonItems = latLonExact.values(object);

    foreach (QGraphicsItem *i, pixelExact.values(object))
        delete i;
    pixelExact.remove(object);

    double tolerance = exactMappingTolerance;
    QGeoMapRouteObject *robj = qobject_cast<QGeoMapRouteObject*>(object);
    if (robj)
        tolerance = robj->detailLevel();
    // square it
    tolerance = tolerance * tolerance;

    foreach (QGraphicsItem *latLonItem, latLonItems) {
        QGraphicsPolygonItem *polyItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(latLonItem);
        if (polyItem) {
            QPolygonF poly = polyItem->polygon();
            QPolygonF pixelPoly = polyToScreen(poly);

            QGraphicsPolygonItem *pi = polyCopy(polyItem);
            pi->setPolygon(pixelPoly);
            pixelExact.insertMulti(object, pi);
            polys << pi->boundingRect();
        }

        QGraphicsPathItem *pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(latLonItem);
        if (pathItem) {
            const QPainterPath path = pathItem->path();
            const int pathSize = path.elementCount();
            QPainterPath mpath;

            const QRectF screen = latLonViewport().boundingRect();

            QPointF lastPixelAdded;
            bool lastOutside = true;

            struct PathStep *steps = new struct PathStep[pathSize];

            for (int i = 0; i < pathSize; ++i) {
                const QPainterPath::Element e = path.elementAt(i);
                steps[i].e = e;

                double x = e.x; x /= 3600.0;
                double y = e.y; y /= 3600.0;

                const QPointF pixel = mdp->coordinateToScreenPosition(x, y);
                const QPointF deltaP = (pixel - lastPixelAdded);
                const double delta = deltaP.x() * deltaP.x() + deltaP.y() * deltaP.y();

                steps[i].pixel = pixel;

                if (!lastPixelAdded.isNull() && delta < tolerance) {
                    steps[i].tooClose = true;
                } else {
                    steps[i].tooClose = false;
                    lastPixelAdded = pixel;
                }
            }

            QPainterPath::Element em = steps[0].e;

            for (int i = 0; i < pathSize; ++i) {
                const QPainterPath::Element &e = steps[i].e;

                if (steps[i].tooClose)
                    continue;

                // guilty until proven innocent
                bool outside = true;
                if (screen.contains(e.x, e.y))
                    outside = false;
                if (lastOutside) {
                    if (em.x < screen.left() && e.x > screen.right())
                        outside = false;
                    if (em.x > screen.right() && e.x < screen.left())
                        outside = false;
                    if (em.y < screen.bottom() && e.y > screen.top())
                        outside = false;
                    if (em.y > screen.top() && e.y < screen.bottom())
                        outside = false;
                }

                // skip points not inside the screen rect
                // or attached to points inside it
                if (outside && lastOutside)
                    continue;

                // entering the screen rect
                if (!outside && lastOutside && i > 0) {
                    const QPointF lastPixel = steps[i-1].pixel;
                    mpath.moveTo(lastPixel);
                }
                lastOutside = outside;

                QPointF pixel = steps[i].pixel;

                if (e.isMoveTo())
                    mpath.moveTo(pixel);
                else
                    mpath.lineTo(pixel);

                em = e;
            }

            delete[] steps;

            QGraphicsPathItem *pi = pathCopy(pathItem);
            pi->setPath(mpath);
            pixelExact.insertMulti(object, pi);
            polys << QPolygonF(pi->boundingRect());
        }
    }
}

void QGeoMapObjectEngine::pixelShiftToScreen(const QGeoCoordinate &origin,
                                            QGeoMapObject *object,
                                            QList<QPolygonF> &polys)
{
    QGraphicsItem *item = graphicsItemFromMapObject(object);
    if (!item)
        return;

    const QRectF localRect = item->boundingRect()
                                | item->childrenBoundingRect();

    // compute the transform as an origin shift
    QList<QPointF> origins;
    origins << QPointF(origin.longitude(), origin.latitude());
    origins << QPointF(origin.longitude() + 360.0, origin.latitude());
    origins << QPointF(origin.longitude() - 360.0, origin.latitude());

    foreach (QPointF o, origins) {
        QTransform pixel = item->transform();
        QPointF pixelOrigin = mdp->coordinateToScreenPosition(o.x(), o.y());
        pixel.translate(pixelOrigin.x(), pixelOrigin.y());
        pixelTrans.insertMulti(object, pixel);
        polys << pixel.map(localRect);
    }
}

/*****************************************************************************
  Update utility functions
 *****************************************************************************/

static void _zoomDepsRecurse(QGeoMapObjectEngine *eng, QGeoMapGroupObject *group)
{
    foreach (QGeoMapObject *obj, group->childObjects()) {
        QGeoMapGroupObject *subgroup = qobject_cast<QGeoMapGroupObject*>(obj);
        if (subgroup) {
            _zoomDepsRecurse(eng, subgroup);
        } else {
            if (obj->units() == QGeoMapObject::PixelUnit) {
                eng->objectsForLatLonUpdate << obj;
                eng->objectsForPixelUpdate << obj;
            }
        }
    }
}

void QGeoMapObjectEngine::invalidateZoomDependents()
{
    if (mdp->containerObject)
        _zoomDepsRecurse(this, mdp->containerObject);
}

void QGeoMapObjectEngine::invalidatePixelsForViewport(bool updateNow)
{
    QPolygonF view = latLonViewport();

    QList<QGraphicsItem*> itemsInView;
    itemsInView = latLonScene->items(view, Qt::IntersectsItemShape,
                                     Qt::AscendingOrder);

    QSet<QGeoMapObject*> done = objectsForPixelUpdate.toSet();

    foreach (QGraphicsItem *latLonItem, itemsInView) {
        QGeoMapObject *object = latLonItems.value(latLonItem);
        Q_ASSERT(object);
        if (!done.contains(object)) {
            objectsForPixelUpdate << object;
            done.insert(object);
        }
    }

    if (updateNow)
        mdp->emitUpdateMapDisplay();
}

void QGeoMapObjectEngine::trimPixelTransforms()
{
    QPolygonF view = latLonViewport();

    QList<QGraphicsItem*> itemsInView;
    itemsInView = latLonScene->items(view, Qt::IntersectsItemShape,
                                     Qt::AscendingOrder);

    QSet<QGeoMapObject*> shouldBe;
    foreach (QGraphicsItem *latLonItem, itemsInView) {
        QGeoMapObject *object = latLonItems.value(latLonItem);
        Q_ASSERT(object);
        shouldBe.insert(object);
    }

    QList<QGraphicsItem*> itemsInPixels;
    itemsInPixels = pixelScene->items();

    QSet<QGeoMapObject*> currentlyAre;
    foreach (QGraphicsItem *pixelItem, itemsInPixels) {
        QGeoMapObject *object = pixelItems.value(pixelItem);
        Q_ASSERT(object);
        currentlyAre.insert(object);
    }

    QSet<QGeoMapObject*> excess = currentlyAre.subtract(shouldBe);
    foreach (QGeoMapObject *object, excess) {
        foreach (QGraphicsItem *item, pixelItemsRev.values(object)) {
            pixelScene->removeItem(item);
            pixelItems.remove(item);
            delete item;
        }
        pixelTrans.remove(object);
        pixelItemsRev.remove(object);
    }

    mdp->emitUpdateMapDisplay();
}

void QGeoMapObjectEngine::invalidateObject(QGeoMapObject *obj)
{
    // force the lat lon to update now
    // otherwise we can't tell if it's supposed to be on screen
    updateLatLonTransform(obj);

    const QRectF view = latLonViewport().boundingRect();

    bool needsPixelUpdate = false;
    foreach (QGraphicsItem *item, latLonItemsRev.values(obj)) {
        if (item->boundingRect().intersects(view)) {
            needsPixelUpdate = true;
            break;
        }
    }

    if (needsPixelUpdate) {
        objectsForPixelUpdate << obj;
        mdp->emitUpdateMapDisplay();
    }
}

// update the transform tables as necessary
void QGeoMapObjectEngine::updateTransforms()
{
    bool groupUpdated = false;

    foreach (QGeoMapObject *obj, objectsForLatLonUpdate) {
        QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(obj);
        if (group) {
            updateLatLonsForGroup(group);
            groupUpdated = true;
        } else {
            updateLatLonTransform(obj);
        }
    }

    objectsForLatLonUpdate.clear();

    foreach (QGeoMapObject *obj, objectsForPixelUpdate) {
        QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(obj);
        if (group) {
            updatePixelsForGroup(group);
            groupUpdated = true;
        } else {
            updatePixelTransform(obj);
        }
    }

    objectsForPixelUpdate.clear();

    if (groupUpdated)
        rebuildScenes();
}

void QGeoMapObjectEngine::updatePixelsForGroup(QGeoMapGroupObject *group)
{
    foreach (QGeoMapObject *obj, group->childObjects()) {
        QGeoMapGroupObject *subgroup = qobject_cast<QGeoMapGroupObject*>(obj);
        if (subgroup) {
            updatePixelsForGroup(subgroup);
        } else {
            updatePixelTransform(obj);
        }
    }
}

void QGeoMapObjectEngine::updateLatLonsForGroup(QGeoMapGroupObject *group)
{
    foreach (QGeoMapObject *object, group->childObjects()) {
        QGeoMapGroupObject *subGroup = qobject_cast<QGeoMapGroupObject*>(object);
        if (subGroup)
            updateLatLonsForGroup(subGroup);
        else
            updateLatLonTransform(object);
    }
}

static void addGroupToScene(QGeoMapObjectEngine *eng, QGeoMapGroupObject *group)
{
    foreach (QGeoMapObject *object, group->childObjects()) {
        QGeoMapGroupObject *subGroup = qobject_cast<QGeoMapGroupObject*>(object);
        if (subGroup) {
            addGroupToScene(eng, subGroup);
        } else {
            foreach (QGraphicsItem *i, eng->latLonItemsRev.values(object))
                eng->latLonScene->addItem(i);
            foreach (QGraphicsItem *i, eng->pixelItemsRev.values(object))
                eng->pixelScene->addItem(i);
        }
    }
}

void QGeoMapObjectEngine::rebuildScenes()
{
    foreach (QGraphicsItem *i, latLonScene->items())
        latLonScene->removeItem(i);
    foreach (QGraphicsItem *i, pixelScene->items())
        pixelScene->removeItem(i);

    delete latLonScene;
    delete pixelScene;

    latLonScene = new QGraphicsScene;
    pixelScene = new QGraphicsScene;
    pixelScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    addGroupToScene(this, mdp->containerObject);
}

/*****************************************************************************
  Actual update functions
 *****************************************************************************/

void QGeoMapObjectEngine::updateLatLonTransform(QGeoMapObject *object)
{
    QGeoCoordinate origin = object->origin();

    QGraphicsItem *item = graphicsItemFromMapObject(object);

    // skip any objects without graphicsitems
    if (!item)
        return;

    QRectF localRect = (item->boundingRect() | item->childrenBoundingRect());

    // skip any objects with invalid bounds
    if (!localRect.isValid() || localRect.isEmpty() || localRect.isNull())
        return;

    QPolygonF local = localRect * item->transform();
    QList<QPolygonF> polys;

    latLonTrans.remove(object);

    if (object->transformType() == QGeoMapObject::BilinearTransform ||
            object->units() == QGeoMapObject::PixelUnit) {
        QTransform latLon;

        if (object->units() == QGeoMapObject::MeterUnit) {
            bilinearMetersToSeconds(origin, item, local, latLon);
        } else if (object->units() == QGeoMapObject::RelativeArcSecondUnit) {
            latLon.translate(origin.longitude() * 3600.0, origin.latitude() * 3600.0);
        } else if (object->units() == QGeoMapObject::PixelUnit) {
            bilinearPixelsToSeconds(origin, item, local, latLon);
        }

        polys << latLon.map(localRect);
        latLonTrans.insertMulti(object, latLon);

        QTransform latLonWest;
        latLonWest.translate(360.0 * 3600.0, 0.0);
        latLonWest = latLon * latLonWest;

        polys << latLonWest.map(localRect);
        latLonTrans.insertMulti(object, latLonWest);

        QTransform latLonEast;
        latLonEast.translate(-360.0 * 3600.0, 0.0);
        latLonEast = latLon * latLonEast;

        polys << latLonEast.map(localRect);
        latLonTrans.insertMulti(object, latLonEast);

    } else if (object->transformType() == QGeoMapObject::ExactTransform) {
        if (object->units() == QGeoMapObject::MeterUnit) {
            if (!exactMetersToSeconds(origin, object, item, polys))
                return;
        } else if (object->units() == QGeoMapObject::AbsoluteArcSecondUnit ||
                   object->units() == QGeoMapObject::RelativeArcSecondUnit) {
            if (!exactSecondsToSeconds(origin, object, item, polys))
                return;
        } else {
            qWarning("QGeoMapData: unknown units for map object");
            return;
        }
    }

    QList<QGraphicsItem*> items = latLonItemsRev.values(object);
    if (items.size() != polys.size()) {
        foreach (QGraphicsItem *item, items) {
            latLonScene->removeItem(item);
            latLonItems.remove(item);
            delete item;
        }
        latLonItemsRev.remove(object);

        foreach (QPolygonF poly, polys) {
            QGraphicsItem *item = new QGraphicsPolygonItem(poly);
            //item->setZValue(object->zValue());
            item->setVisible(true);
            latLonItems.insert(item, object);
            latLonItemsRev.insertMulti(object, item);
            latLonScene->addItem(item);
        }
    } else {
        for (int i = 0; i < polys.size(); ++i) {
            QGraphicsItem *item = items.at(i);

            if (!item)
                continue;

            QGraphicsPolygonItem *pi = dynamic_cast<QGraphicsPolygonItem*>(item);
            Q_ASSERT(pi);
            pi->setPolygon(polys.at(i));
           // pi->setZValue(object->zValue());
        }
    }
}

void QGeoMapObjectEngine::updatePixelTransform(QGeoMapObject *object)
{
    QGeoCoordinate origin = object->origin();
    QGraphicsItem *item = graphicsItemFromMapObject(object);

    // skip any objects without graphicsitems
    if (!item)
        return;

    QRectF localRect = (item->boundingRect() | item->childrenBoundingRect());

    // skip any objects with invalid bounds
    if (!localRect.isValid() || localRect.isEmpty() || localRect.isNull())
        return;

    QList<QPolygonF> polys;

    pixelTrans.remove(object);
    if (object->transformType() == QGeoMapObject::BilinearTransform) {
        bilinearSecondsToScreen(origin, object, polys);
    } else if (object->transformType() == QGeoMapObject::ExactTransform) {
        if (object->units() == QGeoMapObject::PixelUnit) {
            pixelShiftToScreen(origin, object, polys);
        } else {
            exactPixelMap(origin, object, polys);
        }
    }

    QList<QGraphicsItem*> items = pixelItemsRev.values(object);

    if (items.size() != polys.size()) {
        foreach (QGraphicsItem *item, items) {
            pixelItems.remove(item);
            pixelScene->removeItem(item);
            delete item;
        }
        pixelItemsRev.remove(object);
        foreach (QPolygonF poly, polys) {
            QGraphicsPolygonItem *item = new QGraphicsPolygonItem(poly);
            //item->setZValue(object->zValue());
            item->setVisible(true);
            pixelItems.insert(item, object);
            pixelItemsRev.insertMulti(object, item);
            pixelScene->addItem(item);
        }
    } else {
        for (int i = 0; i < polys.size(); ++i) {
            QGraphicsItem *item = items.at(i);

            if (!item)
                continue;

            QGraphicsPolygonItem *pi = dynamic_cast<QGraphicsPolygonItem*>(item);
            Q_ASSERT(pi);
            pi->setPolygon(polys.at(i));
            //pi->setZValue(object->zValue());
        }
    }
}

QPolygonF QGeoMapObjectEngine::latLonViewport()
{
    QPolygonF view;
    QGeoBoundingBox viewport = md->viewport();
    QGeoCoordinate c, c2;
    double offset = 0.0;

    c = viewport.bottomLeft();
    view << QPointF(c.longitude() * 3600.0, c.latitude() * 3600.0);
    c2 = viewport.bottomRight();
    if (c2.longitude() < c.longitude())
        offset = 360.0 * 3600.0;
    view << QPointF(c2.longitude() * 3600.0 + offset, c2.latitude() * 3600.0);
    c = viewport.topRight();
    view << QPointF(c.longitude() * 3600.0 + offset, c.latitude() * 3600.0);
    c = viewport.topLeft();
    view << QPointF(c.longitude() * 3600.0, c.latitude() * 3600.0);

    return view;
}

QPolygonF QGeoMapObjectEngine::polyToScreen(const QPolygonF &poly)
{
    QPolygonF r;
#if QT_VERSION >= 0x040700
    r.reserve(poly.size());
#endif
    foreach (QPointF pt, poly) {
        const double x = pt.x() / 3600.0;
        const double y = pt.y() / 3600.0;
        const QPointF pixel = mdp->coordinateToScreenPosition(x, y);
        r.append(pixel);
    }
    return r;
}

QGraphicsItem* QGeoMapObjectEngine::graphicsItemFromMapObject(QGeoMapObject *object)
{
    if (!object || !object->info())
        return 0;

    QGeoTiledMapObjectInfo *tiledInfo
            = static_cast<QGeoTiledMapObjectInfo*>(object->info());

    if (tiledInfo)
        return tiledInfo->graphicsItem;

    return 0;
}

#include "moc_qgeomapobjectengine_p.cpp"

QTM_END_NAMESPACE
