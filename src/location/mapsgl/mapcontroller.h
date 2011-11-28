/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>

#include "qgeocoordinate.h"
#include "cameradata.h"

QT_BEGIN_NAMESPACE

class Map;

class Q_LOCATION_EXPORT AnimatableCoordinate {
public:
    AnimatableCoordinate();
    AnimatableCoordinate(const QGeoCoordinate &coordinate,
                         QSharedPointer<Projection> projection);

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    QSharedPointer<Projection> projection() const;
    void setProjection(QSharedPointer<Projection> projection);

private:
    QGeoCoordinate coordinate_;
    QSharedPointer<Projection> projection_;
};

class Q_LOCATION_EXPORT MapController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(AnimatableCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_PROPERTY(qreal tilt READ tilt WRITE setTilt NOTIFY tiltChanged)
    Q_PROPERTY(qreal roll READ roll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    MapController(Map *map, QSharedPointer<Projection> projection);
    ~MapController();

    AnimatableCoordinate center() const;
    void setCenter(const AnimatableCoordinate &center);

    qreal bearing() const;
    void setBearing(qreal bearing);

    qreal tilt() const;
    void setTilt(qreal tilt);

    qreal roll() const;
    void setRoll(qreal roll);

    qreal zoom() const;
    void setZoom(qreal zoom);

    void pan(qreal dx, qreal dy);

private slots:
    void cameraDataChanged(const CameraData &cameraData);

signals:
    void centerChanged(const AnimatableCoordinate &center);
    void bearingChanged(qreal bearing);
    void tiltChanged(qreal tilt);
    void rollChanged(qreal roll);
    void zoomChanged(qreal zoom);

private:
    Map *map_;
    QSharedPointer<Projection> projection_;
    CameraData oldCameraData_;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(AnimatableCoordinate)

#endif // MAPCONTROLLER_H
