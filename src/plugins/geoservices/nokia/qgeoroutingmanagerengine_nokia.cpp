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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qgeoroutingmanagerengine_nokia.h"
#include "qgeoroutereply_nokia.h"

#include <QStringList>
#include <QNetworkProxy>
#include <QUrl>
#include <qgeoboundingbox.h>

QGeoRoutingManagerEngineNokia::QGeoRoutingManagerEngineNokia(const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoRoutingManagerEngine(parameters),
        m_host("prd.lbsp.navteq.com"),
        m_token(QGeoServiceProviderFactoryNokia::defaultToken),
        m_referer(QGeoServiceProviderFactoryNokia::defaultReferer)
{
    m_networkManager = new QNetworkAccessManager(this);

    if (parameters.contains("routing.proxy")) {
        QString proxy = parameters.value("routing.proxy").toString();
        if (!proxy.isEmpty()) {
            QUrl proxyUrl(proxy);
            if (proxyUrl.isValid()) {
                m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, 
                    proxyUrl.host(),
                    proxyUrl.port(8080),
                    proxyUrl.userName(),
                    proxyUrl.password()));
            }
        }
    }

    if (parameters.contains("routing.host")) {
        QString host = parameters.value("routing.host").toString();
        if (!host.isEmpty())
            m_host = host;
    }

    if (parameters.contains("routing.referer")) {
        m_referer = parameters.value("routing.referer").toString();
    }

    if (parameters.contains("routing.token")) {
        m_token = parameters.value("routing.token").toString();
    }
    else if (parameters.contains("token")) {
        m_token = parameters.value("token").toString();
    }

    setSupportsRouteUpdates(true);
    setSupportsAlternativeRoutes(true);
    setSupportsExcludeAreas(true);

    QGeoRouteRequest::FeatureTypes featureTypes;
    featureTypes |= QGeoRouteRequest::TollFeature;
    featureTypes |= QGeoRouteRequest::HighwayFeature;
    featureTypes |= QGeoRouteRequest::FerryFeature;
    featureTypes |= QGeoRouteRequest::TunnelFeature;
    featureTypes |= QGeoRouteRequest::DirtRoadFeature;
    setSupportedFeatureTypes(featureTypes);

    QGeoRouteRequest::FeatureWeights featureWeights;
    featureWeights |= QGeoRouteRequest::AvoidFeatureWeight;
    setSupportedFeatureWeights(featureWeights);

    QGeoRouteRequest::ManeuverDetails maneuverDetails;
    maneuverDetails |= QGeoRouteRequest::BasicManeuvers;
    setSupportedManeuverDetails(maneuverDetails);

    QGeoRouteRequest::RouteOptimizations optimizations;
    optimizations |= QGeoRouteRequest::ShortestRoute;
    optimizations |= QGeoRouteRequest::FastestRoute;
    optimizations |= QGeoRouteRequest::MostEconomicRoute;
    setSupportedRouteOptimizations(optimizations);

    QGeoRouteRequest::TravelModes travelModes;
    travelModes |= QGeoRouteRequest::CarTravel;
    travelModes |= QGeoRouteRequest::PedestrianTravel;
    travelModes |= QGeoRouteRequest::PublicTransitTravel;
    setSupportedTravelModes(travelModes);

    QGeoRouteRequest::SegmentDetails segmentDetails;
    segmentDetails |= QGeoRouteRequest::BasicSegmentData;
    setSupportedSegmentDetails(segmentDetails);

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";
}

QGeoRoutingManagerEngineNokia::~QGeoRoutingManagerEngineNokia() {}

QGeoRouteReply* QGeoRoutingManagerEngineNokia::calculateRoute(const QGeoRouteRequest& request)
{
    QString reqString = calculateRouteRequestString(request);

    if (reqString.isEmpty()) {
        QGeoRouteReply *reply = new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError, "The given route request options are not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QNetworkReply *networkReply = m_networkManager->get(QNetworkRequest(QUrl(reqString)));
    QGeoRouteReplyNokia *reply = new QGeoRouteReplyNokia(request, networkReply, this);

    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(routeFinished()));

    connect(reply,
            SIGNAL(error(QGeoRouteReply::Error, QString)),
            this,
            SLOT(routeError(QGeoRouteReply::Error, QString)));

    return reply;
}

