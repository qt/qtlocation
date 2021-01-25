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
    virtual QGeoRouteSegmentPrivate *clone() override;

    bool operator ==(const QGeoRouteSegmentPrivateDefault &other) const;

    virtual bool valid() const override;
    virtual void setValid(bool valid) override;

    virtual bool isLegLastSegment() const override;
    virtual void setLegLastSegment(bool lastSegment) override;

    virtual int travelTime() const override;
    virtual void setTravelTime(int travelTime) override;

    virtual qreal distance() const override;
    virtual void setDistance(qreal distance) override;

    virtual QList<QGeoCoordinate> path() const override;
    virtual void setPath(const QList<QGeoCoordinate> &path) override;

    virtual QGeoManeuver maneuver() const override;
    virtual void setManeuver(const QGeoManeuver &maneuver) override;


    bool m_valid;
    bool m_legLastSegment = false;
    int m_travelTime;
    qreal m_distance;
    QList<QGeoCoordinate> m_path;
    QGeoManeuver m_maneuver;
};

QT_END_NAMESPACE

#endif
