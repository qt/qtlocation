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

#ifndef QDECLARATIVEGEOROUTINGMODEL_H
#define QDECLARATIVEGEOROUTINGMODEL_H

#include "qdeclarativegeoserviceprovider_p.h"

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"

#include <qgeorouterequest.h>
#include <qgeoroutereply.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeParserStatus>
#include <QAbstractListModel>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QGeoServiceProvider;
class QGeoRoutingManager;
class QDeclarativeGeoRoute;

class QDeclarativeGeoRoutingModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(TravelMode)
    Q_ENUMS(FeatureType)
    Q_ENUMS(FeatureWeight)
    Q_ENUMS(RouteOptimization)
    Q_FLAGS(RouteOptimizations)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    enum Status {
        Null,
        Ready,
        Loading,
        Error
    };

    enum TravelMode {
        CarTravel = QGeoRouteRequest::CarTravel,
        PedestrianTravel = QGeoRouteRequest::PedestrianTravel,
        BicycleTravel = QGeoRouteRequest::BicycleTravel,
        PublicTransitTravel = QGeoRouteRequest::PublicTransitTravel,
        TruckTravel = QGeoRouteRequest::TruckTravel
    };
    Q_DECLARE_FLAGS(TravelModes, TravelMode)

    enum FeatureType {
        NoFeature = QGeoRouteRequest::NoFeature,
        TollFeature = QGeoRouteRequest::TollFeature,
        HighwayFeature = QGeoRouteRequest::HighwayFeature,
        PublicTransitFeature = QGeoRouteRequest::PublicTransitFeature,
        FerryFeature = QGeoRouteRequest::FerryFeature,
        TunnelFeature = QGeoRouteRequest::TunnelFeature,
        DirtRoadFeature = QGeoRouteRequest::DirtRoadFeature,
        ParksFeature = QGeoRouteRequest::ParksFeature,
        MotorPoolLaneFeature = QGeoRouteRequest::MotorPoolLaneFeature
    };
    Q_DECLARE_FLAGS(FeatureTypes, FeatureType)

    enum FeatureWeight {
        NeutralFeatureWeight = QGeoRouteRequest::NeutralFeatureWeight,
        PreferFeatureWeight = QGeoRouteRequest::PreferFeatureWeight,
        RequireFeatureWeight = QGeoRouteRequest::RequireFeatureWeight,
        AvoidFeatureWeight = QGeoRouteRequest::AvoidFeatureWeight,
        DisallowFeatureWeight = QGeoRouteRequest::DisallowFeatureWeight
    };
    Q_DECLARE_FLAGS(FeatureWeights, FeatureWeight)

    enum RouteOptimization {
        ShortestRoute = QGeoRouteRequest::ShortestRoute,
        FastestRoute = QGeoRouteRequest::FastestRoute,
        MostEconomicRoute = QGeoRouteRequest::MostEconomicRoute,
        MostScenicRoute = QGeoRouteRequest::MostScenicRoute
    };
    Q_DECLARE_FLAGS(RouteOptimizations, RouteOptimization)


    QDeclarativeGeoRoutingModel(QObject *parent = 0);
    ~QDeclarativeGeoRoutingModel();

    // From QDeclarativeParserStatus
    void classBegin();
    void componentComplete();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    void setAutoUpdate(bool autoUpdate);
    bool autoUpdate() const;

    void setNumberAlternativeRoutes(int numberAlternativeRoutes);
    int numberAlternativeRoutes() const;

    QDeclarativeListProperty<QDeclarativeCoordinate> waypoints();
    QDeclarativeListProperty<QDeclarativeGeoBoundingBox> excludeAreas();

    /*
    feature weights
    */

    void setTravelModes(TravelModes travelModes);
    TravelModes travelModes() const;

    void setRouteOptimization(RouteOptimizations optimization);
    RouteOptimizations routeOptimization() const;

    Status status() const;
    QString error() const;

Q_SIGNALS:
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void autoUpdateChanged(bool autoUpdate);

    void numberAlternativeRoutesChanged(int numberAlternativeRoutes);
    void travelModesChanged(QDeclarativeGeoRoutingModel::TravelModes travelModes);
    void routeOptimizationChanged(QDeclarativeGeoRoutingModel::RouteOptimizations optimization);

    void statusChanged(QDeclarativeGeoRoutingModel::Status status);
    void errorChanged(const QString &error);
    void routesChanged();

public Q_SLOTS:
    void update();

private Q_SLOTS:
    void routingFinished(QGeoRouteReply *reply);
    void routingError(QGeoRouteReply *reply,
                      QGeoRouteReply::Error error,
                      const QString &errorString);

private:
    static void waypoints_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *waypoint);
    static int waypoints_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* waypoints_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index);
    static void waypoints_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);

    static void exclusions_append(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop, QDeclarativeGeoBoundingBox *area);
    static int exclusions_count(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop);
    static QDeclarativeGeoBoundingBox* exclusions_at(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop, int index);
    static void exclusions_clear(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop);

    void setStatus(Status status);
    void setError(const QString &error);

    bool complete_;

    QDeclarativeGeoServiceProvider* plugin_;
    QGeoServiceProvider* serviceProvider_;
    QGeoRoutingManager* routingManager_;

    QGeoRouteRequest request_;

    bool autoUpdate_;
    Status status_;
    QString error_;
    QList<QDeclarativeGeoRoute*> routes_;
};


QTM_END_NAMESPACE

#endif
