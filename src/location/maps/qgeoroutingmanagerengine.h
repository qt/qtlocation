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

#ifndef QGEOROUTINGMANAGERENGINE_H
#define QGEOROUTINGMANAGERENGINE_H

#include "qgeorouterequest.h"
#include "qgeoroutereply.h"

#include <QObject>
#include <QMap>

class QLocale;

QTM_BEGIN_NAMESPACE

class QGeoRoutingManagerEnginePrivate;

class Q_LOCATION_EXPORT QGeoRoutingManagerEngine : public QObject
{
    Q_OBJECT
public:
    QGeoRoutingManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    virtual ~QGeoRoutingManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QGeoRouteReply* calculateRoute(const QGeoRouteRequest& request) = 0;
    virtual QGeoRouteReply* updateRoute(const QGeoRoute &route, const QGeoCoordinate &position);

    bool supportsRouteUpdates() const;
    bool supportsAlternativeRoutes() const;
    bool supportsExcludeAreas() const;
    QGeoRouteRequest::TravelModes supportedTravelModes() const;
    QGeoRouteRequest::FeatureTypes supportedFeatureTypes() const;
    QGeoRouteRequest::FeatureWeights supportedFeatureWeights() const;
    QGeoRouteRequest::RouteOptimizations supportedRouteOptimizations() const;
    QGeoRouteRequest::SegmentDetails supportedSegmentDetails() const;
    QGeoRouteRequest::ManeuverDetails supportedManeuverDetails() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void finished(QGeoRouteReply* reply);
    void error(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString = QString());

protected:
    void setSupportsRouteUpdates(bool supported);
    void setSupportsAlternativeRoutes(bool supported);
    void setSupportsExcludeAreas(bool supported);
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
};

QTM_END_NAMESPACE

#endif
