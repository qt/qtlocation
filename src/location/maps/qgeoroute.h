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

#ifndef QGEOROUTE_H
#define QGEOROUTE_H

#include <QtLocation/QGeoRouteRequest>

#include <QtCore/QObject>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QList>

#include "QtQml/qqml.h"

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoRectangle;
class QGeoRouteSegment;

class QGeoRoutePrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoRoutePrivate, Q_LOCATION_EXPORT)
class Q_LOCATION_EXPORT QGeoRoute
{
    Q_GADGET
    QML_VALUE_TYPE(route)
    QML_STRUCTURED_VALUE

    Q_PROPERTY(QGeoRectangle bounds READ bounds CONSTANT)
    Q_PROPERTY(int travelTime READ travelTime CONSTANT)
    Q_PROPERTY(qreal distance READ distance CONSTANT)
    Q_PROPERTY(QList<QGeoRoute> routeLegs READ routeLegs CONSTANT)
    Q_PROPERTY(QVariantMap extendedAttributes READ extendedAttributes CONSTANT)
    Q_PROPERTY(int legIndex READ legIndex CONSTANT)
    Q_PROPERTY(QGeoRoute overallRoute READ overallRoute CONSTANT)

public:
    QGeoRoute();
    QGeoRoute(const QGeoRoute &other) noexcept;
    QGeoRoute(QGeoRoute &&other) noexcept = default;
    ~QGeoRoute();

    QGeoRoute &operator=(const QGeoRoute &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QGeoRoute)

    void swap(QGeoRoute &other) noexcept { d_ptr.swap(other.d_ptr); }

    friend inline bool operator==(const QGeoRoute &lhs, const QGeoRoute &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator!=(const QGeoRoute &lhs, const QGeoRoute &rhs) noexcept
    { return !lhs.isEqual(rhs); }

    void setRouteId(const QString &id);
    QString routeId() const;

    void setRequest(const QGeoRouteRequest &request);
    QGeoRouteRequest request() const;

    void setBounds(const QGeoRectangle &bounds);
    QGeoRectangle bounds() const;

    void setFirstRouteSegment(const QGeoRouteSegment &routeSegment);
    QGeoRouteSegment firstRouteSegment() const;

    void setTravelTime(int secs);
    int travelTime() const;

    void setDistance(qreal distance);
    qreal distance() const;

    void setTravelMode(QGeoRouteRequest::TravelMode mode);
    QGeoRouteRequest::TravelMode travelMode() const;

    void setPath(const QList<QGeoCoordinate> &path);
    QList<QGeoCoordinate> path() const;

    void setRouteLegs(const QList<QGeoRoute> &legs);
    QList<QGeoRoute> routeLegs() const;

    void setExtendedAttributes(const QVariantMap &extendedAttributes);
    QVariantMap extendedAttributes() const;

    void setLegIndex(int idx);
    int legIndex() const;

    void setOverallRoute(const QGeoRoute &route);
    QGeoRoute overallRoute() const;

protected:
    QExplicitlySharedDataPointer<QGeoRoutePrivate> &d();
    const QExplicitlySharedDataPointer<QGeoRoutePrivate> &const_d() const;

private:
    QExplicitlySharedDataPointer<QGeoRoutePrivate> d_ptr;
    bool isEqual(const QGeoRoute &other) const noexcept;

    friend class QDeclarativeGeoRoute;
    friend class QGeoRoutePrivate;
};

QT_END_NAMESPACE

#endif
