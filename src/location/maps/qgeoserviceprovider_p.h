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
****************************************************************************/

#ifndef QGEOSERVICEPROVIDER_P_H
#define QGEOSERVICEPROVIDER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgeoserviceprovider.h"

#include <QHash>
#include <QJsonObject>
#include <QJsonArray>
#include <QLocale>

QT_BEGIN_NAMESPACE

class QGeocodingManager;
class QGeoRoutingManager;
class QGeoMappingManager;

class QGeoServiceProviderFactory;

class QGeoServiceProviderPrivate
{
public:
    QGeoServiceProviderPrivate();
    ~QGeoServiceProviderPrivate();

    void loadMeta();
    void loadPlugin(const QMap<QString, QVariant> &parameters);
    void unload();
    QGeoServiceProviderFactory *factory;
    QJsonObject metaData;

    QMap<QString, QVariant> parameterMap;

    bool experimental;

    QGeocodingManager *geocodingManager;
    QGeoRoutingManager *routingManager;
    QGeoMappingManager *mappingManager;
    QPlaceManager *placeManager;

    QGeoServiceProvider::Error geocodeError;
    QGeoServiceProvider::Error routingError;
    QGeoServiceProvider::Error mappingError;
    QGeoServiceProvider::Error placeError;

    QString geocodeErrorString;
    QString routingErrorString;
    QString mappingErrorString;
    QString placeErrorString;

    QGeoServiceProvider::Error error;
    QString errorString;

    QString providerName;

    QLocale locale;
    bool localeSet;

    static QHash<QString, QJsonObject> plugins(bool reload = false);
    static void loadPluginMetadata(QHash<QString, QJsonObject> &list);
};

QT_END_NAMESPACE

#endif
