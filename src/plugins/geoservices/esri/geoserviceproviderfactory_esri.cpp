/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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

#include "geoserviceproviderfactory_esri.h"
#include "geotiledmappingmanagerengine_esri.h"
#include "geocodingmanagerengine_esri.h"
#include "georoutingmanagerengine_esri.h"
#include "placemanagerengine_esri.h"

#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

QT_BEGIN_NAMESPACE

QGeoCodingManagerEngine *GeoServiceProviderFactoryEsri::createGeocodingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return new GeoCodingManagerEngineEsri(parameters, error, errorString);
}

QGeoMappingManagerEngine *GeoServiceProviderFactoryEsri::createMappingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return new GeoTiledMappingManagerEngineEsri(parameters, error, errorString);
}

QGeoRoutingManagerEngine *GeoServiceProviderFactoryEsri::createRoutingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    const QString token = parameters.value(QStringLiteral("esri.token")).toString();

    if (!token.isEmpty()) {
        return new GeoRoutingManagerEngineEsri(parameters, error, errorString);
    } else {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = tr("Esri plugin requires a 'esri.token' parameter.\n"
                          "Please visit https://developers.arcgis.com/authentication/accessing-arcgis-online-services/");
        return 0;
    }
}

QPlaceManagerEngine *GeoServiceProviderFactoryEsri::createPlaceManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return new PlaceManagerEngineEsri(parameters, error, errorString);
}

QT_END_NAMESPACE
