/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeoroutemodel_p.h"
#include "qdeclarativegeoroute_p.h"

#include <qgeoserviceprovider.h>
#include <qgeoroutingmanager.h>
#include <QtDeclarative/qdeclarativeinfo.h>

QT_BEGIN_NAMESPACE

QDeclarativeGeoRouteModel::QDeclarativeGeoRouteModel(QObject *parent)
    : QAbstractListModel(parent),
      complete_(false),
      plugin_(0),
      routeQuery_(0),
      reply_(0),
      autoUpdate_(false),
      status_(QDeclarativeGeoRouteModel::Null)
{
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(RouteRole, "routeData");
    setRoleNames(roleNames);
}

QDeclarativeGeoRouteModel::~QDeclarativeGeoRouteModel()
{
    if (!routes_.empty()) {
        qDeleteAll(routes_);
        routes_.clear();
    }
    if (reply_)
        delete reply_;
}

int QDeclarativeGeoRouteModel::count() const
{
    return routes_.count();
}

void QDeclarativeGeoRouteModel::clear()
{
    int oldCount = routes_.count();
    beginResetModel();
    qDeleteAll(routes_);
    routes_.clear();
    if (oldCount) {
       emit countChanged();
       emit routesChanged();
    }
    endResetModel();
}

void QDeclarativeGeoRouteModel::reset()
{
    clear();
    abortRequest();
    setError("");
    setStatus(QDeclarativeGeoRouteModel::Null);
}

void QDeclarativeGeoRouteModel::abortRequest()
{
    if (reply_) {
        reply_->abort();
        reply_->deleteLater();
        reply_ = 0;
    }
}

QDeclarativeGeoRoute* QDeclarativeGeoRouteModel::get(int index)
{
    if (index < 0 || index >= routes_.count()) {
        qmlInfo(this) << tr("Error, invalid index for get(): ") << index;
        return 0;
    }
    return routes_.at(index);
}

void QDeclarativeGeoRouteModel::componentComplete()
{
    complete_ = true;
    if (autoUpdate_) {
        update();
    }
}

int QDeclarativeGeoRouteModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return routes_.count();
}

QVariant QDeclarativeGeoRouteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qmlInfo(this) << tr("Error in indexing route model's data (invalid index).");
        return QVariant();
    }

    if (index.row() >= routes_.count()) {
        qmlInfo(this) << tr("Fatal error in indexing route model's data (index overflow).");
        return QVariant();
    }

    if (role == RouteRole) {
        QObject* route = routes_.at(index.row());
        return QVariant::fromValue(route);
    }
    return QVariant();
}

void QDeclarativeGeoRouteModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_ == plugin)
        return;
    reset(); // reset the model
    plugin_ = plugin;
    if (complete_)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = plugin_->sharedGeoServiceProvider();
    QGeoRoutingManager *routingManager = serviceProvider->routingManager();
    if (!routingManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support routing.");
        return;
    }
    connect(routingManager, SIGNAL(finished(QGeoRouteReply*)),
            this, SLOT(routingFinished(QGeoRouteReply*)));
    connect(routingManager, SIGNAL(error(QGeoRouteReply*,QGeoRouteReply::Error,QString)),
            this, SLOT(routingError(QGeoRouteReply*,QGeoRouteReply::Error,QString)));
}

void QDeclarativeGeoRouteModel::queryDetailsChanged()
{
    if (autoUpdate_ && complete_)
        update();
}

QDeclarativeGeoServiceProvider* QDeclarativeGeoRouteModel::plugin() const
{
    return plugin_;
}

void QDeclarativeGeoRouteModel::setQuery(QDeclarativeGeoRouteQuery* query)
{
    if (!query || query == routeQuery_)
        return;
    if (routeQuery_)
        routeQuery_->disconnect(this);
    routeQuery_ = query;
    connect(query, SIGNAL(queryDetailsChanged()), this, SLOT(queryDetailsChanged()));
    if (complete_) {
        emit queryChanged();
        if (autoUpdate_)
            update();
    }
}

QDeclarativeGeoRouteQuery* QDeclarativeGeoRouteModel::query() const
{
    return routeQuery_;
}

void QDeclarativeGeoRouteModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate_ == autoUpdate)
        return;
    autoUpdate_ = autoUpdate;
    if (complete_)
        emit autoUpdateChanged();
}

bool QDeclarativeGeoRouteModel::autoUpdate() const
{
    return autoUpdate_;
}

