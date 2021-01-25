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

#ifndef QDECLARATIVEGEOROUTESEGMENT_H
#define QDECLARATIVEGEOROUTESEGMENT_H

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
#include <QtLocation/private/qdeclarativegeomaneuver_p.h>

#include <QtCore/QObject>
#include <QtQml/qjsvalue.h>
#include <QtLocation/QGeoRouteSegment>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoRouteSegment : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int travelTime READ travelTime CONSTANT)
    Q_PROPERTY(qreal distance READ distance CONSTANT)
    Q_PROPERTY(QJSValue path READ path CONSTANT)
    Q_PROPERTY(QDeclarativeGeoManeuver *maneuver READ maneuver CONSTANT)

public:
    explicit QDeclarativeGeoRouteSegment(QObject *parent = 0);
    QDeclarativeGeoRouteSegment(const QGeoRouteSegment &segment, QObject *parent = 0);
    ~QDeclarativeGeoRouteSegment();

    int travelTime() const;
    qreal distance() const;
    QJSValue path() const;
    QDeclarativeGeoManeuver *maneuver() const;

private:
    QGeoRouteSegment segment_;
    QDeclarativeGeoManeuver *maneuver_;
};

QT_END_NAMESPACE

#endif
