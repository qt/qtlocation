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

#ifndef QGEOROUTINGMANAGERENGINE_H
#define QGEOROUTINGMANAGERENGINE_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QLocale>
#include <QtLocation/QGeoRouteRequest>
#include <QtLocation/QGeoRouteReply>

QT_BEGIN_NAMESPACE

class QGeoRoutingManagerEnginePrivate;

class Q_LOCATION_EXPORT QGeoRoutingManagerEngine : public QObject
{
    Q_OBJECT
public:
    explicit QGeoRoutingManagerEngine(const QVariantMap &parameters, QObject *parent = nullptr);
    virtual ~QGeoRoutingManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QGeoRouteReply *calculateRoute(const QGeoRouteRequest &request) = 0;
    virtual QGeoRouteReply *updateRoute(const QGeoRoute &route, const QGeoCoordinate &position);

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

protected:
    void setSupportedTravelModes(QGeoRouteRequest::TravelModes travelModes);
    void setSupportedFeatureTypes(QGeoRouteRequest::FeatureTypes featureTypes);
    void setSupportedFeatureWeights(QGeoRouteRequest::FeatureWeights featureWeights);
    void setSupportedRouteOptimizations(QGeoRouteRequest::RouteOptimizations optimizations);
    void setSupportedSegmentDetails(QGeoRouteRequest::SegmentDetails segmentDetails);
    void setSupportedManeuverDetails(QGeoRouteRequest::ManeuverDetails maneuverDetails);

private:
    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    QGeoRoutingManagerEnginePrivate *d_ptr;
    Q_DISABLE_COPY(QGeoRoutingManagerEngine)

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
};

QT_END_NAMESPACE

#endif
