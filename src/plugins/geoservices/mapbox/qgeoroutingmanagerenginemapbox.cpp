/****************************************************************************
**
** Copyright (C) 2016 Vlad Seryakov <vseryakov@gmail.com>
** Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
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

#include "qgeoroutingmanagerenginemapbox.h"
#include "qgeoroutereplymapbox.h"
#include "qmapboxcommon.h"
#include "QtLocation/private/qgeorouteparserosrmv5_p.h"

#include <QtCore/QUrlQuery>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QGeoRoutingManagerEngineMapbox::QGeoRoutingManagerEngineMapbox(const QVariantMap &parameters,
                                                         QGeoServiceProvider::Error *error,
                                                         QString *errorString)
    : QGeoRoutingManagerEngine(parameters),
      m_networkManager(new QNetworkAccessManager(this)),
      m_userAgent(mapboxDefaultUserAgent)
{
    if (parameters.contains(QStringLiteral("mapbox.useragent"))) {
        m_userAgent = parameters.value(QStringLiteral("mapbox.useragent")).toString().toLatin1();
    }

    if (parameters.contains(QStringLiteral("mapbox.access_token"))) {
        m_accessToken = parameters.value(QStringLiteral("mapbox.access_token")).toString();
    }

    bool use_mapbox_text_instructions = true;
    if (parameters.contains(QStringLiteral("mapbox.routing.use_mapbox_text_instructions"))) {
        use_mapbox_text_instructions = parameters.value(QStringLiteral("mapbox.use_mapbox_text_instructions")).toBool();
    }

    QGeoRouteParserOsrmV5 *parser = new QGeoRouteParserOsrmV5(this, use_mapbox_text_instructions);
    parser->setAccessToken(m_accessToken);
    m_routeParser = parser;

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QGeoRoutingManagerEngineMapbox::~QGeoRoutingManagerEngineMapbox()
{
}

QGeoRouteReply* QGeoRoutingManagerEngineMapbox::calculateRoute(const QGeoRouteRequest &request)
{
    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, m_userAgent);

    QString url = mapboxDirectionsApiPath;

    QGeoRouteRequest::TravelModes travelModes = request.travelModes();
    if (travelModes.testFlag(QGeoRouteRequest::PedestrianTravel)) {
        url += QStringLiteral("walking/");
    } else if (travelModes.testFlag(QGeoRouteRequest::BicycleTravel)) {
        url += QStringLiteral("cycling/");
    } else if (travelModes.testFlag(QGeoRouteRequest::CarTravel)) {
        const QList<QGeoRouteRequest::FeatureType> &featureTypes = request.featureTypes();
        int trafficFeatureIdx = featureTypes.indexOf(QGeoRouteRequest::TrafficFeature);
        QGeoRouteRequest::FeatureWeight trafficWeight = request.featureWeight(QGeoRouteRequest::TrafficFeature);
        if (trafficFeatureIdx >= 0 &&
           (trafficWeight == QGeoRouteRequest::AvoidFeatureWeight || trafficWeight == QGeoRouteRequest::DisallowFeatureWeight)) {
            url += QStringLiteral("driving-traffic/");
        } else {
            url += QStringLiteral("driving/");
        }
    }

    networkRequest.setUrl(m_routeParser->requestUrl(request, url));

    QNetworkReply *reply = m_networkManager->get(networkRequest);

    QGeoRouteReplyMapbox *routeReply = new QGeoRouteReplyMapbox(reply, request, this);

    connect(routeReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(routeReply, SIGNAL(error(QGeoRouteReply::Error,QString)),
            this, SLOT(replyError(QGeoRouteReply::Error,QString)));

    return routeReply;
}

const QGeoRouteParser *QGeoRoutingManagerEngineMapbox::routeParser() const
{
    return m_routeParser;
}

void QGeoRoutingManagerEngineMapbox::replyFinished()
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply *>(sender());
    if (reply)
        emit finished(reply);
}

void QGeoRoutingManagerEngineMapbox::replyError(QGeoRouteReply::Error errorCode,
                                             const QString &errorString)
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply *>(sender());
    if (reply)
        emit error(reply, errorCode, errorString);
}

QT_END_NAMESPACE
