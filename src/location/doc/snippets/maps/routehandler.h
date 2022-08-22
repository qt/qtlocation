/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [RouteHandler]
class RouteHandler : public QObject
{
    Q_OBJECT
public:
    RouteHandler(QGeoRoutingManager *routingManager,
                   const QGeoCoordinate &origin,
                   const QGeoCoordinate &destination) {

        QGeoRouteRequest request(origin, destination);

        // The request defaults to the fastest route by car, which is
        // equivalent to:
        // request.setTravelMode(QGeoRouteRequest::CarTravel);
        // request.setRouteOptimization(QGeoRouteRequest::FastestRoute);

        request.setAvoidFeatureTypes(QGeoRouteRequest::AvoidTolls);
        request.setAvoidFeatureTypes(QGeoRouteRequest::AvoidMotorPoolLanes);

        QGeoRouteRequest::AvoidFeaturesTypes avoidableFeatures = routingManager->supportedAvoidFeatureTypes();

        if (!(avoidableFeatures & request.avoidFeatureTypes())) {
            // ... inform the user that the routing manager does not
            // provide support for avoiding tolls and/or motor pool lanes ...
            return;
        }

        QGeoRouteReply *reply = routingManager->calculateRoute(request);

        if (reply->isFinished()) {
            if (reply->error() == QGeoRouteReply::NoError) {
                routeCalculated(reply);
            } else {
                routeError(reply, reply->error(), reply->errorString());
            }
            return;
        }

        connect(routingManager, &QGeoRoutingManager::finished,
                this, &RouteHandler::routeCalculated);

        connect(routingManager, &QGeoRoutingManager::errorOccurred,
                this, &RouteHandler::routeError);
    }

private slots:
    void routeCalculated(QGeoRouteReply *reply)
    {
        // A route request can ask for several alternative routes ...
        if (reply->routes().size() != 0) {

            // ... but by default it will only get a single route
            QGeoRoute route = reply->routes().at(0);

            //... now we have to make use of the route ...
        }

        reply->deleteLater();
    }

    void routeError(QGeoRouteReply *reply, QGeoRouteReply:Error error, const QString &errorString)
    {
        // ... inform the user that an error has occurred ...
        reply->deleteLater();
    }
};
//! [RouteHandler]