void QDeclarativeGeoRouteModel::setStatus(QDeclarativeGeoRouteModel::Status status)
{
    if (status_ == status)
        return;

    status_ = status;

    if (complete_)
        emit statusChanged();
}

QDeclarativeGeoRouteModel::Status QDeclarativeGeoRouteModel::status() const
{
    return status_;
}

void QDeclarativeGeoRouteModel::setError(const QString &error)
{
    if (error_ == error)
        return;

    error_ = error;

    if (complete_)
    emit errorChanged();
}

QString QDeclarativeGeoRouteModel::error() const
{
    return error_;
}

void QDeclarativeGeoRouteModel::update()
{
    if (!complete_)
        return;

    if (!plugin_) {
        qmlInfo(this) << tr("Plugin not set, cannot route.");
        return;
    }

    QGeoServiceProvider *serviceProvider = plugin_->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QGeoRoutingManager *routingManager = serviceProvider->routingManager();
    if (!routingManager) {
        qmlInfo(this) << tr("No routing manager available, cannot route.");
        return;
    }
    if (!routeQuery_) {
        qmlInfo(this) << tr("Route query not set, cannot route.");
        return;
    }
    abortRequest(); // Clear previus requests
    QGeoRouteRequest request = routeQuery_->routeRequest();
    if (request.waypoints().count() < 2) {
        qmlInfo(this) << tr("Not enough waypoints for routing.");
        return;
    }

    setError("");   // clear previous error string

    reply_ = routingManager->calculateRoute(request);
    setStatus(QDeclarativeGeoRouteModel::Loading);
    if (reply_->isFinished()) {
        if (reply_->error() == QGeoRouteReply::NoError) {
            routingFinished(reply_);
        } else {
            routingError(reply_, reply_->error(), reply_->errorString());
        }
    }
}

void QDeclarativeGeoRouteModel::routingFinished(QGeoRouteReply *reply)
{
    if (reply->error() != QGeoRouteReply::NoError) {
        return;
    }
    beginResetModel();
    int oldCount = routes_.count();
    qDeleteAll(routes_);
    // Convert routes to declarative
    routes_.clear();
    for (int i = 0; i < reply->routes().size(); ++i)
        routes_.append(new QDeclarativeGeoRoute(reply->routes().at(i), this));
    endResetModel();

    setError("");
    setStatus(QDeclarativeGeoRouteModel::Ready);

    reply->deleteLater();
    reply_ = 0;

    if (oldCount != 0 || routes_.count() != 0)
        emit routesChanged();
    if (oldCount != routes_.count())
        emit countChanged();
}

void QDeclarativeGeoRouteModel::routingError(QGeoRouteReply *reply,
                                               QGeoRouteReply::Error error,
                                               const QString &errorString)
{
    Q_UNUSED(error)
    setError(errorString);
    setStatus(QDeclarativeGeoRouteModel::Error);
    reply->deleteLater();
    reply_ = 0;
}

// RouteQuery

QDeclarativeGeoRouteQuery::QDeclarativeGeoRouteQuery(QObject *parent)
    : QObject(parent),
      complete_(false)
{
}

QDeclarativeGeoRouteQuery::~QDeclarativeGeoRouteQuery()
{
    if (!waypoints_.isEmpty())
        waypoints_.clear();
    if (!exclusions_.isEmpty())
        exclusions_.clear();
}

void QDeclarativeGeoRouteQuery::componentComplete()
{
    complete_ = true;
}

QList<int> QDeclarativeGeoRouteQuery::featureTypes()
{
    QList<int> list;

    for (int i = 0; i < request_.featureTypes().count(); i++) {
        list.append(static_cast<int>(request_.featureTypes().at(i)));
    }
    return list;
}

void QDeclarativeGeoRouteQuery::setNumberAlternativeRoutes(int numberAlternativeRoutes)
{
    if (numberAlternativeRoutes == request_.numberAlternativeRoutes())
        return;

    request_.setNumberAlternativeRoutes(numberAlternativeRoutes);

    if (complete_) {
        emit numberAlternativeRoutesChanged();
        emit queryDetailsChanged();
    }
}

int QDeclarativeGeoRouteQuery::numberAlternativeRoutes() const
{
    return request_.numberAlternativeRoutes();
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoRouteQuery::waypoints()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
                                                            0,
                                                            waypoints_append,
                                                            waypoints_count,
                                                            waypoints_at,
                                                            waypoints_clear);
}

