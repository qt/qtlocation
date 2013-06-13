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

#include "qgeotiledmapdata_nokia.h"
#include "jsonparser.h"
#include "qgeotiledmappingmanagerengine_nokia.h"
#include "qgeomapcontroller_p.h"

#include <QDebug>
#include <QObject>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QImage>
#include <QRect>

#include <QStaticText>

QT_BEGIN_NAMESPACE

/*!
 Constructs a new tiled map data object, which stores the map data required by
 \a geoMap and makes use of the functionality provided by \a engine.
 */
QGeoTiledMapDataNokia::QGeoTiledMapDataNokia(QGeoTiledMappingManagerEngineNokia *engine, QObject *parent /*= 0*/) :
    QGeoTiledMapData(engine, parent),
    logo(":/images/logo.png"), // Nokia logo image
    copyrightsSlab(1, 1, QImage::Format_ARGB32) {}

QGeoTiledMapDataNokia::~QGeoTiledMapDataNokia() {}

void QGeoTiledMapDataNokia::evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles)
{
    const int copyrightsMargin = 10;
    const int shadowWidth = 2;
    const int fontSize = 10;

    QGeoTiledMappingManagerEngineNokia *engineNokia = static_cast<QGeoTiledMappingManagerEngineNokia *>(engine());
    QString copyrightsString = engineNokia->evaluateCopyrightsText(activeMapType().style(), mapController()->zoom(), visibleTiles);

    if (width() > 0 && height() > 0 && (lastCopyrightsString.isNull() || copyrightsString != lastCopyrightsString)) {
        copyrightsSlab = copyrightsSlab.copy(0, 0, width(), height());

        // Blank image with full alpha
        copyrightsSlab.fill(Qt::transparent);

        QPainter painter(&copyrightsSlab);
        painter.drawImage(QPoint(1, 1), logo);

        QColor fontColor(Qt::black);
        QFont font("Sans Serif");
        font.setPixelSize(fontSize);
        font.setStyleHint(QFont::SansSerif);
        font.setWeight(QFont::Bold);

        painter.setFont(font);
        painter.setPen(fontColor);
        QRect textLimitsRect(0,
                             logo.height(),
                             copyrightsSlab.width() - (copyrightsMargin * 2),
                             copyrightsSlab.height() - logo.height());

        // Drawing the copyrights base shadow (watermark)
        QRect textBoundingRect;
        QRect wmRect(textLimitsRect);
        int x, y;
        for (x = 0; x < shadowWidth; x++) {
            wmRect.setLeft(textLimitsRect.left() + x*shadowWidth);
            for (y = 0; y < shadowWidth; y++) {
                wmRect.setTop(textLimitsRect.top() + y*shadowWidth);
                painter.drawText(wmRect,
                                 Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                                 copyrightsString,
                                 &textBoundingRect);
            }
        }

        // Drawing the copyrights text top face
        font.setWeight(QFont::Bold);
        fontColor = Qt::white;
        painter.setFont(font);
        painter.setPen(fontColor);
        wmRect.setLeft(textLimitsRect.left() + 1);
        wmRect.setTop(textLimitsRect.top() + 1);
        painter.drawText(wmRect,
                         Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                         copyrightsString,
                         &textBoundingRect);

        painter.end();

        copyrightsSlab = copyrightsSlab.copy(0, 0,
                                             qMax(logo.width(), textBoundingRect.width()) + shadowWidth,
                                             logo.height() + textBoundingRect.height());

        QPoint copyrightsPos(copyrightsMargin, height() - (copyrightsSlab.height() + copyrightsMargin));
        lastCopyrightsPos = copyrightsPos;
        emit copyrightsChanged(copyrightsSlab, copyrightsPos);

        lastCopyrightsString = copyrightsString;
    }

    QPoint copyrightsPos(copyrightsMargin, height() - (copyrightsSlab.height() + copyrightsMargin));
    if (copyrightsPos != lastCopyrightsPos) {
        lastCopyrightsPos = copyrightsPos;
        emit copyrightsChanged(copyrightsSlab, copyrightsPos);
    }
}

QT_END_NAMESPACE
