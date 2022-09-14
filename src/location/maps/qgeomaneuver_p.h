/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#ifndef QGEOMANEUVER_P_H
#define QGEOMANEUVER_P_H

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
#include <QtLocation/qgeomaneuver.h>
#include <QtPositioning/qgeocoordinate.h>

#include <QSharedData>
#include <QString>

QT_BEGIN_NAMESPACE

class QGeoManeuverPrivate : public QSharedData
{
public:
    bool equals(const QGeoManeuverPrivate &other) const;

    bool valid() const;
    void setValid(bool valid);

    QString id() const;
    void setId(const QString &id);

    QGeoCoordinate position() const;
    void setPosition(const QGeoCoordinate &position);

    QString text() const;
    void setText(const QString &text);

    QGeoManeuver::InstructionDirection direction() const;
    void setDirection(QGeoManeuver::InstructionDirection direction);

    int timeToNextInstruction() const;
    void setTimeToNextInstruction(int timeToNextInstruction);

    qreal distanceToNextInstruction() const;
    void setDistanceToNextInstruction(qreal distanceToNextInstruction);

    QGeoCoordinate waypoint() const;
    void setWaypoint(const QGeoCoordinate &waypoint);

    QVariantMap extendedAttributes() const;
    void setExtendedAttributes(const QVariantMap &extendedAttributes);

    QString m_id;
    QGeoCoordinate m_position;
    QString m_text;
    QGeoCoordinate m_waypoint;
    QVariantMap m_extendedAttributes;
    qreal m_distanceToNextInstruction = 0.0;
    QGeoManeuver::InstructionDirection m_direction = QGeoManeuver::NoDirection;
    int m_timeToNextInstruction = 0;
    bool m_valid = false;
};

QT_END_NAMESPACE

#endif
