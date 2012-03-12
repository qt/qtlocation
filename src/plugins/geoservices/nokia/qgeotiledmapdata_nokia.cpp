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
#include "qgeotilespec.h"

QT_BEGIN_NAMESPACE

/*!
 Constructs a new tiled map data object, which stores the map data required by
 \a geoMap and makes use of the functionality provided by \a engine.
 */
QGeoTiledMapDataNokia::QGeoTiledMapDataNokia(QGeoTiledMappingManagerEngine *engine, QObject *parent /*= 0*/) :
    QGeoTiledMapData(engine, parent),
    watermark(":/images/watermark.png"), // default watermark
    lastZoomLevel(-1), // an invalid zoom level
    mapSize(-1, -1), // an invalid map object size
    lastMapId(-1) /* an invalid map Id */ {}

QGeoTiledMapDataNokia::~QGeoTiledMapDataNokia() {}

// Current implementation is just to accomodate for first time visible tiles change, or map width/height change.
void QGeoTiledMapDataNokia::evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles)
{
    // TODO: implementation of real copyrights fetching mechanism.
    static bool firstTimeChange = true;

    if (firstTimeChange || height() != mapSize.height() || width() != mapSize.width()) {
        firstTimeChange = false;
        QPoint copyrightsPos(10, height() - 50);
        emit copyrightsChanged(watermark, copyrightsPos);
    }
    mapSize.setWidth(width());
    mapSize.setHeight(height());
}

QT_END_NAMESPACE