QGeoRouteReply* QGeoRoutingManagerEngineNokia::updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
{
    QString reqString = updateRouteRequestString(route, position);

    if (reqString.isEmpty()) {
        QGeoRouteReply *reply = new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError, "The given route request options are not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QNetworkReply *networkReply = m_networkManager->get(QNetworkRequest(QUrl(reqString)));
    QGeoRouteRequest updateRequest(route.request());
    updateRequest.setTravelModes(route.travelMode());
    QGeoRouteReplyNokia *reply = new QGeoRouteReplyNokia(updateRequest, networkReply, this);

    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(routeFinished()));

    connect(reply,
            SIGNAL(error(QGeoRouteReply::Error, QString)),
            this,
            SLOT(routeError(QGeoRouteReply::Error, QString)));

    return reply;
}

bool QGeoRoutingManagerEngineNokia::checkEngineSupport(const QGeoRouteRequest &request,
        QGeoRouteRequest::TravelModes travelModes) const
{
    QList<QGeoRouteRequest::FeatureType> featureTypeList = request.featureTypes();
    QGeoRouteRequest::FeatureTypes featureTypeFlag = QGeoRouteRequest::NoFeature;
    QGeoRouteRequest::FeatureWeights featureWeightFlag = QGeoRouteRequest::NeutralFeatureWeight;

    for (int i = 0; i < featureTypeList.size(); ++i) {
        featureTypeFlag |= featureTypeList.at(i);
        featureWeightFlag |= request.featureWeight(featureTypeList.at(i));
    }

    if ((featureTypeFlag & supportedFeatureTypes()) != featureTypeFlag)
        return false;

    if ((featureWeightFlag & supportedFeatureWeights()) != featureWeightFlag)
        return false;


    if ((request.maneuverDetail() & supportedManeuverDetails()) != request.maneuverDetail())
        return false;

    if ((request.segmentDetail() & supportedSegmentDetails()) != request.segmentDetail())
        return false;

    if ((request.routeOptimization() & supportedRouteOptimizations()) != request.routeOptimization())
        return false;

    if ((travelModes & supportedTravelModes()) != travelModes)
        return false;

    return true;
}

