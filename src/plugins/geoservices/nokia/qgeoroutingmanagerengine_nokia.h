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

#ifndef QGEOROUTINGMANAGER_NOKIA_H
#define QGEOROUTINGMANAGER_NOKIA_H

#include "qgeoserviceproviderplugin_nokia.h"

#include <qgeoserviceprovider.h>
#include <qgeoroutingmanagerengine.h>

QT_BEGIN_NAMESPACE

class QGeoNetworkAccessManager;
class QGeoUriProvider;

class QGeoRoutingManagerEngineNokia : public QGeoRoutingManagerEngine
{
    Q_OBJECT
public:
    QGeoRoutingManagerEngineNokia(QGeoNetworkAccessManager *networkInterface,
                                  const QVariantMap &parameters,
                                  QGeoServiceProvider::Error *error,
                                  QString *errorString);
    ~QGeoRoutingManagerEngineNokia();

    QGeoRouteReply *calculateRoute(const QGeoRouteRequest &request);
    QGeoRouteReply *updateRoute(const QGeoRoute &route, const QGeoCoordinate &position);

private Q_SLOTS:
    void routeFinished();
    void routeError(QGeoRouteReply::Error error, const QString &errorString);

private:
    QStringList calculateRouteRequestString(const QGeoRouteRequest &request);
    QStringList updateRouteRequestString(const QGeoRoute &route, const QGeoCoordinate &position);
    QString routeRequestString(const QGeoRouteRequest &request) const;
    bool checkEngineSupport(const QGeoRouteRequest &request,
                            QGeoRouteRequest::TravelModes travelModes) const;
    QString modesRequestString(const QGeoRouteRequest &request,
                               QGeoRouteRequest::TravelModes travelModes,
                               const QString &optimization) const;
    static QString trimDouble(double degree, int decimalDigits = 10);

    QGeoNetworkAccessManager *m_networkManager;
    QGeoUriProvider *m_uriProvider;
    QString m_appId;
    QString m_token;
};

QT_END_NAMESPACE

#endif
