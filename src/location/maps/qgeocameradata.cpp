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
#include "qgeocameradata_p.h"

#include <QVariant>
#include <QVariantAnimation>

#include <QMetaType>

#include <QWeakPointer>

#include <cmath>

QT_BEGIN_NAMESPACE

class QGeoCameraDataPrivate : public QSharedData
{
public:
    QGeoCameraDataPrivate();
    QGeoCameraDataPrivate(const QGeoCameraDataPrivate &rhs);

    QGeoCameraDataPrivate& operator = (const QGeoCameraDataPrivate &rhs);

    bool operator == (const QGeoCameraDataPrivate &rhs) const;

    QGeoCoordinate center_;
    double distance_;
    double bearing_;
    double tilt_;
    double roll_;
    double aspectRatio_;
    int zoomLevel_;
    double zoomFactor_;

    QWeakPointer<QGeoProjection> projection_;
};

QGeoCameraDataPrivate::QGeoCameraDataPrivate()
    : QSharedData(),
      center_(-27.5, 153),
      distance_(0.02),
      bearing_(0.0),
      tilt_(0.0),
      roll_(0.0),
      aspectRatio_(1.0),
      zoomLevel_(9) {}

QGeoCameraDataPrivate::QGeoCameraDataPrivate(const QGeoCameraDataPrivate &rhs)
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

QGeoCameraDataPrivate& QGeoCameraDataPrivate::operator = (const QGeoCameraDataPrivate &rhs)
{
    center_ = rhs.center_;
    distance_ = rhs.distance_;
    bearing_ = rhs.bearing_;
    tilt_ = rhs.tilt_;
    roll_ = rhs.roll_;
    aspectRatio_ = rhs.aspectRatio_;
    zoomLevel_ = rhs.zoomLevel_;
    zoomFactor_ = rhs.zoomFactor_;
    QSharedPointer<QGeoProjection> p = rhs.projection_.toStrongRef();
    if (p)
        projection_ = p.toWeakRef();
    else
        projection_.clear();

    return *this;
}

bool QGeoCameraDataPrivate::operator == (const QGeoCameraDataPrivate &rhs) const
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

QVariant cameraInterpolator(const QGeoCameraData &start,
                            const QGeoCameraData &end,
                            qreal progress)
{
    QGeoCameraData result = start;


    QSharedPointer<QGeoProjection> p = start.projection();
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

QGeoCameraData::QGeoCameraData()
    : d(new QGeoCameraDataPrivate())
{
    qRegisterMetaType<QGeoCameraData>();
    qRegisterAnimationInterpolator<QGeoCameraData>(cameraInterpolator);
    setZoomFactor(4.0);
}

QGeoCameraData::QGeoCameraData(const QGeoCameraData &other)
    : d(other.d) {}

QGeoCameraData::~QGeoCameraData()
{
}

QGeoCameraData& QGeoCameraData::operator = (const QGeoCameraData &other)
{
    d = other.d;
    return *this;
}

bool QGeoCameraData::operator == (const QGeoCameraData &rhs) const
{
    return (*(d.constData()) == *(rhs.d.constData()));
}

bool QGeoCameraData::operator != (const QGeoCameraData &other) const
{
    return !(operator==(other));
}

void QGeoCameraData::setCenter(const QGeoCoordinate &center)
{
    d->center_ = center;
}

QGeoCoordinate QGeoCameraData::center() const
{
    return d->center_;
}

void QGeoCameraData::setBearing(double bearing)
{
    d->bearing_ = bearing;
}

double QGeoCameraData::bearing() const
{
    return d->bearing_;
}

void QGeoCameraData::setTilt(double tilt)
{
    d->tilt_ = tilt;
}

double QGeoCameraData::tilt() const
{
    return d->tilt_;
}

void QGeoCameraData::setRoll(double roll)
{
    d->roll_ = roll;
}

double QGeoCameraData::roll() const
{
    return d->roll_;
}

void QGeoCameraData::setAspectRatio(double aspectRatio)
{
    d->aspectRatio_ = aspectRatio;
}

double QGeoCameraData::aspectRatio() const
{
    return d->aspectRatio_;
}

/*
  Distance and zoomLevel are only writeable for debugging purposes.
  The setters will eventually go away and then zoomFactor will be
  the way to set these.
*/

void QGeoCameraData::setDistance(double distance)
{
    d->zoomFactor_ = -1.0 * log(distance) / log(2.0);
    d->distance_ = distance;
}

double QGeoCameraData::distance() const
{
    return d->distance_;
}

void QGeoCameraData::setZoomLevel(int zoomLevel)
{
    d->zoomLevel_ = zoomLevel;
}

int QGeoCameraData::zoomLevel() const
{
    return d->zoomLevel_;
}

void QGeoCameraData::setZoomFactor(double zoomFactor)
{
    d->zoomLevel_ = floor(zoomFactor);
//    qDebug() << __FUNCTION__ << zoomFactor << d->zoomLevel_;

    // FIXME this will need some tuning

    d->distance_ = 1.0 / pow(2.0, zoomFactor);

    d->zoomFactor_ = zoomFactor;
}

double QGeoCameraData::zoomFactor() const
{
    return d->zoomFactor_;
}

void QGeoCameraData::setProjection(QSharedPointer<QGeoProjection> projection)
{
    d->projection_ = projection.toWeakRef();
}

QSharedPointer<QGeoProjection> QGeoCameraData::projection() const
{
    return d->projection_.toStrongRef();
}

QT_END_NAMESPACE
