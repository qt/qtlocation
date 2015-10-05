/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKGEOCOORDINATEANIMATION_P_H
#define QQUICKGEOCOORDINATEANIMATION_P_H

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

#include <QtQuick/private/qquickanimation_p.h>
#include <QtPositioning/qgeocoordinate.h>

QT_BEGIN_NAMESPACE

class QQuickGeoCoordinateAnimationPrivate;

class QQuickGeoCoordinateAnimation : public QQuickPropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickGeoCoordinateAnimation)
    Q_PROPERTY(QGeoCoordinate from READ from WRITE setFrom)
    Q_PROPERTY(QGeoCoordinate to READ to WRITE setTo)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)

public:
    enum Direction {
        Shortest,
        West,
        East
    };
    Q_ENUM(Direction)

    QQuickGeoCoordinateAnimation(QObject *parent=0);
    ~QQuickGeoCoordinateAnimation();

    QGeoCoordinate from() const;
    void setFrom(const QGeoCoordinate &);

    QGeoCoordinate to() const;
    void setTo(const QGeoCoordinate &);

    Direction direction() const;
    void setDirection(Direction direction);

Q_SIGNALS:
    void directionChanged();
};

QVariant q_coordinateInterpolator(const QGeoCoordinate &from, const QGeoCoordinate &to, qreal progress);

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickGeoCoordinateAnimation)

#endif // QQUICKCOORDINATEANIMATION_P_H
