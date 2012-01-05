/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MARKER_H
#define MARKER_H

#include <QtCore/QSignalMapper>

#include "qgeomappixmapobject.h"
#include "qgeocodingmanager.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeoaddress.h"
#include "qgeocodereply.h"


class StatusBarItem;

class MarkerPrivate;
class Marker : public QGeoMapPixmapObject
{
    Q_OBJECT

    Q_PROPERTY(MarkerType markerType READ markerType WRITE setMarkerType NOTIFY markerTypeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QGeoAddress address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(bool moveable READ moveable WRITE setMoveable NOTIFY moveableChanged)
public:
    enum MarkerType {
        MyLocationMarker,
        SearchMarker,
        WaypointMarker,
        StartMarker,
        EndMarker,
        PathMarker
    };

    explicit Marker(MarkerType type);

    MarkerType markerType() const;
    void setMarkerType(MarkerType type);

    QString name() const;
    QGeoAddress address() const;
    bool moveable() const;

public slots:
    void setName(QString name);
    void setAddress(QGeoAddress addr);
    void setMoveable(bool moveable);

signals:
    void markerTypeChanged(const MarkerType &type);
    void nameChanged(const QString &name);
    void addressChanged(const QGeoAddress &address);
    void moveableChanged(const bool &moveable);

private:
    MarkerPrivate *d;
};

class MarkerManagerPrivate;
class MarkerManager : public QObject
{
    Q_OBJECT
public:
    explicit MarkerManager(QGeocodingManager *sm, QObject *parent=0);
    ~MarkerManager();

    QGeoCoordinate myLocation() const;

public slots:
    void setMap(QGraphicsGeoMap *map);
    void setStatusBar(StatusBarItem *bar);
    void setMyLocation(QGeoCoordinate coord);
    void search(QString query, qreal radius=-1);
    void removeSearchMarkers();

signals:
    void searchError(QGeocodeReply::Error error, QString errorString);
    void searchFinished();

private:
    MarkerManagerPrivate *d;

private slots:
    void replyFinished(QGeocodeReply *reply);
    void myLocationChanged(QGeoCoordinate location);
    void reverseReplyFinished(QGeocodeReply *reply);
};

#endif // MARKER_H
