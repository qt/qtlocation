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

#include "marker.h"
#include "mapswidget.h"

#include <QtWidgets/QPixmap>

#include "qgeoboundingcircle.h"

class MarkerPrivate
{
public:
    Marker::MarkerType type;
    QString name;
    bool moveable;
    QGeoAddress address;
};

Marker::Marker(MarkerType type) :
    QGeoMapPixmapObject(),
    d(new MarkerPrivate)
{
    setMarkerType(type);
}

void Marker::setMarkerType(MarkerType type)
{
    QString filename;
    QPoint offset;
    int scale;

    d->type = type;

    switch (d->type) {
    case MyLocationMarker:
        filename = ":/icons/mylocation.png";
        break;
    case SearchMarker:
        filename = ":/icons/searchmarker.png";
        break;
    case WaypointMarker:
        filename = ":/icons/waypointmarker.png";
        break;
    case StartMarker:
        filename = ":/icons/startmarker.png";
        break;
    case EndMarker:
        filename = ":/icons/endmarker.png";
        break;
    case PathMarker:
        filename = ":/icons/pathmarker.png";
        break;
    }

    if (d->type == MyLocationMarker) {
        offset = QPoint(-13,-13);
        scale = 25;
    } else {
        offset = QPoint(-15, -36);
        scale = 30;
    }

    setOffset(offset);
    setPixmap(QPixmap(filename).scaledToWidth(scale, Qt::SmoothTransformation));
}

void Marker::setAddress(QGeoAddress addr)
{
    if (d->address != addr) {
        d->address = addr;
        emit addressChanged(d->address);
    }
}

Marker::MarkerType Marker::markerType() const
{
    return d->type;
}

QString Marker::name() const
{
    return d->name;
}

QGeoAddress Marker::address() const
{
    return d->address;
}

bool Marker::moveable() const
{
    return d->moveable;
}

void Marker::setName(QString name)
{
    if (d->name != name) {
        d->name = name;
        emit nameChanged(d->name);
    }
}

void Marker::setMoveable(bool moveable)
{
    if (d->moveable != moveable) {
        d->moveable = moveable;
        emit moveableChanged(d->moveable);
    }
}


class MarkerManagerPrivate
{
public:
    Marker *myLocation;
    QList<Marker*> searchMarkers;

    // a reverse geocode request is currently running
    bool revGeocodeRunning;
    // a request is currently running, and my location has changed
    // since it started (ie, the request is stale)
    bool myLocHasMoved;

    QGraphicsGeoMap *map;
    StatusBarItem *status;
    QGeocodingManager *searchManager;

    QSet<QGeocodeReply*> forwardReplies;
    QSet<QGeocodeReply*> reverseReplies;
};

MarkerManager::MarkerManager(QGeocodingManager *searchManager, QObject *parent) :
    QObject(parent),
    d(new MarkerManagerPrivate)
{
    d->searchManager = searchManager;
    d->status = 0;
    d->revGeocodeRunning = false;
    d->myLocHasMoved = false;

    d->myLocation = new Marker(Marker::MyLocationMarker);
    d->myLocation->setName("Me");

    // hook the coordinateChanged() signal for reverse geocoding
    connect(d->myLocation, SIGNAL(coordinateChanged(QGeoCoordinate)),
            this, SLOT(myLocationChanged(QGeoCoordinate)));

    connect(d->searchManager, SIGNAL(finished(QGeocodeReply*)),
            this, SLOT(replyFinished(QGeocodeReply*)));
    connect(d->searchManager, SIGNAL(finished(QGeocodeReply*)),
            this, SLOT(reverseReplyFinished(QGeocodeReply*)));
}

MarkerManager::~MarkerManager()
{
    d->map->removeMapObject(d->myLocation);
    delete d->myLocation;
    removeSearchMarkers();
}

void MarkerManager::setStatusBar(StatusBarItem *bar)
{
    d->status = bar;
}

void MarkerManager::setMap(QGraphicsGeoMap *map)
{
    d->map = map;
    map->addMapObject(d->myLocation);
}

void MarkerManager::setMyLocation(QGeoCoordinate coord)
{
    d->myLocation->setCoordinate(coord);
}

void MarkerManager::search(QString query, qreal radius)
{
    QGeocodeReply *reply;
    if (radius > 0) {
        QGeoBoundingCircle *boundingCircle = new QGeoBoundingCircle(
                    d->myLocation->coordinate(), radius);
        reply = d->searchManager->geocode(query,
                                        -1, 0,
                                        boundingCircle);
    } else {
        reply = d->searchManager->geocode(query);
    }

    d->forwardReplies.insert(reply);

    if (d->status) {
        d->status->setText("Searching...");
        d->status->show();
    }

    if (reply->isFinished()) {
        replyFinished(reply);
    } else {
        connect(reply, SIGNAL(error(QGeocodeReply::Error,QString)),
                this, SIGNAL(searchError(QGeocodeReply::Error,QString)));
    }
}

void MarkerManager::removeSearchMarkers()
{
    foreach (Marker *m, d->searchMarkers) {
        d->map->removeMapObject(m);
        delete m;
    }
}

QGeoCoordinate MarkerManager::myLocation() const
{
    return d->myLocation->coordinate();
}

void MarkerManager::myLocationChanged(QGeoCoordinate location)
{
    if (d->revGeocodeRunning) {
        d->myLocHasMoved = true;
    } else {
        QGeocodeReply *reply = d->searchManager->reverseGeocode(location);
        d->reverseReplies.insert(reply);
        d->myLocHasMoved = false;

        if (reply->isFinished()) {
            d->revGeocodeRunning = false;
            reverseReplyFinished(reply);
        } else {
            d->revGeocodeRunning = true;
        }
    }
}

void MarkerManager::reverseReplyFinished(QGeocodeReply *reply)
{
    if (!d->reverseReplies.contains(reply))
        return;

    if (!reply->locations().isEmpty()) {
        QGeoLocation location = reply->locations().first();
        d->myLocation->setAddress(location.address());
    }

    d->revGeocodeRunning = false;
    if (d->myLocHasMoved)
        myLocationChanged(d->myLocation->coordinate());

    d->reverseReplies.remove(reply);
    reply->deleteLater();
}

void MarkerManager::replyFinished(QGeocodeReply *reply)
{
    if (!d->forwardReplies.contains(reply))
        return;

    // generate the markers and add them to the map
    foreach (const QGeoLocation &location, reply->locations()) {
        Marker *m = new Marker(Marker::SearchMarker);
        m->setCoordinate(location.coordinate());

        m->setName(QString("%1, %2").arg(location.address().street())
                                        .arg(location.address().city()));
        m->setAddress(location.address());
        m->setMoveable(false);

        d->searchMarkers.append(m);

        if (d->map) {
            d->map->addMapObject(m);
            // also zoom out until marker is visible
            while (!d->map->viewport().contains(location.coordinate()))
                d->map->setZoomLevel(d->map->zoomLevel()-1);
        }
    }

    d->forwardReplies.remove(reply);
    reply->deleteLater();

    emit searchFinished();

    if (d->status)
        d->status->hide();
}
