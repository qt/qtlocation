// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/private/qohoslogger_p.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qurl.h>
#include <QtLocation/QGeoRouteReply>
#include <QtLocation/QGeoRouteRequest>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtPositioning/qgeocoordinate.h>
#include <bitset>
#include <qgeoroutereplyohosmapkit.h>
#include <qgeoroutingmanagerengineohosmapkit.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace {

const QString routingApiBasePath = QLatin1String("https://mapapi.cloud.huawei.com/mapApi/v1/routeService/");
const QHash<QGeoRouteRequest::TravelModes, QUrl> routingApiUrl =
    {{{QGeoRouteRequest::PedestrianTravel}, QUrl(routingApiBasePath + QLatin1String("walking"))},
    {{QGeoRouteRequest::BicycleTravel}, QUrl(routingApiBasePath + QLatin1String("bicycling"))},
    {{QGeoRouteRequest::CarTravel}, QUrl(routingApiBasePath + QLatin1String("driving"))},
};

enum class RestApiRoutingPolicy {
    Fast = 0,
    AvoidToolRoads = 1 << 0,
    AvoidHighways = 1 << 1,
    ShortDistance = 1 << 2,
    AvoidFerries = 1 << 3,
    AvoidCongestedRoads = 1 << 4,
    PrioritizeMainRoads = 1 << 5,
    SelectRoutesIntelligently = 1 << 6,
    PrioritizeHighways = 1 << 7,
    TakeRoutesChangingTheLeast = 1 << 8,
};

bool isSingleTravelMode(QGeoRouteRequest::TravelModes modes)
{
    auto modesBits = std::bitset<sizeof(unsigned)>(static_cast<unsigned>(modes));
    return modesBits.count() == 1;
}

QJsonArray createRestApiAvoidArray(const QGeoRouteRequest &request)
{
    QJsonArray restApiAvoidArray;

    auto features = request.featureTypes();
    auto travelModes = request.travelModes();

    for (auto feature : features) {
        if (travelModes != QGeoRouteRequest::CarTravel && feature != QGeoRouteRequest::FerryFeature)
            continue;

        auto featureWeight = request.featureWeight(feature);

        switch (feature) {
        case QGeoRouteRequest::TollFeature:
            if (featureWeight == QGeoRouteRequest::AvoidFeatureWeight
                || featureWeight == QGeoRouteRequest::DisallowFeatureWeight)
                restApiAvoidArray.push_back(static_cast<int>(RestApiRoutingPolicy::AvoidToolRoads));
            break;
        case QGeoRouteRequest::HighwayFeature:
            if (featureWeight == QGeoRouteRequest::AvoidFeatureWeight
                || featureWeight == QGeoRouteRequest::DisallowFeatureWeight) {
                restApiAvoidArray.push_back(static_cast<int>(RestApiRoutingPolicy::AvoidHighways));
            } else if (featureWeight == QGeoRouteRequest::PreferFeatureWeight
                || featureWeight == QGeoRouteRequest::RequireFeatureWeight) {
                restApiAvoidArray.push_back(static_cast<int>(RestApiRoutingPolicy::PrioritizeHighways));
            }
            break;
        case QGeoRouteRequest::FerryFeature:
            if (featureWeight == QGeoRouteRequest::AvoidFeatureWeight
                || featureWeight == QGeoRouteRequest::DisallowFeatureWeight)
                restApiAvoidArray.push_back(static_cast<int>(RestApiRoutingPolicy::AvoidFerries));
            break;
        default:
            break;
        }
    }

    if (travelModes == QGeoRouteRequest::CarTravel) {
        auto routeOptimizations = request.routeOptimization();
        if (routeOptimizations.testFlag(QGeoRouteRequest::ShortestRoute))
            restApiAvoidArray.push_back(static_cast<int>(RestApiRoutingPolicy::ShortDistance));
        else if (routeOptimizations.testFlag(QGeoRouteRequest::FastestRoute))
            restApiAvoidArray.push_back(static_cast<int>(RestApiRoutingPolicy::Fast));
    }

    return restApiAvoidArray;
}

QJsonArray createWaypointsJsonArray(const QList<QGeoCoordinate> &waypoints)
{
    QJsonArray waypointsArray;

    constexpr int minNumOfWaypointsToCreateRouteMidpoints = 3;
    if (waypoints.size() < minNumOfWaypointsToCreateRouteMidpoints)
        return waypointsArray;
    constexpr int secondElementIndex = 1;
    const int penultimateElementIndex = waypoints.size() - 2;
    for (const auto &waypoint: waypoints.mid(secondElementIndex, penultimateElementIndex))
        waypointsArray.push_back(OhosMapKit::CoordinateJson::tryConvertFromQGeoCoordinate(waypoint));

    return waypointsArray;
}

