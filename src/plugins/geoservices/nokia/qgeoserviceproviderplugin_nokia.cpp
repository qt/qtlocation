/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
** This file is part of the Nokia services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file NOKIA_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Nokia services
** plugin source code.
**
****************************************************************************/

#include "qgeoserviceproviderplugin_nokia.h"

#include "qgeocodingmanagerengine_nokia.h"
#include "qgeoroutingmanagerengine_nokia.h"
#include "qgeotiledmappingmanagerengine_nokia.h"
#include "qplacemanagerengine_nokiav1.h"
#include "qplacemanagerengine_nokiav2.h"
#include "qgeointrinsicnetworkaccessmanager.h"
#include "qgeoerror_messages.h"

#include <QtPlugin>
#include <QNetworkProxy>
#include <QCoreApplication>

QT_BEGIN_NAMESPACE

namespace
{
    bool isValidParameter(const QString &param)
    {
        if (param.isEmpty())
            return false;

        if (param.length() > 512)
            return false;

        foreach (QChar c, param) {
            if (!c.isLetterOrNumber() && c.toLatin1() != '%' && c.toLatin1() != '-' &&
                c.toLatin1() != '+' && c.toLatin1() != '_') {
                return false;
            }
        }
        return true;
    }

    QGeoNetworkAccessManager *tryGetNetworkAccessManager(const QVariantMap &parameters)
    {
        return static_cast<QGeoNetworkAccessManager *>(qvariant_cast<void *>(parameters.value(QStringLiteral("nam"))));
    }

    void checkUsageTerms(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    {
        const QString appId = parameters.value(QLatin1String("app_id")).toString();
        const QString token = parameters.value(QLatin1String("token")).toString();

        if (!isValidParameter(appId) || !isValidParameter(token)) {

            *error = QGeoServiceProvider::MissingRequiredParameterError;
            *errorString = QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, MISSED_CREDENTIALS);
        }
    }

    template<class TInstance>
    TInstance * CreateInstanceOf(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    {
        checkUsageTerms(parameters, error, errorString);

        if (*error != QGeoServiceProvider::NoError)
            return 0;

        QGeoNetworkAccessManager *networkManager = tryGetNetworkAccessManager(parameters);
        if (!networkManager)
            networkManager = new QGeoIntrinsicNetworkAccessManager(parameters);

        return new TInstance(networkManager, parameters, error, errorString);
    }
}

QGeocodingManagerEngine *QGeoServiceProviderFactoryNokia::createGeocodingManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return CreateInstanceOf<QGeocodingManagerEngineNokia>(parameters, error, errorString);
}

QGeoMappingManagerEngine *QGeoServiceProviderFactoryNokia::createMappingManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return CreateInstanceOf<QGeoTiledMappingManagerEngineNokia>(parameters, error, errorString);
}

QGeoRoutingManagerEngine *QGeoServiceProviderFactoryNokia::createRoutingManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return CreateInstanceOf<QGeoRoutingManagerEngineNokia>(parameters, error, errorString);
}

QPlaceManagerEngine *QGeoServiceProviderFactoryNokia::createPlaceManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    switch (parameters.value(QLatin1String("places.api_version"), 2).toUInt()) {
    case 1:
        return CreateInstanceOf<QPlaceManagerEngineNokiaV1>(parameters, error, errorString);
    case 2:
        return CreateInstanceOf<QPlaceManagerEngineNokiaV2>(parameters, error, errorString);
    }
    return 0;
}

QT_END_NAMESPACE
