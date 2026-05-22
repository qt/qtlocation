# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_plugin(QGeoServiceProviderFactoryOhosMapKitPlugin
    OUTPUT_NAME qtgeoservices_ohosmapkit
    CLASS_NAME QGeoServiceProviderFactoryOhosMapKit
    PLUGIN_TYPE geoservices
    SOURCES
        qgeoserviceproviderpluginohosmapkit.h qgeoserviceproviderpluginohosmapkit.cpp
        qgeotiledmappingmanagerengineohosmapkit.h qgeotiledmappingmanagerengineohosmapkit.cpp
        qgeotilefetcherohosmapkit.h qgeotilefetcherohosmapkit.cpp
        qgeofiletilecacheohosmapkit.h qgeofiletilecacheohosmapkit.cpp
        qgeomapreplyohosmapkit.h qgeomapreplyohosmapkit.cpp
        qgeocodingmanagerengineohosmapkit.h qgeocodingmanagerengineohosmapkit.cpp
        qgeocodereplyohosmapkit.h qgeocodereplyohosmapkit.cpp
        qgeoroutingmanagerengineohosmapkit.h qgeoroutingmanagerengineohosmapkit.cpp
        qgeoroutereplyohosmapkit.h qgeoroutereplyohosmapkit.cpp
        qplacemanagerengineohosmapkit.h qplacemanagerengineohosmapkit.cpp
        qplacesearchreplyohosmapkit.h qplacesearchreplyohosmapkit.cpp
        qplacesearchsuggestionreplyohosmapkit.h qplacesearchsuggestionreplyohosmapkit.cpp
        qpoicategoryohosmapkit.h qpoicategoryohosmapkit.cpp
        qohosmapkitcommon.h qohosmapkitcommon.cpp
    LIBRARIES
        Qt::Core
        Qt::CorePrivate
        Qt::Network
        Qt::LocationPrivate
        Qt::PositioningPrivate
)