QByteArray createRouteRequestBody(const QGeoRouteRequest &request)
{
    constexpr int minNumOfWaypoints = 2;
    constexpr int restApiWaypointsLimitation = 25;
    auto waypoints = request.waypoints();
    if (waypoints.size() < minNumOfWaypoints || waypoints.size() > restApiWaypointsLimitation) {
        qOhosPrintfWarning(
            "%s: Invalid number of waypoints: %d. Cannot create route request.",
            Q_FUNC_INFO, int(waypoints.size()));
        return {};
    }

    QJsonObject routeRequestBody;

    routeRequestBody.insert(
        QStringLiteral("origin"),
        OhosMapKit::CoordinateJson::tryConvertFromQGeoCoordinate(waypoints.first()));
    routeRequestBody.insert(
        QStringLiteral("destination"),
        OhosMapKit::CoordinateJson::tryConvertFromQGeoCoordinate(waypoints.last()));

    auto restApiAvoidArray = createRestApiAvoidArray(request);
    if (!restApiAvoidArray.isEmpty())
        routeRequestBody.insert(QStringLiteral("avoid"), restApiAvoidArray);

    if (request.travelModes() == QGeoRouteRequest::CarTravel) {
        auto waypointsArray = createWaypointsJsonArray(waypoints);
        if (!waypointsArray.empty())
            routeRequestBody.insert(QStringLiteral("waypoints"), waypointsArray);
    }

    return QJsonDocument(routeRequestBody).toJson(QJsonDocument::Compact);
}

class QGeoRoutingManagerEngineOhosMapKit : public QGeoRoutingManagerEngine
{
public:
    QGeoRoutingManagerEngineOhosMapKit(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

    QGeoRouteReply *calculateRoute(const QGeoRouteRequest &request) override;

private:
    void onReplyFinished();
    void onReplyError(QGeoRouteReply::Error errorCode, const QString &errorString);

    QNetworkAccessManager m_networkManager;
    QString m_userAgent;
    QString m_authenticationKey;
};

QGeoRoutingManagerEngineOhosMapKit::QGeoRoutingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    : QGeoRoutingManagerEngine(parameters)
    , m_userAgent(OhosMapKit::getAuthenticationKeyParameterOrEmpty(parameters))
    , m_authenticationKey(OhosMapKit::getAuthenticationKeyParameterOrEmpty(parameters))
{
    if (error != nullptr)
        *error = QGeoServiceProvider::NoError;

    if (errorString != nullptr)
        errorString->clear();
}

QGeoRouteReply *QGeoRoutingManagerEngineOhosMapKit::calculateRoute(const QGeoRouteRequest &request)
{
    auto travelModes = request.travelModes();

    if (!isSingleTravelMode(travelModes)) {
        return new QGeoRouteReply(
            QGeoRouteReply::UnsupportedOptionError,
            tr("Selection of multiple travel modes is not supported by OHOS Map Kit plugin"));
    }

    if (!routingApiUrl.contains(travelModes)) {
        return new QGeoRouteReply(
            QGeoRouteReply::UnsupportedOptionError,
            tr("Selected travel mode: %1 is not supported by OHOS Map Kit plugin")
                .arg(travelModes.toInt()));
    }

    auto *reply = makeQGeoRouteReplyOhosMapKit(
        m_networkManager.post(
            OhosMapKit::createOhosMapKitNetworkRequestWithJsonBody(
                routingApiUrl.value(travelModes), m_userAgent, m_authenticationKey),
            createRouteRequestBody(request)),
        request, this);

    connect(
        reply, &QGeoRouteReply::finished,
        this, &QGeoRoutingManagerEngineOhosMapKit::onReplyFinished);
    connect(
        reply, &QGeoRouteReply::errorOccurred,
        this, &QGeoRoutingManagerEngineOhosMapKit::onReplyError);

    return reply;
}

void QGeoRoutingManagerEngineOhosMapKit::onReplyFinished()
{
    auto *reply = qobject_cast<QGeoRouteReply *>(sender());
    if (reply != nullptr)
        Q_EMIT finished(reply);
}

void QGeoRoutingManagerEngineOhosMapKit::onReplyError(
    QGeoRouteReply::Error errorCode, const QString &errorString)
{
    auto *reply = qobject_cast<QGeoRouteReply *>(sender());
    if (reply != nullptr)
        Q_EMIT errorOccurred(reply, errorCode, errorString);
}

}

QGeoRoutingManagerEngine *makeQGeoRoutingManagerEngineOhosMapKit(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
{
    return new QGeoRoutingManagerEngineOhosMapKit(parameters, error, errorString);
}

QT_END_NAMESPACE
