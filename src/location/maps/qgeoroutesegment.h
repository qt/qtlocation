/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QGEOROUTESEGMENT_H
#define QGEOROUTESEGMENT_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QList>
#include <QtLocation/qlocationglobal.h>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoManeuver;
class QGeoRouteSegmentPrivate;

class Q_LOCATION_EXPORT QGeoRouteSegment
{

public:
    QGeoRouteSegment();
    QGeoRouteSegment(const QGeoRouteSegment &other);
    ~QGeoRouteSegment();

    QGeoRouteSegment &operator= (const QGeoRouteSegment &other);

    bool operator ==(const QGeoRouteSegment &other) const;
    bool operator !=(const QGeoRouteSegment &other) const;

    bool isValid() const;
    bool isLegLastSegment() const;

    void setNextRouteSegment(const QGeoRouteSegment &routeSegment);
    QGeoRouteSegment nextRouteSegment() const;

    void setTravelTime(int secs);
    int travelTime() const;

    void setDistance(qreal distance);
    qreal distance() const;

    void setPath(const QList<QGeoCoordinate> &path);
    QList<QGeoCoordinate> path() const;

    void setManeuver(const QGeoManeuver &maneuver);
    QGeoManeuver maneuver() const;

protected:
    QGeoRouteSegment(const QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> &dd);
    QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> &d();

private:
    QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> d_ptr;

    friend class QGeoRouteSegmentPrivate;
};

QT_END_NAMESPACE

#endif
