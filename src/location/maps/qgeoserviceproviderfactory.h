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

#ifndef QGEOSERVICEPROVIDERFACTORY_H
#define QGEOSERVICEPROVIDERFACTORY_H

#include "qgeoserviceprovider.h"

#include <QtPlugin>
#include <QMap>
#include <QString>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Location)

class Q_LOCATION_EXPORT QGeoServiceProviderFactory
{
public:
    virtual ~QGeoServiceProviderFactory() {}

    virtual QString providerName() const = 0;
    virtual int providerVersion() const = 0;

    virtual QGeocodingManagerEngine* createGeocodingManagerEngine(const QMap<QString, QVariant> &parameters,
            QGeoServiceProvider::Error *error,
            QString *errorString) const;
    virtual QGeoMappingManagerEngine* createMappingManagerEngine(const QMap<QString, QVariant> &parameters,
            QGeoServiceProvider::Error *error,
            QString *errorString) const;
    virtual QGeoRoutingManagerEngine* createRoutingManagerEngine(const QMap<QString, QVariant> &parameters,
            QGeoServiceProvider::Error *error,
            QString *errorString) const;
    virtual QPlaceManagerEngine *createPlaceManagerEngine(const QMap<QString, QVariant> &parameters,
            QGeoServiceProvider::Error *error,
            QString *errorString) const;
};

#define QT_GEOSERVICE_BACKEND_INTERFACE "com.nokia.qt.mobility.geoservice.serviceproviderfactory/1.0"
Q_DECLARE_INTERFACE(QGeoServiceProviderFactory, QT_GEOSERVICE_BACKEND_INTERFACE);

QT_END_NAMESPACE

QT_END_HEADER

#endif
