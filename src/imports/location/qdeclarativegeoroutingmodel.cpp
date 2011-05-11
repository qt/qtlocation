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

#include "qdeclarativegeoroutingmodel_p.h"
#include "qdeclarativegeoroute_p.h"

#include <qgeoserviceprovider.h>
#include <qgeoroutingmanager.h>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoRoutingModel::QDeclarativeGeoRoutingModel(QObject *parent)
    : QAbstractListModel(parent),
      complete_(false),
      plugin_(0),
      serviceProvider_(0),
      routingManager_(0),
      autoUpdate_(false),
      status_(QDeclarativeGeoRoutingModel::Null)
{
}

QDeclarativeGeoRoutingModel::~QDeclarativeGeoRoutingModel()
{
    if (serviceProvider_)
        delete serviceProvider_;
}

void QDeclarativeGeoRoutingModel::classBegin()
{
}

void QDeclarativeGeoRoutingModel::componentComplete()
{
    if (!routingManager_)
        return;

    complete_ = true;
}

int QDeclarativeGeoRoutingModel::rowCount(const QModelIndex &parent) const
{
    return routes_.count();
}

QVariant QDeclarativeGeoRoutingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > routes_.count())
        return QVariant();

    return QVariant();
}

QVariant QDeclarativeGeoRoutingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section != 0)
        return QVariant();

    return QVariant();
}

void QDeclarativeGeoRoutingModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_ || !plugin)
        return;

    plugin_ = plugin;

    emit pluginChanged(plugin_);

    serviceProvider_ = new QGeoServiceProvider(plugin_->name(),
                                               plugin_->parameterMap());

    // check for error

    routingManager_ = serviceProvider_->routingManager();

    connect(routingManager_,
            SIGNAL(finished(QGeoRoutReply*)),
            this,
            SLOT(routingFinished(QGeoRouteReply*)));

    connect(routingManager_,
            SIGNAL(error(QGeoRouteReply*, QGeoRouteReply::Error, QString)),
            this,
            SLOT(routingError(QGeoRouteReply*, QGeoRouteReply::Error, QString)));

}

QDeclarativeGeoServiceProvider* QDeclarativeGeoRoutingModel::plugin() const
{
    return plugin_;
}

void QDeclarativeGeoRoutingModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate_ == autoUpdate)
        return;

    autoUpdate_ = autoUpdate;

    emit autoUpdateChanged(autoUpdate_);
}

bool QDeclarativeGeoRoutingModel::autoUpdate() const
{
    return autoUpdate_;
}

void QDeclarativeGeoRoutingModel::setNumberAlternativeRoutes(int numberAlternativeRoutes)
{
    if (numberAlternativeRoutes == request_.numberAlternativeRoutes())
        return;

    request_.setNumberAlternativeRoutes(numberAlternativeRoutes);

    emit numberAlternativeRoutesChanged(numberAlternativeRoutes);

    if (autoUpdate_ && complete_)
        update();
}

int QDeclarativeGeoRoutingModel::numberAlternativeRoutes() const
{
    return request_.numberAlternativeRoutes();
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoRoutingModel::waypoints()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
                                                            0,
                                                            waypoints_append,
                                                            waypoints_count,
                                                            waypoints_at,
                                                            waypoints_clear);
}

void QDeclarativeGeoRoutingModel::waypoints_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *waypoint)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
}

int QDeclarativeGeoRoutingModel::waypoints_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
    return 0;
}

QDeclarativeCoordinate* QDeclarativeGeoRoutingModel::waypoints_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
    return 0;
}

void QDeclarativeGeoRoutingModel::waypoints_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
}

QDeclarativeListProperty<QDeclarativeGeoBoundingBox> QDeclarativeGeoRoutingModel::excludeAreas()
{
    return QDeclarativeListProperty<QDeclarativeGeoBoundingBox>(this,
                                                                0,
                                                                exclusions_append,
                                                                exclusions_count,
                                                                exclusions_at,
                                                                exclusions_clear);
}

void QDeclarativeGeoRoutingModel::exclusions_append(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop, QDeclarativeGeoBoundingBox *area)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
}

int QDeclarativeGeoRoutingModel::exclusions_count(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
    return 0;
}

QDeclarativeGeoBoundingBox* QDeclarativeGeoRoutingModel::exclusions_at(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop, int index)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
    return 0;
}

void QDeclarativeGeoRoutingModel::exclusions_clear(QDeclarativeListProperty<QDeclarativeGeoBoundingBox> *prop)
{
//    static_cast<QDeclarativeGeoRoutingModel>(prop->object)
}

void QDeclarativeGeoRoutingModel::setTravelModes(QDeclarativeGeoRoutingModel::TravelModes travelModes)
{
    QGeoRouteRequest::TravelModes reqTravelModes;

    if (travelModes & QDeclarativeGeoRoutingModel::CarTravel)
        reqTravelModes |= QGeoRouteRequest::CarTravel;
    if (travelModes & QDeclarativeGeoRoutingModel::PedestrianTravel)
        reqTravelModes |= QGeoRouteRequest::PedestrianTravel;
    if (travelModes & QDeclarativeGeoRoutingModel::BicycleTravel)
        reqTravelModes |= QGeoRouteRequest::BicycleTravel;
    if (travelModes & QDeclarativeGeoRoutingModel::PublicTransitTravel)
        reqTravelModes |= QGeoRouteRequest::PublicTransitTravel;
    if (travelModes & QDeclarativeGeoRoutingModel::TruckTravel)
        reqTravelModes |= QGeoRouteRequest::TruckTravel;

    if (reqTravelModes == request_.travelModes())
        return;

    request_.setTravelModes(reqTravelModes);

    emit travelModesChanged(travelModes);

    if (autoUpdate_ && complete_)
        update();
}