void QDeclarativeGeoRouteQuery::waypoints_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *waypoint)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    model->addWaypoint(waypoint);
}

int QDeclarativeGeoRouteQuery::waypoints_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    return model->waypoints_.count();
}

QDeclarativeCoordinate* QDeclarativeGeoRouteQuery::waypoints_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    Q_ASSERT(index < model->waypoints_.count());
    return model->waypoints_.at(index);
}

void QDeclarativeGeoRouteQuery::waypoints_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    model->clearWaypoints();
}

QDeclarativeListProperty<QDeclarativeGeoBoundingBox> QDeclarativeGeoRouteQuery::excludedAreas()
{
    return QDeclarativeListProperty<QDeclarativeGeoBoundingBox>(this,
                                                                0,
                                                                exclusions_append,
                                                                exclusions_count,
                                                                exclusions_at,
                                                                exclusions_clear);
}

void QDeclarativeGeoRouteQuery::exclusions_append(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop, QDeclarativeGeoBoundingBox *area)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    model->addExcludedArea(area);
}

int QDeclarativeGeoRouteQuery::exclusions_count(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    return model->exclusions_.count();
}

QDeclarativeGeoBoundingBox* QDeclarativeGeoRouteQuery::exclusions_at(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop, int index)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    Q_ASSERT(index < model->exclusions_.count());
    return model->exclusions_.at(index);
}

void QDeclarativeGeoRouteQuery::exclusions_clear(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop)
{
    QDeclarativeGeoRouteQuery* model = static_cast<QDeclarativeGeoRouteQuery*>(prop->object);
    model->clearExcludedAreas();
}


Q_INVOKABLE void QDeclarativeGeoRouteQuery::addExcludedArea(QDeclarativeGeoBoundingBox* area)
{
    if (!area)
        return;
    if (!exclusions_.contains(area)) {
        connect(area, SIGNAL(bottomLeftChanged()), this, SIGNAL(queryDetailsChanged()));
        connect(area, SIGNAL(bottomRightChanged()), this, SIGNAL(queryDetailsChanged()));
        connect(area, SIGNAL(topLeftChanged()), this, SIGNAL(queryDetailsChanged()));
        connect(area, SIGNAL(topRightChanged()), this, SIGNAL(queryDetailsChanged()));
        connect(area, SIGNAL(centerChanged()), this, SIGNAL(queryDetailsChanged()));
        connect(area, SIGNAL(widthChanged()), this, SIGNAL(queryDetailsChanged()));
        connect(area, SIGNAL(heightChanged()), this, SIGNAL(queryDetailsChanged()));
    }
    exclusions_.append(area);
    if (complete_) {
        emit excludedAreasChanged();
        emit queryDetailsChanged();
    }
}

Q_INVOKABLE void QDeclarativeGeoRouteQuery::removeExcludedArea(QDeclarativeGeoBoundingBox* area)
{
    if (!area)
        return;

    int index = exclusions_.lastIndexOf(area);
    if (index == -1) {
        qmlInfo(this) << tr("Cannot remove nonexistent area.");
        return;
    }
    exclusions_.removeAt(index);
    if (!exclusions_.contains(area)) {
        area->disconnect(this);
    }
    emit excludedAreasChanged();
    emit queryDetailsChanged();
}

Q_INVOKABLE void QDeclarativeGeoRouteQuery::clearExcludedAreas()
{
    if (!exclusions_.count())
        return;
    for (int i = 0; i < exclusions_.count(); i++)
        exclusions_.at(i)->disconnect(this);
    exclusions_.clear();
    emit excludedAreasChanged();
    emit queryDetailsChanged();
}

Q_INVOKABLE void QDeclarativeGeoRouteQuery::addWaypoint(QDeclarativeCoordinate* waypoint)
{
    if (!waypoint)
        return;
    if (!waypoints_.contains(waypoint)) {
        connect (waypoint, SIGNAL(latitudeChanged(double)), this, SIGNAL(queryDetailsChanged()));
        connect (waypoint, SIGNAL(longitudeChanged(double)), this, SIGNAL(queryDetailsChanged()));
        connect (waypoint, SIGNAL(altitudeChanged(double)), this, SIGNAL(queryDetailsChanged()));
    }
    waypoints_.append(waypoint);
    if (complete_) {
        emit waypointsChanged();
        emit queryDetailsChanged();
    }
}

