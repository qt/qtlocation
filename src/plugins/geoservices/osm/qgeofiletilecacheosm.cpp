/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeofiletilecacheosm.h"
#include <QtLocation/private/qgeotilespec_p.h>
#include <QDir>
#include <QDirIterator>
#include <QPair>
#include <QDateTime>
#include <QtConcurrent>
#include <QThread>

QT_BEGIN_NAMESPACE

QGeoFileTileCacheOsm::QGeoFileTileCacheOsm(const QString &offlineDirectory, const QString &directory, QObject *parent)
    :QGeoFileTileCache(directory, parent), m_offlineDirectory(offlineDirectory),
     m_requestCancel(0)
{
}

QGeoFileTileCacheOsm::~QGeoFileTileCacheOsm()
{
    m_requestCancel = 1;
    m_future.waitForFinished();
}

QSharedPointer<QGeoTileTexture> QGeoFileTileCacheOsm::get(const QGeoTileSpec &spec)
{
    QSharedPointer<QGeoTileTexture> tt = getFromMemory(spec);
    if (tt)
        return tt;
    if (tt = getFromOfflineStorage(spec))
        return tt;
    return getFromDisk(spec);
}

void QGeoFileTileCacheOsm::init()
{
    QGeoFileTileCache::init();
    if (!m_offlineDirectory.isEmpty())
        m_future = QtConcurrent::run(this, &QGeoFileTileCacheOsm::initOfflineRegistry);
}

QSharedPointer<QGeoTileTexture> QGeoFileTileCacheOsm::getFromOfflineStorage(const QGeoTileSpec &spec)
{
    QMutexLocker locker(&storageLock);
    if (m_tilespecToOfflineFilepath.contains(spec)) {
        QFile file(m_tilespecToOfflineFilepath[spec]);
        file.open(QIODevice::ReadOnly);
        QByteArray bytes = file.readAll();
        file.close();
        locker.unlock();

        QImage image;
        if (!image.loadFromData(bytes)) {
            handleError(spec, QLatin1String("Problem with tile image"));
            return QSharedPointer<QGeoTileTexture>(0);
        }

        addToMemoryCache(spec, bytes, QString());
        QSharedPointer<QGeoTileTexture> tt = addToTextureCache(spec, image);
        if (tt)
            return tt;
    }

    return QSharedPointer<QGeoTileTexture>();
}

void QGeoFileTileCacheOsm::initOfflineRegistry()
{
    // Dealing with duplicates: picking the newest
    QMap<QString, QPair<QString, QDateTime> > fileDates; // key is filename, value is <filepath, lastmodified>
    QDirIterator it(m_offlineDirectory, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks );
    while (it.hasNext()) {
        const QString &path = it.next();
        QFileInfo f(path);
        if (!fileDates.contains(f.fileName()) || fileDates[f.fileName()].second < f.lastModified())
            fileDates[f.fileName()] = QPair<QString, QDateTime>(f.filePath(), f.lastModified());
        if (m_requestCancel)
            return;
    }

    int count = 0;
    for (auto i= fileDates.begin(); i != fileDates.end(); ++i) {
        QGeoTileSpec spec = filenameToTileSpec(i.key());
        if (spec.zoom() == -1)
            continue;
        count++;
        storageLock.lock();
        m_tilespecToOfflineFilepath[spec] = i.value().first;
        storageLock.unlock();
        if (m_requestCancel)
            return;
    }
    qWarning() << "OSM Offline tiles: "<<count;
}

QT_END_NAMESPACE
