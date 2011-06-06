/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMANEUVER_H
#define QDECLARATIVEGEOMANEUVER_H

#include <qgeomaneuver.h>

#include "qdeclarativecoordinate_p.h"

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoManeuver : public QObject
{
    Q_OBJECT
    Q_ENUMS(Direction)

    Q_PROPERTY(bool valid READ valid)
    Q_PROPERTY(QDeclarativeCoordinate* position READ position CONSTANT)
    Q_PROPERTY(QString instructionText READ instructionText CONSTANT)
    Q_PROPERTY(Direction direction READ direction CONSTANT)
    Q_PROPERTY(int timeToNextInstruction READ timeToNextInstruction CONSTANT)
    Q_PROPERTY(qreal distanceToNextInstruction READ distanceToNextInstruction CONSTANT)
    Q_PROPERTY(QDeclarativeCoordinate* waypoint READ waypoint CONSTANT)

public:
    enum Direction {
        NoDirection = QGeoManeuver::NoDirection,
        DirectionForward = QGeoManeuver::DirectionForward,
        DirectionBearRight = QGeoManeuver::DirectionBearRight,
        DirectionLightRight = QGeoManeuver::DirectionLightRight,
        DirectionRight = QGeoManeuver::DirectionRight,
        DirectionHardRight = QGeoManeuver::DirectionHardRight,
        DirectionUTurnRight = QGeoManeuver::DirectionUTurnRight,
        DirectionUTurnLeft = QGeoManeuver::DirectionUTurnLeft,
        DirectionHardLeft = QGeoManeuver::DirectionHardLeft,
        DirectionLeft = QGeoManeuver::DirectionLeft,
        DirectionLightLeft = QGeoManeuver::DirectionLightLeft,
        DirectionBearLeft = QGeoManeuver::DirectionBearLeft
    };

    QDeclarativeGeoManeuver(QObject *parent = 0);
    QDeclarativeGeoManeuver(const QGeoManeuver &maneuver, QObject *parent = 0);
    ~QDeclarativeGeoManeuver();

    bool valid() const;

    QDeclarativeCoordinate* position() const;
    QString instructionText() const;
    Direction direction() const;
    int timeToNextInstruction() const;
    qreal distanceToNextInstruction() const;
    QDeclarativeCoordinate* waypoint() const;

private:
    QGeoManeuver maneuver_;
    QDeclarativeCoordinate* position_;
    QDeclarativeCoordinate* waypoint_;
};

QTM_END_NAMESPACE

#endif
