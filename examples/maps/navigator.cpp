/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "navigator.h"
#include "mapswidget.h"
#include "marker.h"

Navigator::Navigator(QGeoRoutingManager *routingManager,
                     QGeocodingManager *searchManager,
                     MapsWidget *mapsWidget, const QString &address,
                     const QGeoRouteRequest &requestTemplate) :
    address(address),
    request(requestTemplate),
    routingManager(routingManager),
    searchManager(searchManager),
    mapsWidget(mapsWidget),
    routeObject(0),
    endMarker(0),
    startMarker(0)
{
}

Navigator::~Navigator()
{
    if (routeObject) {
        mapsWidget->map()->removeMapObject(routeObject);
        delete routeObject;
    }
    if (endMarker) {
        mapsWidget->map()->removeMapObject(endMarker);
        delete endMarker;
    }
    if (startMarker) {
        mapsWidget->map()->removeMapObject(startMarker);
        delete startMarker;
    }
}

void Navigator::start()
{
    QList<QGeoCoordinate> waypoints = request.waypoints();
    waypoints.append(mapsWidget->markerManager()->myLocation());
    request.setWaypoints(waypoints);

    startMarker = new Marker(Marker::StartMarker);
    startMarker->setCoordinate(mapsWidget->markerManager()->myLocation());
    startMarker->setName("Start point");
    mapsWidget->map()->addMapObject(startMarker);

    addressReply = searchManager->geocode(address);
    if (addressReply->isFinished()) {
        on_addressSearchFinished();
    } else {
        connect(addressReply, SIGNAL(error(QGeocodeReply::Error,QString)),
                this, SIGNAL(searchError(QGeocodeReply::Error,QString)));
        connect(addressReply, SIGNAL(finished()),
                this, SLOT(on_addressSearchFinished()));
    }
}

void Navigator::on_addressSearchFinished()
{
    if (addressReply->locations().isEmpty()) {
        addressReply->deleteLater();
        return;
    }

    QGeoLocation location = addressReply->locations().at(0);

    QList<QGeoCoordinate> waypoints = request.waypoints();
    waypoints.append(location.coordinate());
    request.setWaypoints(waypoints);

    routeReply = routingManager->calculateRoute(request);
    if (routeReply->isFinished()) {
        on_routingFinished();
    } else {
        connect(routeReply, SIGNAL(error(QGeoRouteReply::Error,QString)),
                this, SIGNAL(routingError(QGeoRouteReply::Error,QString)));
        connect(routeReply, SIGNAL(finished()),
                this, SLOT(on_routingFinished()));
    }

    endMarker = new Marker(Marker::EndMarker);
    endMarker->setCoordinate(location.coordinate());
    endMarker->setAddress(location.address());
    endMarker->setName("Destination");
    mapsWidget->map()->addMapObject(endMarker);

    addressReply->deleteLater();
}

QGeoRoute Navigator::route() const
{
    return firstRoute;
}

void Navigator::on_routingFinished()
{
    if (routeReply->routes().size() <= 0) {
        emit routingError(QGeoRouteReply::NoError, "No valid routes returned");
        routeReply->deleteLater();
        return;
    }

    QGeoRoute route = routeReply->routes().at(0);
    firstRoute = route;

    routeObject = new QGeoMapRouteObject;
    routeObject->setRoute(route);
    routeObject->setPen(QPen(Qt::blue, 2.0));

    mapsWidget->map()->addMapObject(routeObject);

    emit finished();
    routeReply->deleteLater();
}
