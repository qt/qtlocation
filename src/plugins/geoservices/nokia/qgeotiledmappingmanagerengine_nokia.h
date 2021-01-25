/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
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

    virtual QGeoMap *createMap();
    QString evaluateCopyrightsText(const QGeoMapType mapType,
                                   const qreal zoomLevel,
                                   const QSet<QGeoTileSpec> &tiles);
    QString getScheme(int mapId);
    QString getBaseScheme(int mapId);
    int mapVersion();

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
