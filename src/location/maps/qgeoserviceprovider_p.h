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

QTM_BEGIN_NAMESPACE

class QGeoSearchManager;
class QGeoRoutingManager;
class QGeoMappingManager;

class QGeoServiceProviderFactory;

class QGeoServiceProviderPrivate
{
public:
    QGeoServiceProviderPrivate();
    ~QGeoServiceProviderPrivate();

    void loadPlugin(const QString &providerName, const QMap<QString, QVariant> &parameters);

    QGeoServiceProviderFactory *factory;

    QMap<QString, QVariant> parameterMap;

    QGeoSearchManager *searchManager;
    QGeoRoutingManager *routingManager;
    QGeoMappingManager *mappingManager;

    QGeoServiceProvider::Error searchError;
    QGeoServiceProvider::Error routingError;
    QGeoServiceProvider::Error mappingError;

    QString searchErrorString;
    QString routingErrorString;
    QString mappingErrorString;

    QGeoServiceProvider::Error error;
    QString errorString;

    static QHash<QString, QGeoServiceProviderFactory*> plugins(bool reload = false);
    static void loadDynamicPlugins(QHash<QString, QGeoServiceProviderFactory*> *plugins);
    static void loadStaticPlugins(QHash<QString, QGeoServiceProviderFactory*> *plugins);
};

QTM_END_NAMESPACE

#endif
