/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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
**
**
**
****************************************************************************/

#ifndef LOCATIONSINGLETON_H
#define LOCATIONSINGLETON_H

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
#include <QtCore/qnumeric.h>
#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoShape>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoPath>
#include <QtPositioning/QGeoPolygon>
#include <QtQml/QJSValue>
#include <QVariant>
#include <QPointF>

class LocationSingleton : public QObject
{
    Q_OBJECT

public:
    explicit LocationSingleton(QObject *parent = 0);

    Q_INVOKABLE QGeoCoordinate coordinate() const;
    Q_INVOKABLE QGeoCoordinate coordinate(double latitude, double longitude,
                                          double altitude = qQNaN()) const;

    Q_INVOKABLE QGeoShape shape() const;

    Q_INVOKABLE QGeoRectangle rectangle() const;
    Q_INVOKABLE QGeoRectangle rectangle(const QGeoCoordinate &center,
                                        double width, double height) const;
    Q_INVOKABLE QGeoRectangle rectangle(const QGeoCoordinate &topLeft,
                                        const QGeoCoordinate &bottomRight) const;
    Q_INVOKABLE QGeoRectangle rectangle(const QVariantList &coordinates) const;

    Q_INVOKABLE QGeoCircle circle() const;
    Q_INVOKABLE QGeoCircle circle(const QGeoCoordinate &center, qreal radius = -1.0) const;

    Q_INVOKABLE QGeoPath path() const;
    Q_INVOKABLE QGeoPath path(const QJSValue &value, qreal width = 0.0) const;

    Q_INVOKABLE QGeoPolygon polygon() const;
    Q_INVOKABLE QGeoPolygon polygon(const QVariantList &value) const;
    Q_INVOKABLE QGeoPolygon polygon(const QVariantList &perimeter, const QVariantList &holes) const;

    Q_INVOKABLE QGeoCircle shapeToCircle(const QGeoShape &shape) const;
    Q_INVOKABLE QGeoRectangle shapeToRectangle(const QGeoShape &shape) const;
    Q_INVOKABLE QGeoPath shapeToPath(const QGeoShape &shape) const;
    Q_INVOKABLE QGeoPolygon shapeToPolygon(const QGeoShape &shape) const;

    Q_REVISION(12) Q_INVOKABLE QGeoCoordinate mercatorToCoord(const QPointF &mercator) const;
    Q_REVISION(12) Q_INVOKABLE QPointF coordToMercator(const QGeoCoordinate &coord) const;
};

#endif // LOCATIONSINGLETON_H
