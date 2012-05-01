/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOROUTEMODEL_H
#define QDECLARATIVEGEOROUTEMODEL_H

#include "qdeclarativegeoserviceprovider_p.h"

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"

#include <qgeorouterequest.h>
#include <qgeoroutereply.h>

#include <QtQml/qqml.h>
#include <QtQml/QQmlParserStatus>
#include <QAbstractListModel>

#include <QObject>

QT_BEGIN_NAMESPACE

class QGeoServiceProvider;
class QGeoRoutingManager;
class QDeclarativeGeoRoute;
class QDeclarativeGeoRouteQuery;

class QDeclarativeGeoRouteModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(RouteError)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QDeclarativeGeoRouteQuery* query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(RouteError error READ error NOTIFY errorChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    enum Roles {
        RouteRole = Qt::UserRole + 500
    };

    enum Status {
        Null,
        Ready,
        Loading,
        Error
    };

    enum RouteError {
        NoError = QGeoRouteReply::NoError,
        EngineNotSetError = QGeoRouteReply::EngineNotSetError,
        CommunicationError = QGeoRouteReply::CommunicationError,
        ParseError = QGeoRouteReply::ParseError,
        UnsupportedOptionError = QGeoRouteReply::UnsupportedOptionError,
        UnknownError = QGeoRouteReply::UnknownError
    };

    QDeclarativeGeoRouteModel(QObject *parent = 0);
    ~QDeclarativeGeoRouteModel();

    // From QQmlParserStatus
    void classBegin() {}
    void componentComplete();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    void setQuery(QDeclarativeGeoRouteQuery* query);
    QDeclarativeGeoRouteQuery* query() const;

    void setAutoUpdate(bool autoUpdate);
    bool autoUpdate() const;

    Status status() const;
    QString errorString() const;
    RouteError error() const;

    int count() const;
    Q_INVOKABLE QDeclarativeGeoRoute* get(int index);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void cancel();

Q_SIGNALS:
    void countChanged();
    void pluginChanged();
    void queryChanged();
    void autoUpdateChanged();
    void statusChanged();
    void errorStringChanged();
    void errorChanged();
    void routesChanged();

public Q_SLOTS:
    void update();

private Q_SLOTS:
    void routingFinished(QGeoRouteReply *reply);
    void routingError(QGeoRouteReply *reply,
                      QGeoRouteReply::Error error,
                      const QString &errorString);
    void queryDetailsChanged();
    void pluginReady();

private:
    void setStatus(Status status);
    void setErrorString(const QString &error);
    void setError(RouteError error);
    void abortRequest();

    bool complete_;

    QDeclarativeGeoServiceProvider* plugin_;
    QDeclarativeGeoRouteQuery* routeQuery_;
    QGeoRouteReply* reply_;

    QList<QDeclarativeGeoRoute*> routes_;
    bool autoUpdate_;
    Status status_;
    QString errorString_;
    RouteError error_;
};

class QDeclarativeGeoRouteQuery : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(TravelMode)
    Q_ENUMS(FeatureType)
    Q_ENUMS(FeatureWeight)
    Q_ENUMS(SegmentDetail)
    Q_ENUMS(ManeuverDetail)
    Q_ENUMS(RouteOptimization)
    Q_FLAGS(RouteOptimizations)
    Q_FLAGS(ManeuverDetails)
    Q_FLAGS(SegmentDetails)
    Q_FLAGS(TravelModes)

    Q_PROPERTY(int numberAlternativeRoutes READ numberAlternativeRoutes WRITE setNumberAlternativeRoutes NOTIFY numberAlternativeRoutesChanged)
    Q_PROPERTY(TravelModes travelModes READ travelModes WRITE setTravelModes NOTIFY travelModesChanged)
    Q_PROPERTY(RouteOptimizations routeOptimizations READ routeOptimizations WRITE setRouteOptimizations NOTIFY routeOptimizationsChanged)
    Q_PROPERTY(SegmentDetail segmentDetail READ segmentDetail WRITE setSegmentDetail NOTIFY segmentDetailChanged)
    Q_PROPERTY(ManeuverDetail maneuverDetail READ maneuverDetail WRITE setManeuverDetail NOTIFY maneuverDetailChanged)
    Q_PROPERTY(QQmlListProperty<QDeclarativeCoordinate> waypoints READ waypoints NOTIFY waypointsChanged)
    Q_PROPERTY(QQmlListProperty<QDeclarativeGeoBoundingBox> excludedAreas READ excludedAreas NOTIFY excludedAreasChanged)
    Q_PROPERTY(QList<int> featureTypes READ featureTypes NOTIFY featureTypesChanged)
    Q_INTERFACES(QQmlParserStatus)

public:

    QDeclarativeGeoRouteQuery(QObject *parent = 0);
    ~QDeclarativeGeoRouteQuery();

    // From QQmlParserStatus
    void classBegin() {}
    void componentComplete();

    QGeoRouteRequest& routeRequest();

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

    enum SegmentDetail {
        NoSegmentData = 0x0000,
        BasicSegmentData = 0x0001
    };
    Q_DECLARE_FLAGS(SegmentDetails, SegmentDetail)

    enum ManeuverDetail {
        NoManeuvers = 0x0000,
        BasicManeuvers = 0x0001
    };
    Q_DECLARE_FLAGS(ManeuverDetails, ManeuverDetail)

    void setNumberAlternativeRoutes(int numberAlternativeRoutes);
    int numberAlternativeRoutes() const;

    //QList<FeatureType> featureTypes();
    QList<int> featureTypes();

    // READ functions for list properties
    QQmlListProperty<QDeclarativeCoordinate> waypoints();
    QQmlListProperty<QDeclarativeGeoBoundingBox> excludedAreas();

    Q_INVOKABLE void addWaypoint(QDeclarativeCoordinate* waypoint);
    Q_INVOKABLE void removeWaypoint(QDeclarativeCoordinate* waypoint);
    Q_INVOKABLE void clearWaypoints();

    Q_INVOKABLE void addExcludedArea(QDeclarativeGeoBoundingBox* area);
    Q_INVOKABLE void removeExcludedArea(QDeclarativeGeoBoundingBox* area);
    Q_INVOKABLE void clearExcludedAreas();

    Q_INVOKABLE void setFeatureWeight(FeatureType featureType, FeatureWeight featureWeight);
    Q_INVOKABLE int featureWeight(FeatureType featureType);
    Q_INVOKABLE void resetFeatureWeights();

    /*
    feature weights
    */

    void setTravelModes(TravelModes travelModes);
    TravelModes travelModes() const;

    void setSegmentDetail(SegmentDetail segmentDetail);
    SegmentDetail segmentDetail() const;

    void setManeuverDetail(ManeuverDetail maneuverDetail);
    ManeuverDetail maneuverDetail() const;

    void setRouteOptimizations(RouteOptimizations optimization);
    RouteOptimizations routeOptimizations() const;

Q_SIGNALS:
    void numberAlternativeRoutesChanged();
    void travelModesChanged();
    void routeOptimizationsChanged();

    void waypointsChanged();
    void excludedAreasChanged();

    void featureTypesChanged();
    void maneuverDetailChanged();
    void segmentDetailChanged();

    void queryDetailsChanged();

private Q_SLOTS:
    void waypointDestroyed(QObject* object);

private:

    static void waypoints_append(QQmlListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *waypoint);
    static int waypoints_count(QQmlListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* waypoints_at(QQmlListProperty<QDeclarativeCoordinate> *prop, int index);
    static void waypoints_clear(QQmlListProperty<QDeclarativeCoordinate> *prop);

    static void exclusions_append(QQmlListProperty<QDeclarativeGeoBoundingBox> *prop, QDeclarativeGeoBoundingBox *area);
    static int exclusions_count(QQmlListProperty<QDeclarativeGeoBoundingBox> *prop);
    static QDeclarativeGeoBoundingBox* exclusions_at(QQmlListProperty<QDeclarativeGeoBoundingBox> *prop, int index);
    static void exclusions_clear(QQmlListProperty<QDeclarativeGeoBoundingBox> *prop);

    QList<QDeclarativeCoordinate*> waypoints_;
    QList<QDeclarativeGeoBoundingBox*> exclusions_;

    QGeoRouteRequest request_;
    bool complete_;

};

QT_END_NAMESPACE

#endif
