// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <qgeocodingmanagerengineohosmapkit.h>
#include <qgeoroutingmanagerengineohosmapkit.h>
#include <qgeoserviceproviderpluginohosmapkit.h>
#include <qgeotiledmappingmanagerengineohosmapkit.h>
#include <qohosmapkitcommon.h>
#include <qplacemanagerengineohosmapkit.h>

QT_BEGIN_NAMESPACE

namespace {

bool isAuthenticationKeySet(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
{
    if (OhosMapKit::getAuthenticationKeyParameterOrEmpty(parameters).isEmpty()) {
        if (error != nullptr)
            *error = QGeoServiceProvider::MissingRequiredParameterError;

        if (errorString != nullptr) {
            *errorString = QGeoServiceProviderFactoryOhosMapKit::tr(
                "OHOS MapKit plugin requires a '%1' parameter. "
                "Visit Huawei AppGallery Connect service to generate it.")
                .arg(OhosMapKit::authenticationKeyParameterName);
        }
        return false;
    }
    return true;
}

}

QGeoCodingManagerEngine *QGeoServiceProviderFactoryOhosMapKit::createGeocodingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return isAuthenticationKeySet(parameters, error, errorString)
        ? makeQGeoCodingManagerEngineOhosMapKit(parameters, error, errorString)
        : nullptr;
}

QGeoMappingManagerEngine *QGeoServiceProviderFactoryOhosMapKit::createMappingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return isAuthenticationKeySet(parameters, error, errorString)
        ? makeQGeoTiledMappingManagerEngineOhosMapKit(parameters, error, errorString)
        : nullptr;
}

QGeoRoutingManagerEngine *QGeoServiceProviderFactoryOhosMapKit::createRoutingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return isAuthenticationKeySet(parameters, error, errorString)
        ? makeQGeoRoutingManagerEngineOhosMapKit(parameters, error, errorString)
        : nullptr;
}

QPlaceManagerEngine *QGeoServiceProviderFactoryOhosMapKit::createPlaceManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return isAuthenticationKeySet(parameters, error, errorString)
        ? makeQPlaceManagerEngineOhosMapKit(parameters, error, errorString)
        : nullptr;
}

QT_END_NAMESPACE
