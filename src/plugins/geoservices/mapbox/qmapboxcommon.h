/****************************************************************************
**
** Copyright (C) 2017 Mapbox, Inc.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QMAPBOXAPICOMMON_H
#define QMAPBOXAPICOMMON_H

#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtPositioning/QGeoLocation>

QT_BEGIN_NAMESPACE

static const QString mapboxTilesApiPath = QStringLiteral("http://api.tiles.mapbox.com/v4/");

// https://www.mapbox.com/api-documentation/#geocoding
static const QString mapboxGeocodingApiPath = QStringLiteral("https://api.mapbox.com/geocoding/v5/mapbox.places/");
static const QString mapboxGeocodingEnterpriseApiPath = QStringLiteral("https://api.mapbox.com/geocoding/v5/mapbox.places-permanent/");

// https://www.mapbox.com/api-documentation/#directions
static const QString mapboxDirectionsApiPath = QStringLiteral("https://api.mapbox.com/directions/v5/mapbox/");

static const QByteArray mapboxDefaultUserAgent = QByteArrayLiteral("Qt Location based application");

static const qreal mapboxDefaultRadius = 50000;

class QMapboxCommon
{
public:
    static QString mapboxNameForCategory(const QString &category);
    static QGeoLocation parseGeoLocation(const QJsonObject &response);
};

QT_END_NAMESPACE

#endif // QMAPBOXAPICOMMON_H
