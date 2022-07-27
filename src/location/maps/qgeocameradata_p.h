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
#ifndef QGEOCAMERADATA_P_H
#define QGEOCAMERADATA_P_H

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

#include <QtLocation/private/qlocationglobal_p.h>

#include <QtCore/QSharedDataPointer>
#include <QtCore/QMetaType>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoCameraDataPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoCameraDataPrivate, Q_LOCATION_PRIVATE_EXPORT)

class Q_LOCATION_PRIVATE_EXPORT QGeoCameraData
{
public:
    QGeoCameraData();
    QGeoCameraData(const QGeoCameraData &other) noexcept;
    QGeoCameraData(QGeoCameraData &&other) noexcept = default;
    ~QGeoCameraData();

    QGeoCameraData &operator = (const QGeoCameraData &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QGeoCameraData)

    void swap(QGeoCameraData &other) noexcept { d.swap(other.d); }

    friend inline bool operator==(const QGeoCameraData &lhs, const QGeoCameraData &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator!=(const QGeoCameraData &lhs, const QGeoCameraData &rhs) noexcept
    { return !lhs.isEqual(rhs); }

    void setCenter(const QGeoCoordinate &coordinate);
    QGeoCoordinate center() const;

    void setBearing(double bearing);
    double bearing() const;

    void setTilt(double tilt);
    double tilt() const;

    void setRoll(double roll);
    double roll() const;

    void setFieldOfView(double fieldOfView);
    double fieldOfView() const;

    // Zoom level is intended to be relative to a tileSize of 256^2 pixels.
    // E.g., a zoom level of 0 must result in a mapWidth of 256, and so on.
    void setZoomLevel(double zoomLevel);
    double zoomLevel() const;

private:
    QSharedDataPointer<QGeoCameraDataPrivate> d;

    bool isEqual(const QGeoCameraData &other) const;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoCameraData)

#endif // QGEOCAMERADATA_P_H
