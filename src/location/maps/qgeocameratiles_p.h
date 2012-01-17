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
#ifndef QGEOCAMERATILES_P_H
#define QGEOCAMERATILES_P_H

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

#include <QtLocation/qlocationglobal.h>
#include <QSharedPointer>
#include <QSet>
#include <QSize>
#include <QSizeF>
#include <QPair>

QT_BEGIN_NAMESPACE

class QGeoProjection;
class QGeoCameraData;
class QGeoTileSpec;
class QGeoMapType;

class QGeoCameraTilesPrivate;

class Q_LOCATION_EXPORT QGeoCameraTiles {
public:
    QGeoCameraTiles(QSharedPointer<QGeoProjection> projection);
    ~QGeoCameraTiles();

    void setCamera(const QGeoCameraData &camera);
    void setScreenSize(const QSize &size);
    void setPluginString(const QString &pluginString);
    void setMapType(const QGeoMapType &mapType);

    void setTileSize(int tileSize);
    void setMaximumZoomLevel(int maxZoom);

    QSet<QGeoTileSpec> tiles() const;
    QPair<QSet<QGeoTileSpec>, QSet<QGeoTileSpec> > tilesSplitByDateline() const;

private:
    QGeoCameraTilesPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoCameraTiles)
};

QT_END_NAMESPACE

#endif // QGEOCAMERATILES_P_H
