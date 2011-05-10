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

#ifndef QGEOMAPPINGMANAGERENGINE_NOKIA_H
#define QGEOMAPPINGMANAGERENGINE_NOKIA_H

#include "qgeoserviceproviderplugin_nokia.h"

#include <qgeoserviceprovider.h>
#include <qgeotiledmappingmanagerengine.h>

class QNetworkAccessManager;
class QNetworkDiskCache;

QTM_USE_NAMESPACE

class QGeoMappingManagerEngineNokia : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT
public:
    QGeoMappingManagerEngineNokia(const QMap<QString, QVariant> &parameters,
                                  QGeoServiceProvider::Error *error,
                                  QString *errorString);
    ~QGeoMappingManagerEngineNokia();

    QGeoMapData* createMapData();
    QGeoTiledMapReply* getTileImage(const QGeoTiledMapRequest &request);

private:
    Q_DISABLE_COPY(QGeoMappingManagerEngineNokia)

    QString getRequestString(const QGeoTiledMapRequest &request) const;

    static QString sizeToStr(const QSize &size);
    static QString mapTypeToStr(QGraphicsGeoMap::MapType type);
#if defined(Q_OS_SYMBIAN)
    QChar findFirstInternalFlashDrive();
#endif //Q_OS_SYMBIAN

    QNetworkAccessManager *m_networkManager;
    QNetworkDiskCache *m_cache;
    QString m_host;
    QString m_token;
    QString m_referer;
};

#endif
