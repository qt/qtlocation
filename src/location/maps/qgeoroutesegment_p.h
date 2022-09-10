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

#ifndef QGEOROUTESEGMENT_P_H
#define QGEOROUTESEGMENT_P_H

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
#include <QtLocation/qgeoroutesegment.h>


#include <QSharedData>
#include <QList>
#include <QString>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;

class Q_LOCATION_PRIVATE_EXPORT QGeoRouteSegmentPrivate : public QSharedData
{
public:
    QGeoRouteSegmentPrivate();
    QGeoRouteSegmentPrivate(const QGeoRouteSegmentPrivate &other);
    virtual ~QGeoRouteSegmentPrivate();
    virtual QGeoRouteSegmentPrivate *clone() = 0;

    bool operator ==(const QGeoRouteSegmentPrivate &other) const;

    virtual bool valid() const;
    virtual void setValid(bool valid);

    virtual bool isLegLastSegment() const;
    virtual void setLegLastSegment(bool lastSegment);

    virtual int travelTime() const;
    virtual void setTravelTime(int travelTime);

    virtual qreal distance() const;
    virtual void setDistance(qreal distance);

    virtual QList<QGeoCoordinate> path() const;
    virtual void setPath(const QList<QGeoCoordinate> &path);

    virtual QGeoManeuver maneuver() const;
    virtual void setManeuver(const QGeoManeuver &maneuver);

    virtual QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> nextRouteSegment() const;
    virtual void setNextRouteSegment(const QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> &next);

    QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> m_nextSegment;
    static QGeoRouteSegmentPrivate *get(QGeoRouteSegment &segment);

protected:
    virtual bool equals(const QGeoRouteSegmentPrivate &other) const;
};



class Q_LOCATION_PRIVATE_EXPORT QGeoRouteSegmentPrivateDefault : public QGeoRouteSegmentPrivate
{
public:
    QGeoRouteSegmentPrivateDefault();
    QGeoRouteSegmentPrivateDefault(const QGeoRouteSegmentPrivateDefault &other);
    ~QGeoRouteSegmentPrivateDefault();
    QGeoRouteSegmentPrivate *clone() override;

    bool operator ==(const QGeoRouteSegmentPrivateDefault &other) const;

    bool valid() const override;
    void setValid(bool valid) override;

    bool isLegLastSegment() const override;
    void setLegLastSegment(bool lastSegment) override;

    int travelTime() const override;
    void setTravelTime(int travelTime) override;

    qreal distance() const override;
    void setDistance(qreal distance) override;

    QList<QGeoCoordinate> path() const override;
    void setPath(const QList<QGeoCoordinate> &path) override;

    QGeoManeuver maneuver() const override;
    void setManeuver(const QGeoManeuver &maneuver) override;


    bool m_valid;
    bool m_legLastSegment = false;
    int m_travelTime;
    qreal m_distance;
    QList<QGeoCoordinate> m_path;
    QGeoManeuver m_maneuver;
};

QT_END_NAMESPACE

#endif
