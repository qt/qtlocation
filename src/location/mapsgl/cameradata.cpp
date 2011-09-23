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
#include "cameradata.h"

#include "projection_p.h"

#include <QVariant>
#include <QVariantAnimation>

#include <QMetaType>

#include <QWeakPointer>

#include <cmath>


class CameraDataPrivate : public QSharedData
{
public:
    CameraDataPrivate();
    CameraDataPrivate(const CameraDataPrivate &rhs);

    CameraDataPrivate& operator = (const CameraDataPrivate &rhs);

    bool operator == (const CameraDataPrivate &rhs) const;

    QGeoCoordinate center_;
    double distance_;
    double bearing_;
    double tilt_;
    double roll_;
    double aspectRatio_;
    int zoomLevel_;
    double zoomFactor_;

    QWeakPointer<Projection> projection_;
};

CameraDataPrivate::CameraDataPrivate()
    : QSharedData(),
      center_(-27.5, 153),
      distance_(0.02),
      bearing_(0.0),
      tilt_(0.0),
      roll_(0.0),
      aspectRatio_(1.0),
      zoomLevel_(9) {}

CameraDataPrivate::CameraDataPrivate(const CameraDataPrivate &rhs)
    : QSharedData(rhs),
      center_(rhs.center_),
      distance_(rhs.distance_),
      bearing_(rhs.bearing_),
      tilt_(rhs.tilt_),
      roll_(rhs.roll_),
      aspectRatio_(rhs.aspectRatio_),
      zoomLevel_(rhs.zoomLevel_),
      zoomFactor_(rhs.zoomFactor_),
      projection_(rhs.projection_) {}

CameraDataPrivate& CameraDataPrivate::operator = (const CameraDataPrivate &rhs)
{
    center_ = rhs.center_;
    distance_ = rhs.distance_;
    bearing_ = rhs.bearing_;
    tilt_ = rhs.tilt_;
    roll_ = rhs.roll_;
    aspectRatio_ = rhs.aspectRatio_;
    zoomLevel_ = rhs.zoomLevel_;
    zoomFactor_ = rhs.zoomFactor_;
    QSharedPointer<Projection> p = rhs.projection_.toStrongRef();
    if (p)
        projection_ = p.toWeakRef();
    else
        projection_.clear();

    return *this;
}

bool CameraDataPrivate::operator == (const CameraDataPrivate &rhs) const
{
    return ((center_ == rhs.center_)
            && (distance_ == rhs.distance_)
            && (bearing_ == rhs.bearing_)
            && (tilt_ == rhs.tilt_)
            && (roll_ == rhs.roll_)
            && (aspectRatio_ == rhs.aspectRatio_)
            && (zoomLevel_ == rhs.zoomLevel_)
            && (zoomFactor_ == rhs.zoomFactor_));
}

QVariant cameraInterpolator(const CameraData &start,
                            const CameraData &end,
                            qreal progress)
{
    CameraData result = start;


    QSharedPointer<Projection> p = start.projection();
    if (!p)
        p = end.projection();

    if (!p)
        result.setCenter(start.center());
    else
        result.setCenter(p->interpolate(start.center(), end.center(), progress));

    double sf = 1.0 - progress;
    double ef = progress;

    result.setBearing(sf * start.bearing() + ef * end.bearing());
    result.setTilt(sf * start.tilt() + ef * end.tilt());
    result.setRoll(sf * start.roll() + ef * end.roll());
    result.setZoomFactor(sf * start.zoomFactor() + ef * end.zoomFactor());

    return QVariant::fromValue(result);
}

CameraData::CameraData()
    : d(new CameraDataPrivate())
{
    qRegisterMetaType<CameraData>();
    qRegisterAnimationInterpolator<CameraData>(cameraInterpolator);
    setZoomFactor(4.0);
}

CameraData::CameraData(const CameraData &other)
    : d(other.d) {}

CameraData::~CameraData()
{
}

CameraData& CameraData::operator = (const CameraData &other)
{
    d = other.d;
    return *this;
}

bool CameraData::operator == (const CameraData &rhs) const
{
    return (*(d.constData()) == *(rhs.d.constData()));
}

bool CameraData::operator != (const CameraData &other) const
{
    return !(operator==(other));
}

void CameraData::setCenter(const QGeoCoordinate &center)
{
    d->center_ = center;
}

QGeoCoordinate CameraData::center() const
{
    return d->center_;
}

void CameraData::setBearing(double bearing)
{
    d->bearing_ = bearing;
}

double CameraData::bearing() const
{
    return d->bearing_;
}

void CameraData::setTilt(double tilt)
{
    d->tilt_ = tilt;
}

double CameraData::tilt() const
{
    return d->tilt_;
}

void CameraData::setRoll(double roll)
{
    d->roll_ = roll;
}

double CameraData::roll() const
{
    return d->roll_;
}

void CameraData::setAspectRatio(double aspectRatio)
{
    d->aspectRatio_ = aspectRatio;
}

double CameraData::aspectRatio() const
{
    return d->aspectRatio_;
}

/*
  Distance and zoomLevel are only writeable for debugging purposes.
  The setters will eventually go away and then zoomFactor will be
  the way to set these.
*/

void CameraData::setDistance(double distance)
{
    d->zoomFactor_ = -1.0 * log(distance) / log(2.0);
    d->distance_ = distance;
}

double CameraData::distance() const
{
    return d->distance_;
}

void CameraData::setZoomLevel(int zoomLevel)
{
    d->zoomLevel_ = zoomLevel;
}

int CameraData::zoomLevel() const
{
    return d->zoomLevel_;
}

void CameraData::setZoomFactor(double zoomFactor)
{
    d->zoomLevel_ = floor(zoomFactor);
//    qDebug() << __FUNCTION__ << zoomFactor << d->zoomLevel_;

    // FIXME this will need some tuning

    d->distance_ = 1.0 / pow(2.0, zoomFactor);

    d->zoomFactor_ = zoomFactor;
}

double CameraData::zoomFactor() const
{
    return d->zoomFactor_;
}

void CameraData::setProjection(QSharedPointer<Projection> projection)
{
    d->projection_ = projection.toWeakRef();
}

QSharedPointer<Projection> CameraData::projection() const
{
    return d->projection_.toStrongRef();
}
