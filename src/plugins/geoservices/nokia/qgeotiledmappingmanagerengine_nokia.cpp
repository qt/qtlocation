/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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

#include "qgeocameracapabilities_p.h"
#include "qgeotiledmappingmanagerengine_nokia.h"
#include "qgeotiledmapdata_nokia.h"
#include "qgeotilefetcher_nokia.h"
#include "qgeotilespec_p.h"
#include "qgeoprojection_p.h"
#include "qdoublevector2d_p.h"
#include "qgeotilecache_p.h"

#include <QDebug>
#include <QDir>
#include <QVariant>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

QGeoTiledMappingManagerEngineNokia::QGeoTiledMappingManagerEngineNokia(
    QGeoNetworkAccessManager *networkManager,
    const QMap<QString, QVariant> &parameters,
    QGeoServiceProvider::Error *error,
    QString *errorString)
    : QGeoTiledMappingManagerEngine()
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    QGeoCameraCapabilities capabilities;

    capabilities.setMinimumZoomLevel(0.0);
    capabilities.setMaximumZoomLevel(20.0);

    setCameraCapabilities(capabilities);

    setTileSize(QSize(256, 256));

    QList<QGeoMapType> types;
    types << QGeoMapType(QGeoMapType::StreetMap,tr("Street Map"),tr("Nokia Street Map"), false, 1);
    types << QGeoMapType(QGeoMapType::SatelliteMapDay,tr("Satellite Map(day)"),tr("Nokia Satellite Map (day)"), false, 2);
    types << QGeoMapType(QGeoMapType::TerrainMap,tr("Terrain Map"),tr("Nokia Terrain Map"), false, 3);
    types << QGeoMapType(QGeoMapType::HybridMap,tr("Hybrid Map"),tr("Nokia Hybrid Map"), false, 4);
    types << QGeoMapType(QGeoMapType::TransitMap,tr("Transit Map"),tr("Nokia Transit Map"), false, 5);
    types << QGeoMapType(QGeoMapType::GrayStreetMap,tr("Gray Street Map"),tr("Nokia Gray Street Map"), false, 6);
    types << QGeoMapType(QGeoMapType::StreetMap,tr("Mobile Street Map"),tr("Nokia Mobile Street Map"), true, 7);
    types << QGeoMapType(QGeoMapType::TerrainMap,tr("Mobile Terrain Map"),tr("Nokia Mobile Terrain Map"), true, 8);
    types << QGeoMapType(QGeoMapType::HybridMap,tr("Mobile Hybrid Map"),tr("Nokia Mobile Hybrid Map"), true, 9);
    types << QGeoMapType(QGeoMapType::TransitMap,tr("Mobile Transit Map"),tr("Nokia Mobile Transit Map"), true, 10);
    types << QGeoMapType(QGeoMapType::GrayStreetMap,tr("Mobile Gray Street Map"),tr("Nokia Mobile Gray Street Map"), true, 11);
    setSupportedMapTypes(types);

    QGeoTileFetcherNokia *fetcher = new QGeoTileFetcherNokia(parameters, networkManager, this, tileSize());
    setTileFetcher(fetcher);

    // TODO: do this in a plugin-neutral way so that other tiled map plugins
    //       don't need this boilerplate

    QString cacheDir;
    if (parameters.contains(QLatin1String("mapping.cache.directory")))
        cacheDir = parameters.value(QLatin1String("mapping.cache.directory")).toString();

    QGeoTileCache *tileCache = createTileCacheWithDir(cacheDir);

    if (parameters.contains(QLatin1String("mapping.cache.disk.size"))) {
      bool ok = false;
      int cacheSize = parameters.value(QLatin1String("mapping.cache.disk.size")).toString().toInt(&ok);
      if (ok)
          tileCache->setMaxDiskUsage(cacheSize);
    }

    if (parameters.contains(QLatin1String("mapping.cache.memory.size"))) {
      bool ok = false;
      int cacheSize = parameters.value(QLatin1String("mapping.cache.memory.size")).toString().toInt(&ok);
      if (ok)
          tileCache->setMaxMemoryUsage(cacheSize);
    }

    if (parameters.contains(QLatin1String("mapping.cache.texture.size"))) {
      bool ok = false;
      int cacheSize = parameters.value(QLatin1String("mapping.cache.texture.size")).toString().toInt(&ok);
      if (ok)
          tileCache->setExtraTextureUsage(cacheSize);
    }

    populateMapTypesDb();
    QMetaObject::invokeMethod(fetcher, "fetchCopyrightsData", Qt::QueuedConnection);
}

QGeoTiledMappingManagerEngineNokia::~QGeoTiledMappingManagerEngineNokia() {}

void QGeoTiledMappingManagerEngineNokia::populateMapTypesDb()
{
    m_mapTypeStrings[QGeoMapType::NoMap]             = QLatin1String("normal");
    m_mapTypeStrings[QGeoMapType::TerrainMap]        = QLatin1String("terrain");
    m_mapTypeStrings[QGeoMapType::StreetMap]         = QLatin1String("normal");
    m_mapTypeStrings[QGeoMapType::SatelliteMapDay]   = QLatin1String("satellite");
    m_mapTypeStrings[QGeoMapType::SatelliteMapNight] = QLatin1String("satellite");
    m_mapTypeStrings[QGeoMapType::HybridMap]         = QLatin1String("hybrid");
    m_mapTypeStrings[QGeoMapType::TransitMap]        = QLatin1String("normal");
    m_mapTypeStrings[QGeoMapType::GrayStreetMap]     = QLatin1String("normal");
}

