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

#include "marker.h"
#include "mapswidget.h"

#include <QPixmap>

#include "qlandmark.h"
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
    QGeoSearchManager *searchManager;

    QSet<QGeoSearchReply*> forwardReplies;
    QSet<QGeoSearchReply*> reverseReplies;
};

MarkerManager::MarkerManager(QGeoSearchManager *searchManager, QObject *parent) :
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

    connect(d->searchManager, SIGNAL(finished(QGeoSearchReply*)),
            this, SLOT(replyFinished(QGeoSearchReply*)));
    connect(d->searchManager, SIGNAL(finished(QGeoSearchReply*)),
            this, SLOT(reverseReplyFinished(QGeoSearchReply*)));
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
    QGeoSearchReply *reply;
    if (radius > 0) {
        QGeoBoundingCircle *boundingCircle = new QGeoBoundingCircle(
                    d->myLocation->coordinate(), radius);
        reply = d->searchManager->search(query,
                                        QGeoSearchManager::SearchAll,
                                        -1, 0,
                                        boundingCircle);
    } else {
        reply = d->searchManager->search(query);
    }

    d->forwardReplies.insert(reply);

    if (d->status) {
        d->status->setText("Searching...");
        d->status->show();
    }

    if (reply->isFinished()) {
        replyFinished(reply);
    } else {
        connect(reply, SIGNAL(error(QGeoSearchReply::Error,QString)),
                this, SIGNAL(searchError(QGeoSearchReply::Error,QString)));
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
        QGeoSearchReply *reply = d->searchManager->reverseGeocode(location);
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

void MarkerManager::reverseReplyFinished(QGeoSearchReply *reply)
{
    if (!d->reverseReplies.contains(reply))
        return;

    if (reply->places().size() > 0) {
        QGeoPlace place = reply->places().first();
        d->myLocation->setAddress(place.address());
    }

    d->revGeocodeRunning = false;
    if (d->myLocHasMoved)
        myLocationChanged(d->myLocation->coordinate());

    d->reverseReplies.remove(reply);
    reply->deleteLater();
}

void MarkerManager::replyFinished(QGeoSearchReply *reply)
{
    if (!d->forwardReplies.contains(reply))
        return;

    // generate the markers and add them to the map
    foreach (QGeoPlace place, reply->places()) {
        Marker *m = new Marker(Marker::SearchMarker);
        m->setCoordinate(place.coordinate());

        if (place.isLandmark()) {
            QLandmark lm(place);
            m->setName(lm.name());
        } else {
            m->setName(QString("%1, %2").arg(place.address().street())
                                        .arg(place.address().city()));
        }
        m->setAddress(place.address());
        m->setMoveable(false);

        d->searchMarkers.append(m);

        if (d->map) {
            d->map->addMapObject(m);
            // also zoom out until marker is visible
            while (!d->map->viewport().contains(place.coordinate()))
                d->map->setZoomLevel(d->map->zoomLevel()-1);
        }
    }

    d->forwardReplies.remove(reply);
    reply->deleteLater();

    emit searchFinished();

    if (d->status)
        d->status->hide();
}
