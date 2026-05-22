// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOSERVICEPROVIDERPLUGINOHOSMAPKIT_H
#define QGEOSERVICEPROVIDERPLUGINOHOSMAPKIT_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtLocation/QGeoServiceProviderFactory>

QT_BEGIN_NAMESPACE

class QGeoServiceProviderFactoryOhosMapKit : public QObject, public QGeoServiceProviderFactory
{
    Q_OBJECT
    Q_INTERFACES(QGeoServiceProviderFactory)
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.geoservice.serviceproviderfactory/6.0" FILE "ohosmapkit_plugin.json")

public:
    QGeoCodingManagerEngine *createGeocodingManagerEngine(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const;
    QGeoMappingManagerEngine *createMappingManagerEngine(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const;
    QGeoRoutingManagerEngine *createRoutingManagerEngine(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const;
    QPlaceManagerEngine *createPlaceManagerEngine(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const;
};

QT_END_NAMESPACE

#endif // QGEOSERVICEPROVIDERPLUGINOHOSMAPKIT_H
