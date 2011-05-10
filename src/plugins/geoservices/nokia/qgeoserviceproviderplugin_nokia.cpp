/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qgeoserviceproviderplugin_nokia.h"

#include "qgeosearchmanagerengine_nokia.h"
#include "qgeoroutingmanagerengine_nokia.h"
#include "qgeomappingmanagerengine_nokia.h"

#include <QtPlugin>
#include <QNetworkProxy>

QGeoServiceProviderFactoryNokia::QGeoServiceProviderFactoryNokia() {}

QGeoServiceProviderFactoryNokia::~QGeoServiceProviderFactoryNokia() {}

QString QGeoServiceProviderFactoryNokia::providerName() const
{
    return "nokia";
}

int QGeoServiceProviderFactoryNokia::providerVersion() const
{
    return 1;
}

QGeoSearchManagerEngine* QGeoServiceProviderFactoryNokia::createSearchManagerEngine(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return new QGeoSearchManagerEngineNokia(parameters, error, errorString);
}

QGeoMappingManagerEngine* QGeoServiceProviderFactoryNokia::createMappingManagerEngine(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return new QGeoMappingManagerEngineNokia(parameters, error, errorString);
}

QGeoRoutingManagerEngine* QGeoServiceProviderFactoryNokia::createRoutingManagerEngine(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return new QGeoRoutingManagerEngineNokia(parameters, error, errorString);
}

const QString QGeoServiceProviderFactoryNokia::defaultToken("152022572f0e44e07489c35cd46fa246");
const QString QGeoServiceProviderFactoryNokia::defaultReferer("qtlocationapi");

Q_EXPORT_PLUGIN2(qtgeoservices_nokia, QGeoServiceProviderFactoryNokia)