QString QGeoRoutingManagerEngineNokia::calculateRouteRequestString(const QGeoRouteRequest &request)
{
    bool supported = checkEngineSupport(request, request.travelModes());

    if ((request.numberAlternativeRoutes() != 0) && !supportsAlternativeRoutes())
        supported = false;

    if (!supported)
        return "";

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/routing/6.2/calculateroute.xml?referer=" + m_referer;

    if (!m_token.isNull())
        requestString += "&token=" + m_token;

    int numWaypoints = request.waypoints().size();
    if (numWaypoints < 2)
        return "";

    for (int i = 0;i < numWaypoints;++i) {
        requestString += "&waypoint";
        requestString += QString::number(i);
        requestString += "=";
        requestString += trimDouble(request.waypoints().at(i).latitude());
        requestString += ",";
        requestString += trimDouble(request.waypoints().at(i).longitude());
    }

    requestString += modesRequestString(request, request.travelModes());

    requestString += "&alternatives=";
    requestString += QString::number(request.numberAlternativeRoutes());

    requestString += routeRequestString(request);

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::updateRouteRequestString(const QGeoRoute &route, const QGeoCoordinate &position)
{
    if (!checkEngineSupport(route.request(), route.travelMode()))
        return "";

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/routing/6.2/getroute.xml";

    requestString += "?routeid=";
    requestString += route.routeId();

    requestString += "&pos=";
    requestString += QString::number(position.latitude());
    requestString += ",";
    requestString += QString::number(position.longitude());

    requestString += modesRequestString(route.request(), route.travelMode());

    requestString += routeRequestString(route.request());

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::modesRequestString(const QGeoRouteRequest &request,
        QGeoRouteRequest::TravelModes travelModes) const
{
    QString requestString;

    QGeoRouteRequest::RouteOptimizations optimization = request.routeOptimization();

    QStringList types;
    if (optimization.testFlag(QGeoRouteRequest::ShortestRoute))
        types.append("shortest");
    if (optimization.testFlag(QGeoRouteRequest::FastestRoute))
        types.append("fastestNow");
    if (optimization.testFlag(QGeoRouteRequest::MostEconomicRoute))
        types.append("economic");
    if (optimization.testFlag(QGeoRouteRequest::MostScenicRoute))
        types.append("scenic");

    QStringList modes;
    if (travelModes.testFlag(QGeoRouteRequest::CarTravel))
        modes.append("car");
    if (travelModes.testFlag(QGeoRouteRequest::PedestrianTravel))
        modes.append("pedestrian");
    if (travelModes.testFlag(QGeoRouteRequest::PublicTransitTravel))
        modes.append("publicTransport");

    QStringList featureStrings;
    QList<QGeoRouteRequest::FeatureType> featureTypes = request.featureTypes();
    for (int i = 0; i < featureTypes.size(); ++i) {
        QGeoRouteRequest::FeatureWeight weight = request.featureWeight(featureTypes.at(i));

        if (weight == QGeoRouteRequest::NeutralFeatureWeight)
            continue;

        QString weightString = "";
        switch (weight) {
            case QGeoRouteRequest::PreferFeatureWeight:
                weightString = "1";
                break;
            case QGeoRouteRequest::AvoidFeatureWeight:
                weightString = "-1";
                break;
            case QGeoRouteRequest::DisallowFeatureWeight:
                weightString = "-3";
                break;
        }

        if (weightString.isEmpty())
            continue;

        switch (featureTypes.at(i)) {
            case QGeoRouteRequest::TollFeature:
                featureStrings.append("tollroad:" + weightString);
                break;
            case QGeoRouteRequest::HighwayFeature:
                featureStrings.append("motorway:" + weightString);
                break;
            case QGeoRouteRequest::FerryFeature:
                featureStrings.append("boatFerry:" + weightString);
                featureStrings.append("railFerry:" + weightString);
                break;
            case QGeoRouteRequest::TunnelFeature:
                featureStrings.append("tunnel:" + weightString);
                break;
            case QGeoRouteRequest::DirtRoadFeature:
                featureStrings.append("dirtRoad:" + weightString);
                break;
        }
    }

    for (int i = 0;i < types.count();++i) {
        requestString += "&mode" + QString::number(i) + "=";
        requestString += types[i] + ";" + modes.join(",");
        if (featureStrings.count())
            requestString += ";" + featureStrings.join(",");
    }
    return requestString;
}

QString QGeoRoutingManagerEngineNokia::routeRequestString(const QGeoRouteRequest &request) const
{
    QString requestString;

    int numAreas = request.excludeAreas().count();
    if (numAreas > 0) {
        requestString += "&avoidareas";
        for (int i = 0;i < numAreas;++i) {
            requestString += i == 0 ? "=" : ";";
            QGeoBoundingBox box = request.excludeAreas().at(i);
            requestString += trimDouble(box.topLeft().latitude());
            requestString += ",";
            requestString += trimDouble(box.topLeft().longitude());
            requestString += ",";
            requestString += trimDouble(box.bottomRight().latitude());
            requestString += ",";
            requestString += trimDouble(box.bottomRight().longitude());
        }
    }

//    TODO: work out what was going on here
//    - segment and instruction/maneuever functions are mixed and matched
//    - tried to implement sensible equivalents below
//    QStringList legAttributes;
//    if (request.instructionDetail() & QGeoRouteRequest::BasicSegmentData) {
//        requestString += "&linkattributes=sh,le"; //shape,length
//        legAttributes.append("links");
//    }
//
//    if (request.instructionDetail() & QGeoRouteRequest::BasicInstructions) {
//        legAttributes.append("maneuvers");
//        requestString += "&maneuverattributes=po,tt,le,di"; //position,traveltime,length,direction
//        if (!(request.instructionDetail() & QGeoRouteRequest::NoSegmentData))
//            requestString += ",li"; //link
//    }

    QStringList legAttributes;
    if (request.segmentDetail() & QGeoRouteRequest::BasicSegmentData) {
        requestString += "&linkattributes=sh,le"; //shape,length
        legAttributes.append("links");
    }

    if (request.maneuverDetail() & QGeoRouteRequest::BasicManeuvers) {
        legAttributes.append("maneuvers");
        requestString += "&maneuverattributes=po,tt,le,di"; //position,traveltime,length,direction
        if (!(request.segmentDetail() & QGeoRouteRequest::NoSegmentData))
            requestString += ",li"; //link
    }

    requestString += "&routeattributes=sm,sh,bb,lg"; //summary,shape,boundingBox,legs
    if (legAttributes.count() > 0) {
        requestString += "&legattributes=";
        requestString += legAttributes.join(",");
    }

    requestString += "&departure=";
    requestString += QDateTime::currentDateTime().toUTC().toString("yyyy-MM-ddThh:mm:ssZ");

    requestString += "&instructionformat=text";

    requestString += "&language=";
    requestString += locale().name();

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::trimDouble(qreal degree, int decimalDigits)
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

void QGeoRoutingManagerEngineNokia::routeFinished()
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(finished(QGeoRouteReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoRoutingManagerEngineNokia::routeError(QGeoRouteReply::Error error, const QString &errorString)
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(error(QGeoRouteReply*, QGeoRouteReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}
