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

#ifndef QGEOROUTINGMANAGER_H
#define QGEOROUTINGMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QLocale>
#include <QtLocation/QGeoRouteRequest>
#include <QtLocation/QGeoRouteReply>

QT_BEGIN_NAMESPACE

class QGeoRoutingManagerEngine;
class QGeoRoutingManagerPrivate;

class Q_LOCATION_EXPORT QGeoRoutingManager : public QObject
{
    Q_OBJECT

public:
    ~QGeoRoutingManager();

    QString managerName() const;
    int managerVersion() const;

    QGeoRouteReply *calculateRoute(const QGeoRouteRequest &request);
    QGeoRouteReply *updateRoute(const QGeoRoute &route, const QGeoCoordinate &position);

    QGeoRouteRequest::TravelModes supportedTravelModes() const;
    QGeoRouteRequest::FeatureTypes supportedFeatureTypes() const;
    QGeoRouteRequest::FeatureWeights supportedFeatureWeights() const;
    QGeoRouteRequest::RouteOptimizations supportedRouteOptimizations() const;
    QGeoRouteRequest::SegmentDetails supportedSegmentDetails() const;
    QGeoRouteRequest::ManeuverDetails supportedManeuverDetails() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;
    void setMeasurementSystem(QLocale::MeasurementSystem system);
    QLocale::MeasurementSystem measurementSystem() const;

Q_SIGNALS:
    void finished(QGeoRouteReply *reply);
    void error(QGeoRouteReply *reply, QGeoRouteReply::Error error, QString errorString = QString());

private:
    explicit QGeoRoutingManager(QGeoRoutingManagerEngine *engine, QObject *parent = nullptr);

    QGeoRoutingManagerPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoRoutingManager)

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
};

QT_END_NAMESPACE

#endif
