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

#ifndef QGEOMANEUVER_H
#define QGEOMANEUVER_H

#include <QtCore/qshareddata.h>
#include <QtLocation/qlocationglobal.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QString;

class QGeoCoordinate;
class QGeoManeuverPrivate;

class Q_LOCATION_EXPORT QGeoManeuver
{

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
    QGeoManeuver(const QGeoManeuver &other);
    ~QGeoManeuver();

    QGeoManeuver &operator= (const QGeoManeuver &other);

    bool operator== (const QGeoManeuver &other) const;
    bool operator!= (const QGeoManeuver &other) const;

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

protected:
    QGeoManeuver(const QSharedDataPointer<QGeoManeuverPrivate> &dd);

private:
    QSharedDataPointer<QGeoManeuverPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif
