/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#ifndef QDECLARATIVEGEOROUTEMODEL_H
#define QDECLARATIVEGEOROUTEMODEL_H

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

#include <QObject>
#include <QAbstractListModel>
#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoRectangle>
#include <qgeorouterequest.h>
#include <qgeoroutereply.h>
#include <QtQml/qqml.h>
#include <QtQml/QQmlParserStatus>

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qdeclarativegeoserviceprovider_p.h>

#include <QtPositioning/private/qgeocoordinateobject_p.h>

QT_BEGIN_NAMESPACE

class QGeoServiceProvider;
class QGeoRoutingManager;
class QDeclarativeGeoRoute;
class QDeclarativeGeoRouteQuery;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoRouteModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(RouteModel)
    QML_ADDED_IN_VERSION(5, 0)
    Q_ENUMS(Status)
    Q_ENUMS(RouteError)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QDeclarativeGeoRouteQuery *query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(RouteError error READ error NOTIFY errorChanged)
    Q_PROPERTY(QLocale::MeasurementSystem measurementSystem READ measurementSystem WRITE setMeasurementSystem NOTIFY measurementSystemChanged)

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
        UnknownError = QGeoRouteReply::UnknownError,
        //we leave gap for future QGeoRouteReply errors

        //QGeoServiceProvider related errors start here
        UnknownParameterError = 100,
        MissingRequiredParameterError
    };

    explicit QDeclarativeGeoRouteModel(QObject *parent = nullptr);

    // From QQmlParserStatus
    void classBegin() override {}
    void componentComplete() override;

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int,QByteArray> roleNames() const override;

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider *plugin() const;

    void setQuery(QDeclarativeGeoRouteQuery *query);
    QDeclarativeGeoRouteQuery *query() const;

    void setAutoUpdate(bool autoUpdate);
    bool autoUpdate() const;

    void setMeasurementSystem(QLocale::MeasurementSystem ms);
    QLocale::MeasurementSystem measurementSystem() const;

    Status status() const;
    QString errorString() const;
    RouteError error() const;

    int count() const;
    Q_INVOKABLE QGeoRoute get(int index);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void cancel();

Q_SIGNALS:
    void countChanged();
    void pluginChanged();
    void queryChanged();
    void autoUpdateChanged();
    void statusChanged();
    void errorChanged(); //emitted also for errorString notification
    void routesChanged();
    void measurementSystemChanged();
    void abortRequested();

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
    void setError(RouteError error, const QString &errorString);

    bool complete_ = false;

    QDeclarativeGeoServiceProvider *plugin_ = nullptr;
    QDeclarativeGeoRouteQuery *routeQuery_ = nullptr;

    QList<QGeoRoute> routes_;
    bool autoUpdate_ = false;
    Status status_ = QDeclarativeGeoRouteModel::Null;
    QString errorString_;
    RouteError error_ = QDeclarativeGeoRouteModel::NoError;
};



// purpose of this class is to be convertible to a QGeoCoordinate (through QGeoWaypoint), but also
// to behave like it, so that in QML source compatibility would be preserved. This is, however, not possible to achieve at the present.
class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoWaypoint : public QGeoCoordinateObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Waypoint)
    QML_ADDED_IN_VERSION(5, 11)

    Q_PROPERTY(double latitude READ latitude WRITE setLatitude STORED false)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude STORED false)
    Q_PROPERTY(double altitude READ altitude WRITE setAltitude STORED false)
    Q_PROPERTY(bool isValid READ isValid STORED false)

    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    QDeclarativeGeoWaypoint(QObject *parent = nullptr);
    virtual ~QDeclarativeGeoWaypoint();

    bool operator==(const QDeclarativeGeoWaypoint &other) const;

    qreal latitude() const;
    void setLatitude(qreal latitude);

    qreal longitude() const;
    void setLongitude(qreal longitude);

    qreal altitude() const;
    void setAltitude(qreal altitude);

    bool isValid() const;

    qreal bearing() const;
    void setBearing(qreal bearing);

Q_SIGNALS:
    void completed();
    void waypointDetailsChanged();
    void bearingChanged();

protected:
    // From QQmlParserStatus
    void classBegin() override {}
    void componentComplete() override { m_complete = true; emit completed(); }

    // other data members
    bool m_complete = false;

    qreal m_bearing = Q_QNAN;
};




class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoRouteQuery : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(RouteQuery)
    QML_ADDED_IN_VERSION(5, 0)
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
    Q_PROPERTY(QVariantList waypoints READ waypoints WRITE setWaypoints NOTIFY waypointsChanged)
    Q_PROPERTY(QList<QGeoRectangle> excludedAreas READ excludedAreas WRITE setExcludedAreas NOTIFY excludedAreasChanged)
    Q_PROPERTY(QList<int> featureTypes READ featureTypes NOTIFY featureTypesChanged)
    Q_PROPERTY(QVariantMap extraParameters READ extraParameters REVISION(5, 11))
    Q_PROPERTY(QDateTime departureTime READ departureTime WRITE setDepartureTime NOTIFY departureTimeChanged REVISION(5, 13))
    Q_INTERFACES(QQmlParserStatus)

public:

    explicit QDeclarativeGeoRouteQuery(QObject *parent = nullptr);
    QDeclarativeGeoRouteQuery(const QGeoRouteRequest &request, QObject *parent = nullptr); // init from request. For instances intended to be read only
    ~QDeclarativeGeoRouteQuery();

    // From QQmlParserStatus
    void classBegin() override {}
    void componentComplete() override;

    QGeoRouteRequest routeRequest() const;
    QVariantMap extraParameters() const;

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
        MotorPoolLaneFeature = QGeoRouteRequest::MotorPoolLaneFeature,
        TrafficFeature = QGeoRouteRequest::TrafficFeature
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
    QList<int> featureTypes() const;


    QVariantList waypoints() const;
    Q_INVOKABLE QVariantList waypointObjects() const;
    void setWaypoints(const QVariantList &value);

    QList<QGeoRectangle> excludedAreas() const;
    void setExcludedAreas(const QList<QGeoRectangle> &value);

    Q_INVOKABLE void addWaypoint(const QVariant &w);
    Q_INVOKABLE void removeWaypoint(const QVariant &waypoint);
    Q_INVOKABLE void clearWaypoints();
    void flushWaypoints(QList<QDeclarativeGeoWaypoint *> &waypoints);

    Q_INVOKABLE void addExcludedArea(const QGeoRectangle &area);
    Q_INVOKABLE void removeExcludedArea(const QGeoRectangle &area);
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

    void setDepartureTime(const QDateTime &departureTime);
    QDateTime departureTime() const;

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
    Q_REVISION(11) void extraParametersChanged();
    void departureTimeChanged();

private Q_SLOTS:
    void excludedAreaCoordinateChanged();
    void waypointChanged();

private:
    Q_INVOKABLE void doCoordinateChanged();

    mutable QGeoRouteRequest request_;
    bool complete_ = false;
    bool m_excludedAreaCoordinateChanged = false;
    mutable bool m_waypointsChanged = false;
    QList<QDeclarativeGeoWaypoint *> m_waypoints;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeGeoWaypoint*)

#endif
