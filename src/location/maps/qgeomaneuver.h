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

#ifndef QGEOMANEUVER_H
#define QGEOMANEUVER_H

#include <QtCore/QObject>
#include <QtCore/qshareddata.h>
#include <QtLocation/qlocationglobal.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QString;
class QGeoCoordinate;
class QGeoManeuverPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoManeuverPrivate, Q_LOCATION_EXPORT)

class Q_LOCATION_EXPORT QGeoManeuver
{
    Q_GADGET
    QML_VALUE_TYPE(routeManeuver)
    Q_ENUMS(InstructionDirection)

    Q_PROPERTY(bool valid READ isValid CONSTANT)
    Q_PROPERTY(QGeoCoordinate position READ position CONSTANT)
    Q_PROPERTY(QString instructionText READ instructionText CONSTANT)
    Q_PROPERTY(InstructionDirection direction READ direction CONSTANT)
    Q_PROPERTY(int timeToNextInstruction READ timeToNextInstruction CONSTANT)
    Q_PROPERTY(qreal distanceToNextInstruction READ distanceToNextInstruction CONSTANT)
    Q_PROPERTY(QGeoCoordinate waypoint READ waypoint CONSTANT)
    Q_PROPERTY(QVariantMap extendedAttributes READ extendedAttributes CONSTANT)

public:
    enum InstructionDirection {
        NoDirection,
        DirectionForward,
        DirectionBearRight,
        DirectionLightRight,
        DirectionRight,
        DirectionHardRight,
        DirectionUTurnRight,
        DirectionUTurnLeft,
        DirectionHardLeft,
        DirectionLeft,
        DirectionLightLeft,
        DirectionBearLeft
    };

    QGeoManeuver();
    QGeoManeuver(const QGeoManeuver &other) noexcept;
    QGeoManeuver(QGeoManeuver &&other) noexcept = default;
    ~QGeoManeuver();

    QGeoManeuver &operator= (const QGeoManeuver &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QGeoManeuver)

    void swap(QGeoManeuver &other) noexcept { d_ptr.swap(other.d_ptr); }

    friend inline bool operator==(const QGeoManeuver &lhs, const QGeoManeuver &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator!=(const QGeoManeuver &lhs, const QGeoManeuver &rhs) noexcept
    { return !lhs.isEqual(rhs); }

    bool isValid() const;

    void setPosition(const QGeoCoordinate &position);
    QGeoCoordinate position() const;

    void setInstructionText(const QString &instructionText);
    QString instructionText() const;

    void setDirection(InstructionDirection direction);
    InstructionDirection direction() const;

    void setTimeToNextInstruction(int secs);
    int timeToNextInstruction() const;

    void setDistanceToNextInstruction(qreal distance);
    qreal distanceToNextInstruction() const;

    void setWaypoint(const QGeoCoordinate &coordinate);
    QGeoCoordinate waypoint() const;

    void setExtendedAttributes(const QVariantMap &extendedAttributes);
    QVariantMap extendedAttributes() const;

private:
    QSharedDataPointer<QGeoManeuverPrivate> d_ptr;

    bool isEqual(const QGeoManeuver &other) const;
};

Q_DECLARE_SHARED(QGeoManeuver)

namespace QGeoManeuverForeignNamespace
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QGeoManeuver)
    QML_NAMED_ELEMENT(RouteManeuver)
}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoManeuver)
Q_DECLARE_METATYPE(QGeoManeuver::InstructionDirection)

#endif
