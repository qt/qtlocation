/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOROUTESEGMENT_H
#define QGEOROUTESEGMENT_H

#include <QtCore/QObject>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QList>
#include <QtLocation/qlocationglobal.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoManeuver;
class QGeoRouteSegmentPrivate;

QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoRouteSegmentPrivate, Q_LOCATION_EXPORT)
class Q_LOCATION_EXPORT QGeoRouteSegment
{
    Q_GADGET
    QML_VALUE_TYPE(routeSegment)

    Q_PROPERTY(int travelTime READ travelTime CONSTANT)
    Q_PROPERTY(qreal distance READ distance CONSTANT)
    Q_PROPERTY(QList<QGeoCoordinate> path READ path CONSTANT)
    Q_PROPERTY(QGeoManeuver maneuver READ maneuver CONSTANT)
public:
    QGeoRouteSegment();
    QGeoRouteSegment(const QGeoRouteSegment &other) noexcept;
    QGeoRouteSegment(QGeoRouteSegment &&other) noexcept = default;
    ~QGeoRouteSegment();

    QGeoRouteSegment &operator=(const QGeoRouteSegment &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QGeoRouteSegment)

    void swap(QGeoRouteSegment &other) noexcept { d_ptr.swap(other.d_ptr); }

    friend inline bool operator ==(const QGeoRouteSegment &lhs, const QGeoRouteSegment &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator !=(const QGeoRouteSegment &lhs, const QGeoRouteSegment &rhs) noexcept
    { return !lhs.isEqual(rhs); }

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

private:
    QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> d_ptr;
    QGeoRouteSegment(QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> &&dd);

    bool isEqual(const QGeoRouteSegment &other) const noexcept;

    friend class QGeoRouteSegmentPrivate;
};

QT_END_NAMESPACE

#endif
