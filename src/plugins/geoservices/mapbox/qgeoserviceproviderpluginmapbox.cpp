/****************************************************************************
**
** Copyright (C) 2014 Canonical Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeoserviceproviderpluginmapbox.h"
#include "qgeotiledmappingmanagerenginemapbox.h"

#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

QT_BEGIN_NAMESPACE

QGeoCodingManagerEngine *QGeoServiceProviderFactoryMapbox::createGeocodingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(parameters)
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    return 0;
}

QGeoMappingManagerEngine *QGeoServiceProviderFactoryMapbox::createMappingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    const QString mapId = parameters.value(QStringLiteral("mapbox.map_id")).toString();
    const QString accessToken = parameters.value(QStringLiteral("mapbox.access_token")).toString();

    if (!mapId.isEmpty() && !accessToken.isEmpty()) {
        return new QGeoTiledMappingManagerEngineMapbox(parameters, error, errorString);
    } else {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = tr("Mapbox plugin requires 'mapbox.map_id' and 'mapbox.access_token' parameters.\n"
                          "Please visit https://www.mapbox.com");
        return 0;
    }
}

QGeoRoutingManagerEngine *QGeoServiceProviderFactoryMapbox::createRoutingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(parameters)
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    return 0;
}

QPlaceManagerEngine *QGeoServiceProviderFactoryMapbox::createPlaceManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(parameters)
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    return 0;
}

QT_END_NAMESPACE
