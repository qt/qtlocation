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

#include "qgeotiledmapdata_nokia.h"
#include "jsonparser.h"
#include "qgeomappingmanagerengine_nokia.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"

#include <QNetworkAccessManager>
#include <QNetworkProxy>

QTM_USE_NAMESPACE

/*!
 Constructs a new tiled map data object, which stores the map data required by
 \a geoMap and makes use of the functionality provided by \a engine.
 */
QGeoTiledMapDataNokia::QGeoTiledMapDataNokia(QGeoMappingManagerEngineNokia *engine) :
    QGeoTiledMapData(engine),
    watermark(":/images/watermark.png")
{
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(copyrightReplyFinished(QNetworkReply*)));

    m_networkManager->get(QNetworkRequest(QUrl("http://maptile.maps.svc.ovi.com/maptiler/v2/copyright/newest")));
}

QGeoTiledMapDataNokia::~QGeoTiledMapDataNokia()
{
}

static QGeoBoundingBox variantListToBoundingBox(const QVariantList & list) {
    if (list.size() < 4) return QGeoBoundingBox();

    qreal latTop = list[0].toReal();
    qreal latBottom = list[2].toReal();
    if (latTop < latBottom)
        qSwap(latTop, latBottom);

    return QGeoBoundingBox(QGeoCoordinate(latTop, list[1].toReal()), QGeoCoordinate(latBottom, list[3].toReal()));
}

void QGeoTiledMapDataNokia::copyrightReplyFinished(QNetworkReply * reply)
{
    if (reply->error() != QNetworkReply::NoError)
        return;

    JSONParser jp(reply->readAll());
    QVariant root = jp.parse();

    if (!root.isValid())
        return;

    copyrights.clear();

    QVariantHash rootHash = root.toHash();

    foreach (QString key, rootHash.keys()) {
        QList<CopyrightDescriptor> copyrightDescriptorList;
        foreach (QVariant copyrightSource, rootHash[key].toList()) {
            QVariantHash copyrightSourceHash = copyrightSource.toHash();
            CopyrightDescriptor copyrightDescriptor;
            copyrightDescriptor.minLevel = copyrightSourceHash["minLevel"].toReal();
            copyrightDescriptor.maxLevel = copyrightSourceHash["maxLevel"].toReal();
            copyrightDescriptor.label    = copyrightSourceHash["label"].toString();
            copyrightDescriptor.alt      = copyrightSourceHash["alt"].toString();

            foreach (QVariant box, copyrightSourceHash["boxes"].toList()) {
                copyrightDescriptor.boxes << variantListToBoundingBox(box.toList());
            }

            copyrightDescriptorList << copyrightDescriptor;
        }

        copyrights[key] = copyrightDescriptorList;
    }
}

QString QGeoTiledMapDataNokia::getViewCopyright()
{
    QGeoBoundingBox viewport = this->viewport();

    QString terraintype;

    switch (mapType()) {
        case QGraphicsGeoMap::StreetMap:
            terraintype = "normal";
            break;

        case QGraphicsGeoMap::SatelliteMapDay:
        case QGraphicsGeoMap::SatelliteMapNight:
            terraintype = "hybrid";
            break;

        case QGraphicsGeoMap::TerrainMap:
            terraintype = "terrain";
            break;

        default:
            terraintype = "normal";
    }

    CopyrightDescriptor fallback;

    QStringList copyrightStrings;
    bool contained = false;
    foreach (CopyrightDescriptor copyrightDescriptor, copyrights[terraintype]) {
        if (zoomLevel() < copyrightDescriptor.minLevel) continue;
        if (zoomLevel() > copyrightDescriptor.maxLevel) continue;

        if (copyrightDescriptor.boxes.isEmpty()) {
            fallback = copyrightDescriptor;
        }
        else {
            foreach (QGeoBoundingBox box, copyrightDescriptor.boxes) {
                if (box.intersects(viewport)) {
                    copyrightStrings << copyrightDescriptor.label;
                    if (box.contains(viewport)) {
                        contained = true;
                        break;
                    }
                    // TODO: consider the case where the viewport is fully contained by the combined bounding boxes, but not by one individual bounding box
                }
            }
        }
    }

    if (copyrightStrings.isEmpty() || !contained) {
        if (!fallback.label.isEmpty()) copyrightStrings << fallback.label;
    }

    copyrightStrings.removeDuplicates();

    QString ret = copyrightStrings.join(", ");

    return ret;
}

/*!
 \reimp
 */
void QGeoTiledMapDataNokia::paintProviderNotices(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    QRect viewport = painter->combinedTransform().inverted().mapRect(painter->viewport());

    painter->drawPixmap(
        viewport.bottomLeft()+QPoint(5,-5-watermark.height()),
        watermark
    );

    QString copyrightText = getViewCopyright();

    if (copyrightText != lastCopyrightText || lastViewport != viewport) {
        lastCopyrightText = copyrightText;
        lastViewport = viewport;

        QRect maxBoundingRect(QPoint(viewport.left()+10+watermark.width(), viewport.top()), QPoint(viewport.right()-5, viewport.bottom()-5));

        QRect textBoundingRect = painter->boundingRect(maxBoundingRect, Qt::AlignLeft | Qt::AlignBottom | Qt::TextWordWrap, copyrightText);
        lastCopyrightRect = textBoundingRect.adjusted(-1, -1, 1, 1);

        lastCopyright = QPixmap(lastCopyrightRect.size());
        lastCopyright.fill(QColor(Qt::transparent));

        {
            QPainter painter2(&lastCopyright);

            painter2.drawText(
                QRect(QPoint(1, 2), textBoundingRect.size()),
                Qt::TextWordWrap,
                copyrightText
            );

            painter2.drawPixmap(QRect(QPoint(-1, -1), lastCopyrightRect.size()), lastCopyright);
            painter2.drawPixmap(QRect(QPoint(1, -1), lastCopyrightRect.size()), lastCopyright);

            painter2.setPen(QColor(Qt::white));
            painter2.drawText(
                QRect(QPoint(1, 1), textBoundingRect.size()),
                Qt::TextWordWrap,
                copyrightText
            );
        }
    }

    painter->drawPixmap(
        lastCopyrightRect,
        lastCopyright
    );
}
