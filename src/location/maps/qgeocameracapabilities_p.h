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

#ifndef QGEOCAMERACAPABILITIES_P_H
#define QGEOCAMERACAPABILITIES_P_H

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

#include <QtCore/QObject>
#include <QtLocation/private/qlocationglobal_p.h>

#include <QtCore/QSharedDataPointer>

QT_BEGIN_NAMESPACE

class QGeoCameraCapabilitiesPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoCameraCapabilitiesPrivate, Q_LOCATION_PRIVATE_EXPORT)

class Q_LOCATION_PRIVATE_EXPORT QGeoCameraCapabilities
{
    Q_GADGET

    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevelAt256 CONSTANT)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevelAt256 CONSTANT)
    Q_PROPERTY(qreal minimumTilt READ minimumTilt CONSTANT)
    Q_PROPERTY(qreal maximumTilt READ maximumTilt CONSTANT)
    Q_PROPERTY(qreal minimumFieldOfView READ minimumFieldOfView CONSTANT)
    Q_PROPERTY(qreal maximumFieldOfView READ maximumFieldOfView CONSTANT)

public:
    QGeoCameraCapabilities();
    QGeoCameraCapabilities(const QGeoCameraCapabilities &other) noexcept;
    QGeoCameraCapabilities(QGeoCameraCapabilities &&other) noexcept = default;
    ~QGeoCameraCapabilities();

    QGeoCameraCapabilities &operator=(const QGeoCameraCapabilities &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QGeoCameraCapabilities)

    void swap(QGeoCameraCapabilities &other) noexcept { d.swap(other.d); }

    friend inline bool operator==(const QGeoCameraCapabilities &lhs,
                                  const QGeoCameraCapabilities &rhs) noexcept
    {
        return lhs.isEqual(rhs);
    }
    friend inline bool operator!=(const QGeoCameraCapabilities &lhs,
                                  const QGeoCameraCapabilities &rhs) noexcept
    {
        return !lhs.isEqual(rhs);
    }

    void setTileSize(int tileSize);
    int tileSize() const;

    void setMinimumZoomLevel(double minimumZoomLevel);
    double minimumZoomLevel() const;
    double minimumZoomLevelAt256() const;

    void setMaximumZoomLevel(double maximumZoomLevel);
    double maximumZoomLevel() const;
    double maximumZoomLevelAt256() const;

    void setSupportsBearing(bool supportsBearing);
    bool supportsBearing() const;

    void setSupportsRolling(bool supportsRolling);
    bool supportsRolling() const;

    void setSupportsTilting(bool supportsTilting);
    bool supportsTilting() const;

    void setMinimumTilt(double minimumTilt);
    double minimumTilt() const;

    void setMaximumTilt(double maximumTilt);
    double maximumTilt() const;

    void setMinimumFieldOfView(double minimumFieldOfView);
    double minimumFieldOfView() const;

    void setMaximumFieldOfView(double maximumFieldOfView);
    double maximumFieldOfView() const;

    void setOverzoomEnabled(bool overzoomEnabled);
    bool overzoomEnabled() const;

    bool isValid() const;

private:
    QSharedDataPointer<QGeoCameraCapabilitiesPrivate> d;

    bool isEqual(const QGeoCameraCapabilities &other) const;
};

QT_END_NAMESPACE

#endif // QGEOCAMERACAPABILITIES_P_H