Q_INVOKABLE void QDeclarativeGeoRouteQuery::removeWaypoint(QDeclarativeCoordinate* waypoint)
{
    if (!waypoint)
        return;

    int index = waypoints_.lastIndexOf(waypoint);
    if (index == -1) {
        qmlInfo(this) << tr("Cannot remove nonexistent waypoint.");
        return;
    }
    waypoints_.removeAt(index);
    if (!waypoints_.contains(waypoint)) {
        waypoint->disconnect(this);
    }
    emit waypointsChanged();
    emit queryDetailsChanged();
}

Q_INVOKABLE void QDeclarativeGeoRouteQuery::clearWaypoints()
{
    if (!waypoints_.count())
        return;
    for (int i = 0; i < waypoints_.count(); i++)
        waypoints_.at(i)->disconnect(this);
    waypoints_.clear();
    emit waypointsChanged();
    emit queryDetailsChanged();
}

Q_INVOKABLE void QDeclarativeGeoRouteQuery::setFeatureWeight(FeatureType featureType, FeatureWeight featureWeight)
{
    if (featureType == NoFeature && !request_.featureTypes().isEmpty()) {
        // reset all feature types.
        QList<QGeoRouteRequest::FeatureType> featureTypes = request_.featureTypes();
        for (int i = 0; i < featureTypes.count(); ++i) {
            request_.setFeatureWeight(featureTypes.at(i), QGeoRouteRequest::NeutralFeatureWeight);
        }
        if (complete_) {
            emit featureTypesChanged();
            emit queryDetailsChanged();
        }
        return;
    }

    // Check if the weight changes, as we need to signal it
    FeatureWeight originalWeight = static_cast<FeatureWeight>(request_.featureWeight(static_cast<QGeoRouteRequest::FeatureType>(featureType)));
    if (featureWeight == originalWeight)
        return;

    request_.setFeatureWeight(static_cast<QGeoRouteRequest::FeatureType>(featureType),
                              static_cast<QGeoRouteRequest::FeatureWeight>(featureWeight));
    if (complete_ && ((originalWeight == NeutralFeatureWeight) || (featureWeight == NeutralFeatureWeight))) {
        // featureTypes should now give a different list, because the original and new weight
        // were not same, and other one was neutral weight
        emit featureTypesChanged();
        emit queryDetailsChanged();
    }
}

Q_INVOKABLE int QDeclarativeGeoRouteQuery::featureWeight(FeatureType featureType)
{
    return request_.featureWeight(static_cast<QGeoRouteRequest::FeatureType>(featureType));
}

void QDeclarativeGeoRouteQuery::setTravelModes(QDeclarativeGeoRouteQuery::TravelModes travelModes)
{
    QGeoRouteRequest::TravelModes reqTravelModes;

    if (travelModes & QDeclarativeGeoRouteQuery::CarTravel)
        reqTravelModes |= QGeoRouteRequest::CarTravel;
    if (travelModes & QDeclarativeGeoRouteQuery::PedestrianTravel)
        reqTravelModes |= QGeoRouteRequest::PedestrianTravel;
    if (travelModes & QDeclarativeGeoRouteQuery::BicycleTravel)
        reqTravelModes |= QGeoRouteRequest::BicycleTravel;
    if (travelModes & QDeclarativeGeoRouteQuery::PublicTransitTravel)
        reqTravelModes |= QGeoRouteRequest::PublicTransitTravel;
    if (travelModes & QDeclarativeGeoRouteQuery::TruckTravel)
        reqTravelModes |= QGeoRouteRequest::TruckTravel;

    if (reqTravelModes == request_.travelModes())
        return;

    request_.setTravelModes(reqTravelModes);

    if (complete_) {
        emit travelModesChanged();
        emit queryDetailsChanged();
    }
}

void QDeclarativeGeoRouteQuery::setSegmentDetail(SegmentDetail segmentDetail)
{
    if (static_cast<QGeoRouteRequest::SegmentDetail>(segmentDetail) == request_.segmentDetail())
        return;
    request_.setSegmentDetail(static_cast<QGeoRouteRequest::SegmentDetail>(segmentDetail));
    if (complete_) {
        emit segmentDetailChanged();
        emit queryDetailsChanged();
    }
}

QDeclarativeGeoRouteQuery::SegmentDetail QDeclarativeGeoRouteQuery::segmentDetail() const
{
    return static_cast<QDeclarativeGeoRouteQuery::SegmentDetail>(request_.segmentDetail());
}

