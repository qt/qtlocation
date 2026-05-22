// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qmetaobject.h>
#include <QtLocation/private/qgeorouteparser_p.h>
#include <QtLocation/private/qgeoroute_p.h>
#include <QtLocation/qgeoroutesegment.h>
#include <QtLocation/qgeomaneuver.h>
#include <qgeoroutereplyohosmapkit.h>
#include <qgeoroutingmanagerengineohosmapkit.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace {

QHash<QString, QGeoManeuver::InstructionDirection> actionsToDirectionsMap = {
    {"turn-slight-left", QGeoManeuver::DirectionLightLeft},
    {"turn-sharp-left", QGeoManeuver::DirectionHardLeft},
    {"uturn-left", QGeoManeuver::DirectionUTurnLeft},
    {"turn-left", QGeoManeuver::DirectionLeft},
    {"turn-slight-right", QGeoManeuver::DirectionLightRight},
    {"turn-sharp-right", QGeoManeuver::DirectionHardRight},
    {"uturn-right", QGeoManeuver::DirectionUTurnRight},
    {"turn-right", QGeoManeuver::DirectionRight},
    {"straight", QGeoManeuver::DirectionForward},
    {"ramp-left", QGeoManeuver::DirectionBearLeft},
    {"ramp-right", QGeoManeuver::DirectionBearRight},
    {"fork-left", QGeoManeuver::DirectionBearLeft},
    {"fork-right", QGeoManeuver::DirectionBearRight},
    {"roundabout-left", QGeoManeuver::DirectionLeft},
    {"roundabout-right", QGeoManeuver::DirectionRight},
};

QList<QGeoCoordinate> parsePolyline(const QJsonArray &polyArray)
{
    QList<QGeoCoordinate> coords;

    coords.reserve(polyArray.size());
    for (const auto &coordVal : polyArray)
        coords.append(OhosMapKit::CoordinateJson::tryConvertToQGeoCoordinate(coordVal.toObject()));

    return coords;
}

QGeoManeuver parseManeuver(const QJsonObject &stepObj)
{
    QGeoManeuver maneuver;

    maneuver.setInstructionText(stepObj.value("instruction").toString());
    maneuver.setPosition(
        OhosMapKit::CoordinateJson::tryConvertToQGeoCoordinate(stepObj.value("startLocation").toObject()));
    maneuver.setDistanceToNextInstruction(stepObj.value("distance").toDouble());
    maneuver.setTimeToNextInstruction(stepObj.value("duration").toDouble());

    auto action = stepObj.value("action").toString();
    maneuver.setDirection(actionsToDirectionsMap.value(action, QGeoManeuver::NoDirection));

    return maneuver;
}

QGeoRouteSegment parseStep(const QJsonObject &stepObj)
{
    QGeoRouteSegment segment;

    segment.setDistance(stepObj.value("distance").toDouble());
    segment.setTravelTime(stepObj.value("duration").toDouble());
    segment.setPath(parsePolyline(stepObj.value("polyline").toArray()));
    segment.setManeuver(parseManeuver(stepObj));

    return segment;
}

QGeoRoute parseRoute(const QJsonObject &routeObj)
{
    QGeoRoute route;

    route.setBounds(
        OhosMapKit::CoordinateBoundsJson::tryConvertToQGeoRectangle(routeObj.value("bounds").toObject()));

    double totalDistance = 0.0;
    double totalTimeSeconds = 0.0;
    QList<QGeoCoordinate> fullRoutePath;
    QList<QGeoRouteSegment> routeSegments;

    auto pathsArray = routeObj.value("paths").toArray();
    for (const auto &pathVal : pathsArray) {
        auto pathObj = pathVal.toObject();
        totalDistance += pathObj.value("distance").toDouble();
        totalTimeSeconds += pathObj.value("duration").toDouble();

        auto stepsArray = pathObj.value("steps").toArray();
        for (const auto &stepVal : stepsArray) {
            routeSegments.push_back(parseStep(stepVal.toObject()));

            auto segmentPath = routeSegments.last().path();
            if (!segmentPath.isEmpty()) {
                if (!fullRoutePath.isEmpty() && fullRoutePath.last() == segmentPath.first())
                    segmentPath.pop_front();

                fullRoutePath += segmentPath;
            }
        }
    }

    route.setDistance(totalDistance);
    route.setTravelTime(totalTimeSeconds);
    route.setPath(fullRoutePath);
    if (!routeSegments.isEmpty()) {
        auto currentRouteSegment = routeSegments.last();
        for (int i = routeSegments.size() - 2; i >= 0; --i) {
            auto tmpRouteSegment = routeSegments[i];
            tmpRouteSegment.setNextRouteSegment(currentRouteSegment);
            currentRouteSegment = tmpRouteSegment;
        }
        route.setFirstRouteSegment(currentRouteSegment);
    }

    return route;
}

QList<QGeoRoute> parseRouteResponse(const QJsonObject &responseBodyJsonObj)
{
    QList<QGeoRoute> routes;

    auto routesArray = responseBodyJsonObj.value("routes").toArray();
    for (const auto &routeVal : routesArray)
        routes.append(parseRoute(routeVal.toObject()));

    return routes;
}

class QGeoRouteReplyOhosMapKit : public QGeoRouteReply
{
public:
    QGeoRouteReplyOhosMapKit(
        QNetworkReply *reply, const QGeoRouteRequest &request, QObject *parent);

private:
    void networkReplyFinished();
    void networkReplyError(QNetworkReply::NetworkError error);
};

QGeoRouteReplyOhosMapKit::QGeoRouteReplyOhosMapKit(
    QNetworkReply *reply, const QGeoRouteRequest &request, QObject *parent)
    : QGeoRouteReply(request, parent)
{
    if (reply == nullptr) {
        setError(QGeoRouteReply::UnknownError, QStringLiteral("Null reply"));
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &QGeoRouteReplyOhosMapKit::networkReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, &QGeoRouteReplyOhosMapKit::networkReplyError);
    connect(this, &QGeoRouteReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

void QGeoRouteReplyOhosMapKit::networkReplyFinished()
{
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        setError(
            QGeoRouteReply::UnknownError,
            tr("Network reply error: %1")
                .arg(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(reply->error())));
        return;
    }

    auto responseBodyJsonDoc = QJsonDocument::fromJson(reply->readAll());
    if (!responseBodyJsonDoc.isObject()) {
        setError(QGeoRouteReply::ParseError, tr("Response parse error"));
        return;
    }

    setRoutes(parseRouteResponse(responseBodyJsonDoc.object()));

    setFinished(true);
}

void QGeoRouteReplyOhosMapKit::networkReplyError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    setError(QGeoRouteReply::CommunicationError, reply->errorString());
}

}

QGeoRouteReply *makeQGeoRouteReplyOhosMapKit(
    QNetworkReply *reply, const QGeoRouteRequest &request, QObject *parent)
{
    return new QGeoRouteReplyOhosMapKit(reply, request, parent);
}

QT_END_NAMESPACE