void QGeoTiledMappingManagerEngineNokia::loadCopyrightsDescriptorsFromJson(const QByteArray &jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(jsonData));
    if (doc.isNull()) {
        qDebug() << "QGeoCopyrightsCacheNokia::copyrightsFetche() Invalid JSon document";
        return;
    }

    QJsonObject jsonObj = doc.object();
    QStringList keys = jsonObj.keys();

    m_copyrights.clear();
    for (int keyIndex = 0; keyIndex < keys.count(); keyIndex++) {
        QList<CopyrightDesc> copyrightDescList;

        QJsonArray descs = jsonObj[ keys[ keyIndex ] ].toArray();
        for (int descIndex = 0; descIndex < descs.count(); descIndex++) {
            CopyrightDesc copyrightDesc;
            QJsonObject desc = descs.at(descIndex).toObject();

            copyrightDesc.minLevel = desc["minLevel"].toDouble();
            copyrightDesc.maxLevel = desc["maxLevel"].toDouble();
            copyrightDesc.label = desc["label"].toString();
            copyrightDesc.alt  = desc["alt"].toString();

            QJsonArray coordBoxes = desc["boxes"].toArray();
            for (int boxIndex = 0; boxIndex < coordBoxes.count(); boxIndex++) {
                QJsonArray box = coordBoxes[boxIndex].toArray();
                qreal top    = box[0].toDouble();
                qreal left   = box[1].toDouble();
                qreal bottom = box[2].toDouble();
                qreal right  = box[3].toDouble();
                QGeoRectangle boundingBox(QGeoCoordinate(top > bottom? top : bottom,
                                                           left),
                                            QGeoCoordinate(top > bottom? bottom : top,
                                                           right));
                copyrightDesc.boxes << boundingBox;
            }
            copyrightDescList << copyrightDesc;
        }
        m_copyrights[ keys[ keyIndex ] ] = copyrightDescList;
    }
}

QString QGeoTiledMappingManagerEngineNokia::evaluateCopyrightsText(const QGeoMapType::MapStyle mapStyle,
                                                                   const int zoomLevel,
                                                                   const QSet<QGeoTileSpec> &tiles)
{
    typedef QSet<QGeoTileSpec>::const_iterator tile_iter;
    QGeoRectangle viewport;
    double viewX0, viewY0, viewX1, viewY1;

    tile_iter tile = tiles.constBegin();
    tile_iter lastTile = tiles.constEnd();

    if (tiles.count()) {
        viewX0 = viewX1 = tile->x();
        viewY0 = viewY1 = tile->y();

        // this approach establishes a geo-bounding box from passed tiles to test for intersecition
        // with copyrights boxes.
        int numTiles = 0;
        for (; tile != lastTile; ++tile) {
            if (tile->x() < viewX0)
                viewX0 = tile->x();
            if (tile->x() > viewX1)
                viewX1 = tile->x();
            if (tile->y() < viewY0)
                viewY0 = tile->y();
            if (tile->y() > viewY1)
                viewY1 = tile->y();
            numTiles++;
        }

        viewX1++;
        viewY1++;

        QDoubleVector2D pt;
        double divFactor = qPow(2.0, zoomLevel);

        pt.setX(viewX0 / divFactor);
        pt.setY(viewY0 / divFactor);
        viewport.setTopLeft(QGeoProjection::mercatorToCoord(pt));
        pt.setX(viewX1 / divFactor);
        pt.setY(viewY1 / divFactor);
        viewport.setBottomRight(QGeoProjection::mercatorToCoord(pt));
    }

    // TODO: the following invalidation detection algorithm may be improved later.
    QList<CopyrightDesc> descriptorList = m_copyrights[ m_mapTypeStrings[mapStyle] ];
    CopyrightDesc *descriptor;
    int descIndex, boxIndex;
    QString copyrightsString("");

    for (descIndex = 0; descIndex < descriptorList.count(); descIndex++) {
        if (descriptorList[descIndex].minLevel <= zoomLevel && zoomLevel <= descriptorList[descIndex].maxLevel) {
            descriptor = &descriptorList[descIndex];
            for (boxIndex = 0; boxIndex < descriptor->boxes.count(); boxIndex++) {
                QGeoRectangle box = descriptor->boxes[boxIndex];
                QGeoCoordinate topRight = box.topRight();
                QGeoCoordinate bottomLeft = box.bottomLeft();

                if (descriptor->boxes[boxIndex].intersects(viewport)) {
                    if (copyrightsString.length())
                        copyrightsString += "\n";
                    copyrightsString += descriptor->label;
                    break;
                }
            }
            if (!descriptor->boxes.count()) {
                if (copyrightsString.length())
                    copyrightsString += "\n";
                copyrightsString += descriptor->label;
            }
        }
    }

    return copyrightsString;
}

QGeoMapData *QGeoTiledMappingManagerEngineNokia::createMapData()
{
    return new QGeoTiledMapDataNokia(this);
}

QT_END_NAMESPACE