void QDeclarativeGeoRouteQuery::setManeuverDetail(ManeuverDetail maneuverDetail)
{
    if (static_cast<QGeoRouteRequest::ManeuverDetail>(maneuverDetail) == request_.maneuverDetail())
        return;
    request_.setManeuverDetail(static_cast<QGeoRouteRequest::ManeuverDetail>(maneuverDetail));
    if (complete_) {
        emit maneuverDetailChanged();
        emit queryDetailsChanged();
    }
}

QDeclarativeGeoRouteQuery::ManeuverDetail QDeclarativeGeoRouteQuery::maneuverDetail() const
{
    return static_cast<QDeclarativeGeoRouteQuery::ManeuverDetail>(request_.maneuverDetail());
}

QDeclarativeGeoRouteQuery::TravelModes QDeclarativeGeoRouteQuery::travelModes() const
{
    QGeoRouteRequest::TravelModes reqTravelModes = request_.travelModes();
    QDeclarativeGeoRouteQuery::TravelModes travelModes;

    if (reqTravelModes & QGeoRouteRequest::CarTravel)
        travelModes |= QDeclarativeGeoRouteQuery::CarTravel;
    if (reqTravelModes & QGeoRouteRequest::PedestrianTravel)
        travelModes |= QDeclarativeGeoRouteQuery::PedestrianTravel;
    if (reqTravelModes & QGeoRouteRequest::BicycleTravel)
        travelModes |= QDeclarativeGeoRouteQuery::BicycleTravel;
    if (reqTravelModes & QGeoRouteRequest::PublicTransitTravel)
        travelModes |= QDeclarativeGeoRouteQuery::PublicTransitTravel;
    if (reqTravelModes & QGeoRouteRequest::TruckTravel)
        travelModes |= QDeclarativeGeoRouteQuery::TruckTravel;

    return travelModes;
}

void QDeclarativeGeoRouteQuery::setRouteOptimizations(QDeclarativeGeoRouteQuery::RouteOptimizations optimization)
{
    QGeoRouteRequest::RouteOptimizations reqOptimizations;

    if (optimization & QDeclarativeGeoRouteQuery::ShortestRoute)
        reqOptimizations |= QGeoRouteRequest::ShortestRoute;
    if (optimization & QDeclarativeGeoRouteQuery::FastestRoute)
        reqOptimizations |= QGeoRouteRequest::FastestRoute;
    if (optimization & QDeclarativeGeoRouteQuery::MostEconomicRoute)
        reqOptimizations |= QGeoRouteRequest::MostEconomicRoute;
    if (optimization & QDeclarativeGeoRouteQuery::MostScenicRoute)
        reqOptimizations |= QGeoRouteRequest::MostScenicRoute;

    if (reqOptimizations == request_.routeOptimization())
        return;

    request_.setRouteOptimization(reqOptimizations);

    if (complete_) {
        emit routeOptimizationsChanged();
        emit queryDetailsChanged();
    }
}

QDeclarativeGeoRouteQuery::RouteOptimizations QDeclarativeGeoRouteQuery::routeOptimizations() const
{
    QGeoRouteRequest::RouteOptimizations reqOptimizations = request_.routeOptimization();
    QDeclarativeGeoRouteQuery::RouteOptimizations optimization;

    if (reqOptimizations & QGeoRouteRequest::ShortestRoute)
        optimization |= QDeclarativeGeoRouteQuery::ShortestRoute;
    if (reqOptimizations & QGeoRouteRequest::FastestRoute)
        optimization |= QDeclarativeGeoRouteQuery::FastestRoute;
    if (reqOptimizations & QGeoRouteRequest::MostEconomicRoute)
        optimization |= QDeclarativeGeoRouteQuery::MostEconomicRoute;
    if (reqOptimizations & QGeoRouteRequest::MostScenicRoute)
        optimization |= QDeclarativeGeoRouteQuery::MostScenicRoute;

    return optimization;
}

QGeoRouteRequest& QDeclarativeGeoRouteQuery::routeRequest()
{
    // Bit inefficient, but waypoint and excludearea count is not big
    QList<QGeoCoordinate> waypoints;
    for (int i = 0; i < waypoints_.count(); i++)
        waypoints.append(waypoints_.at(i)->coordinate());
    QList<QGeoBoundingBox> exclusions;
    for (int i = 0; i < exclusions_.count(); i++)
        exclusions.append(exclusions_.at(i)->box());

    request_.setWaypoints(waypoints);
    request_.setExcludeAreas(exclusions);
    return request_;
}

#include "moc_qdeclarativegeoroutemodel_p.cpp"

QT_END_NAMESPACE