QDeclarativeGeoRoutingModel::TravelModes QDeclarativeGeoRoutingModel::travelModes() const
{
    QGeoRouteRequest::TravelModes reqTravelModes = request_.travelModes();
    QDeclarativeGeoRoutingModel::TravelModes travelModes;

    if (reqTravelModes & QGeoRouteRequest::CarTravel)
        travelModes |= QDeclarativeGeoRoutingModel::CarTravel;
    if (reqTravelModes & QGeoRouteRequest::PedestrianTravel)
        travelModes |= QDeclarativeGeoRoutingModel::PedestrianTravel;
    if (reqTravelModes & QGeoRouteRequest::BicycleTravel)
        travelModes |= QDeclarativeGeoRoutingModel::BicycleTravel;
    if (reqTravelModes & QGeoRouteRequest::PublicTransitTravel)
        travelModes |= QDeclarativeGeoRoutingModel::PublicTransitTravel;
    if (reqTravelModes & QGeoRouteRequest::TruckTravel)
        travelModes |= QDeclarativeGeoRoutingModel::TruckTravel;

    return travelModes;
}

void QDeclarativeGeoRoutingModel::setRouteOptimization(QDeclarativeGeoRoutingModel::RouteOptimizations optimization)
{
    QGeoRouteRequest::RouteOptimizations reqOptimizations;

    if (optimization & QDeclarativeGeoRoutingModel::ShortestRoute)
        reqOptimizations |= QGeoRouteRequest::ShortestRoute;
    if (optimization & QDeclarativeGeoRoutingModel::FastestRoute)
        reqOptimizations |= QGeoRouteRequest::FastestRoute;
    if (optimization & QDeclarativeGeoRoutingModel::MostEconomicRoute)
        reqOptimizations |= QGeoRouteRequest::MostEconomicRoute;
    if (optimization & QDeclarativeGeoRoutingModel::MostScenicRoute)
        reqOptimizations |= QGeoRouteRequest::MostScenicRoute;

    if (reqOptimizations == request_.routeOptimization())
        return;

    request_.setRouteOptimization(reqOptimizations);

    emit routeOptimizationChanged(optimization);

    if (autoUpdate_ && complete_)
        update();
}

QDeclarativeGeoRoutingModel::RouteOptimizations QDeclarativeGeoRoutingModel::routeOptimization() const
{
    QGeoRouteRequest::RouteOptimizations reqOptimizations = request_.routeOptimization();
    QDeclarativeGeoRoutingModel::RouteOptimizations optimization;

    if (reqOptimizations & QGeoRouteRequest::ShortestRoute)
        optimization |= QDeclarativeGeoRoutingModel::ShortestRoute;
    if (reqOptimizations & QGeoRouteRequest::FastestRoute)
        optimization |= QDeclarativeGeoRoutingModel::FastestRoute;
    if (reqOptimizations & QGeoRouteRequest::MostEconomicRoute)
        optimization |= QDeclarativeGeoRoutingModel::MostEconomicRoute;
    if (reqOptimizations & QGeoRouteRequest::MostScenicRoute)
        optimization |= QDeclarativeGeoRoutingModel::MostScenicRoute;

    return optimization;
}

void QDeclarativeGeoRoutingModel::setStatus(QDeclarativeGeoRoutingModel::Status status)
{
    if (status_ == status)
        return;

    status_ = status;

    emit statusChanged(status_);
}

QDeclarativeGeoRoutingModel::Status QDeclarativeGeoRoutingModel::status() const
{
    return status_;
}

void QDeclarativeGeoRoutingModel::setError(const QString &error)
{
    if (error_ == error)
        return;

    error_ = error;

    emit errorChanged(error_);
}

QString QDeclarativeGeoRoutingModel::error() const
{
    return error_;
}

void QDeclarativeGeoRoutingModel::update()
{
    if (!routingManager_)
        return;

    routingManager_->calculateRoute(request_);
}

void QDeclarativeGeoRoutingModel::routingFinished(QGeoRouteReply *reply)
{
    if (reply->error() != QGeoRouteReply::NoError)
        return;

    beginResetModel();

    qDeleteAll(routes_);
    routes_.clear();
    for (int i = 0; i < reply->routes().size(); ++i)
        routes_.append(new QDeclarativeGeoRoute(reply->routes().at(i), this));

    endResetModel();

    setError("");
    setStatus(QDeclarativeGeoRoutingModel::Ready);

    reply->deleteLater();

    emit routesChanged();
}

void QDeclarativeGeoRoutingModel::routingError(QGeoRouteReply *reply,
                                               QGeoRouteReply::Error error,
                                               const QString &errorString)
{
    setError(errorString);
    setStatus(QDeclarativeGeoRoutingModel::Error);
    reply->deleteLater();
}

#include "moc_qdeclarativegeoroutingmodel_p.cpp"

QTM_END_NAMESPACE
