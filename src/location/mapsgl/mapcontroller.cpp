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

#include "mapcontroller.h"

#include "map.h"
#include "projection_p.h"

#include <QPointF>

#include <QVariant>
#include <QVariantAnimation>

QVariant coordinateInterpolator(const AnimatableCoordinate &start,
                                const AnimatableCoordinate &end,
                                qreal progress)
{
    AnimatableCoordinate result;

    QSharedPointer<Projection> p = start.projection();
    if (!p)
        p = end.projection();

    if (!p)
        result.setCoordinate(start.coordinate());
    else
        result.setCoordinate(p->interpolate(start.coordinate(),
                                            end.coordinate(),
                                            progress));

    result.setProjection(p);

    return QVariant::fromValue(result);
}

AnimatableCoordinate::AnimatableCoordinate() {}

AnimatableCoordinate::AnimatableCoordinate(const QGeoCoordinate &coordinate,
                     QSharedPointer<Projection> projection)
    : coordinate_(coordinate),
      projection_(projection)
{
}

QGeoCoordinate AnimatableCoordinate::coordinate() const
{
    return coordinate_;
}

void AnimatableCoordinate::setCoordinate(const QGeoCoordinate &coordinate)
{
    coordinate_ = coordinate;
}

QSharedPointer<Projection> AnimatableCoordinate::projection() const
{
    return projection_;
}

void AnimatableCoordinate::setProjection(QSharedPointer<Projection> projection)
{
    projection_ = projection;
}

MapController::MapController(Map *map, QSharedPointer<Projection> projection)
    : QObject(map),
      map_(map),
      projection_(projection)
{
    qRegisterMetaType<AnimatableCoordinate>();
    qRegisterAnimationInterpolator<AnimatableCoordinate>(coordinateInterpolator);

    oldCameraData_ = map_->cameraData();

    connect(map,
            SIGNAL(cameraDataChanged(CameraData)),
            this,
            SLOT(cameraDataChanged(CameraData)));
}

MapController::~MapController() {}

void MapController::cameraDataChanged(const CameraData &cameraData)
{
    if (oldCameraData_.center() != cameraData.center())
        emit centerChanged(AnimatableCoordinate(cameraData.center(), projection_));

    if (oldCameraData_.bearing() != cameraData.bearing())
        emit bearingChanged(cameraData.bearing());

    if (oldCameraData_.tilt() != cameraData.tilt())
        emit tiltChanged(cameraData.tilt());

    if (oldCameraData_.roll() != cameraData.roll())
        emit rollChanged(cameraData.roll());

    if (oldCameraData_.zoomFactor() != cameraData.zoomFactor())
        emit zoomChanged(cameraData.zoomFactor());

    oldCameraData_ = cameraData;
}

AnimatableCoordinate MapController::center() const
{
    return AnimatableCoordinate(map_->cameraData().center(), projection_);
}

void MapController::setCenter(const AnimatableCoordinate &center)
{
    CameraData cd = map_->cameraData();

    if (center.coordinate() == cd.center())
        return;

    cd.setCenter(center.coordinate());
    map_->setCameraData(cd);
}

qreal MapController::bearing() const
{
    return map_->cameraData().bearing();
}

void MapController::setBearing(qreal bearing)
{
    CameraData cd = map_->cameraData();

    if (bearing == cd.bearing())
        return;

    cd.setBearing(bearing);
    map_->setCameraData(cd);
}

qreal MapController::tilt() const
{
    return map_->cameraData().tilt();
}

void MapController::setTilt(qreal tilt)
{
    CameraData cd = map_->cameraData();

    if (tilt == cd.tilt())
        return;

    cd.setTilt(tilt);
    map_->setCameraData(cd);
}

qreal MapController::roll() const
{
    return map_->cameraData().roll();
}

void MapController::setRoll(qreal roll)
{
    CameraData cd = map_->cameraData();

    if (roll == cd.roll())
        return;

    cd.setRoll(roll);
    map_->setCameraData(cd);
}

qreal MapController::zoom() const
{
    return map_->cameraData().zoomFactor();
}

void MapController::setZoom(qreal zoom)
{
    CameraData cd = map_->cameraData();

    if (zoom == cd.zoomFactor())
        return;

    cd.setZoomFactor(zoom);
    map_->setCameraData(cd);
}

void MapController::pan(qreal dx, qreal dy)
{
    CameraData cd = map_->cameraData();
    QGeoCoordinate coord = map_->screenPositionToCoordinate(
                                QPointF(map_->width() / 2 + dx,
                                        map_->height() / 2 - dy));

    if (coord.isValid()) {
        cd.setCenter(coord);
        map_->setCameraData(cd);
    }
}

