/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#ifndef QGEOTILEDMAPPINGMANAGERENGINE_NOKIA_H
#define QGEOTILEDMAPPINGMANAGERENGINE_NOKIA_H

#include "qgeotiledmappingmanagerengine_p.h"
#include <QtPositioning/QGeoRectangle>
#include "qgeomaptype_p.h"
#include "qgeomapversion.h"

#include <QGeoServiceProvider>

#include <QList>
#include <QHash>
#include <QSet>

QT_BEGIN_NAMESPACE

class QByteArray;
class QGeoTileSpec;
class QGeoNetworkAccessManager;

class QGeoTiledMappingManagerEngineNokia : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT

public:
    QGeoTiledMappingManagerEngineNokia(QGeoNetworkAccessManager *networkManager,
                                       const QVariantMap &parameters,
                                       QGeoServiceProvider::Error *error,
                                       QString *errorString);
    ~QGeoTiledMappingManagerEngineNokia();

    QGeoMap *createMap() override;
    QString evaluateCopyrightsText(const QGeoMapType &mapType,
                                   qreal zoomLevel,
                                   const QSet<QGeoTileSpec> &tiles);
    QString getScheme(int mapId) const;
    QString getBaseScheme(int mapId) const;
    int mapVersion() const;

public Q_SLOTS:
    void loadCopyrightsDescriptorsFromJson(const QByteArray &jsonData);
    void parseNewVersionInfo(const QByteArray &versionData);

private:
    class CopyrightDesc
    {
    public:
        CopyrightDesc()
            : maxLevel(-1),
              minLevel(-1) {}

        qreal maxLevel;
        qreal minLevel;
        QList<QGeoRectangle> boxes;
        QString alt;
        QString label;
    };

    void initialize();
    void populateMapSchemes();
    void updateVersion(const QJsonObject &newVersionData);
    void saveMapVersion();
    void loadMapVersion();

    QHash<QString, QList<CopyrightDesc> > m_copyrights;
    QHash<int, QString> m_mapSchemes;
    QGeoMapVersion m_mapVersion;

    QString m_cacheDirectory;
};

QT_END_NAMESPACE

#endif // QGEOTILEDMAPPINGMANAGERENGINE_NOKIA_H
