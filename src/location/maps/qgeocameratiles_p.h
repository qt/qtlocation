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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtCore/QScopedPointer>
#include <QRectF>

QT_BEGIN_NAMESPACE

class QGeoCameraData;
class QGeoTileSpec;
class QGeoMapType;
class QGeoCameraTilesPrivate;
class QSize;

class Q_LOCATION_PRIVATE_EXPORT QGeoCameraTiles {
public:
    QGeoCameraTiles();
    ~QGeoCameraTiles();

    void setCameraData(const QGeoCameraData &camera);
    QGeoCameraData cameraData() const;
    void setVisibleArea(const QRectF &visibleArea);
    void setScreenSize(const QSize &size);
    void setTileSize(int tileSize);
    int tileSize() const;
    void setViewExpansion(double viewExpansion);
    void setPluginString(const QString &pluginString);
    void setMapType(const QGeoMapType &mapType);
    QGeoMapType activeMapType() const;
    void setMapVersion(int mapVersion);
    const QSet<QGeoTileSpec>& createTiles();

protected:
    QScopedPointer<QGeoCameraTilesPrivate> d_ptr;

    friend class QGeoCameraTilesPrivate;
    Q_DISABLE_COPY(QGeoCameraTiles)
};

QT_END_NAMESPACE

#endif // QGEOCAMERATILES_P_H
