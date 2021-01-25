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

#ifndef QGEOFILETILECACHEMAPBOX_H
#define QGEOFILETILECACHEMAPBOX_H

#include <QtLocation/private/qgeofiletilecache_p.h>
#include <QMap>

QT_BEGIN_NAMESPACE

class QGeoFileTileCacheMapbox : public QGeoFileTileCache
{
    Q_OBJECT
public:
    QGeoFileTileCacheMapbox(const QList<QGeoMapType> &mapTypes, int scaleFactor, const QString &directory = QString(), QObject *parent = 0);
    ~QGeoFileTileCacheMapbox();

protected:
    QString tileSpecToFilename(const QGeoTileSpec &spec, const QString &format, const QString &directory) const override;
    QGeoTileSpec filenameToTileSpec(const QString &filename) const override;

    QList<QGeoMapType> m_mapTypes;
    QMap<QString, int> m_mapNameToId;
    int m_scaleFactor;
};

QT_END_NAMESPACE

#endif // QGEOFILETILECACHEMAPBOX_H
