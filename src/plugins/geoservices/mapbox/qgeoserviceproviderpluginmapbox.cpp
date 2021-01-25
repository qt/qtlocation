/****************************************************************************
**
** Copyright (C) 2014 Canonical Ltd.
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
****************************************************************************/

#include "qgeoserviceproviderpluginmapbox.h"
#include "qgeocodingmanagerenginemapbox.h"
#include "qgeotiledmappingmanagerenginemapbox.h"
#include "qgeoroutingmanagerenginemapbox.h"
#include "qplacemanagerenginemapbox.h"

#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

QT_BEGIN_NAMESPACE

static inline QString msgAccessTokenParameter()
{
    return QGeoServiceProviderFactoryMapbox::tr("Mapbox plugin requires a 'mapbox.access_token' parameter.\n"
                                                "Please visit https://www.mapbox.com");
}

QGeoCodingManagerEngine *QGeoServiceProviderFactoryMapbox::createGeocodingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    const QString accessToken = parameters.value(QStringLiteral("mapbox.access_token")).toString();

    if (!accessToken.isEmpty()) {
        return new QGeoCodingManagerEngineMapbox(parameters, error, errorString);
    } else {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = msgAccessTokenParameter();
        return 0;
    }
}

QGeoMappingManagerEngine *QGeoServiceProviderFactoryMapbox::createMappingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    const QString accessToken = parameters.value(QStringLiteral("mapbox.access_token")).toString();

    if (!accessToken.isEmpty()) {
        return new QGeoTiledMappingManagerEngineMapbox(parameters, error, errorString);
    } else {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = msgAccessTokenParameter();
        return 0;
    }
}

QGeoRoutingManagerEngine *QGeoServiceProviderFactoryMapbox::createRoutingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    const QString accessToken = parameters.value(QStringLiteral("mapbox.access_token")).toString();

    if (!accessToken.isEmpty()) {
        return new QGeoRoutingManagerEngineMapbox(parameters, error, errorString);
    } else {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = msgAccessTokenParameter();
        return 0;
    }
}

QPlaceManagerEngine *QGeoServiceProviderFactoryMapbox::createPlaceManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    const QString accessToken = parameters.value(QStringLiteral("mapbox.access_token")).toString();

    if (!accessToken.isEmpty()) {
        return new QPlaceManagerEngineMapbox(parameters, error, errorString);
    } else {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = msgAccessTokenParameter();
        return 0;
    }
}

QT_END_NAMESPACE
