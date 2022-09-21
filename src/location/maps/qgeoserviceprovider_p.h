/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
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
#include <QCborMap>
#include <QLocale>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QGeoCodingManager;
class QGeoRoutingManager;
class QGeoMappingManager;

class QGeoServiceProviderFactory;
class QQmlEngine;

class QGeoServiceProviderPrivate
{
public:
    QGeoServiceProviderPrivate();
    ~QGeoServiceProviderPrivate();

    void loadMeta();
    void loadPlugin(const QVariantMap &parameters);
    void unload();
    void filterParameterMap();

    /* helper templates for generating the feature and manager accessors */
    template <class Manager, class Engine>
    Manager *manager(QGeoServiceProvider::Error *error,
                     QString *errorString);
    template <class Flags>
    Flags features(const char *enumName) const;

    QGeoServiceProviderFactory *factory = nullptr;
    QCborMap metaData;

    QVariantMap parameterMap;
    QVariantMap cleanedParameterMap;

    bool experimental = false;

    std::unique_ptr<QGeoCodingManager> geocodingManager;
    std::unique_ptr<QGeoRoutingManager> routingManager;
    std::unique_ptr<QGeoMappingManager> mappingManager;
    std::unique_ptr<QPlaceManager> placeManager;
    QQmlEngine *qmlEngine = nullptr;

    QGeoServiceProvider::Error geocodeError = QGeoServiceProvider::NoError;
    QGeoServiceProvider::Error routingError = QGeoServiceProvider::NoError;
    QGeoServiceProvider::Error mappingError = QGeoServiceProvider::NoError;
    QGeoServiceProvider::Error placeError = QGeoServiceProvider::NoError;
    QGeoServiceProvider::Error navigationError = QGeoServiceProvider::NoError;

    QString geocodeErrorString;
    QString routingErrorString;
    QString mappingErrorString;
    QString placeErrorString;
    QString navigationErrorString;

    QGeoServiceProvider::Error error = QGeoServiceProvider::NoError;
    QString errorString;

    QString providerName;

    QLocale locale;
    bool localeSet = false;

    static QMultiHash<QString, QCborMap> plugins(bool reload = false);
    static void loadPluginMetadata(QMultiHash<QString, QCborMap> &list);
};

QT_END_NAMESPACE

#endif
