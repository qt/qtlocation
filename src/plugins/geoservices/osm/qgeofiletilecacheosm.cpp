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

QGeoFileTileCacheOsm::QGeoFileTileCacheOsm(const QVector<QGeoTileProviderOsm *> &providers, const QString &offlineDirectory, const QString &directory, QObject *parent)
:   QGeoFileTileCache(directory, parent), m_offlineDirectory(offlineDirectory),
     m_requestCancel(0), m_providers(providers)
{
    m_highDpi.resize(providers.size());
    for (int i = 0; i < providers.size(); i++) {
        m_highDpi[i] = providers[i]->isHighDpi();
        connect(providers[i], &QGeoTileProviderOsm::resolutionFinished, this, &QGeoFileTileCacheOsm::onProviderResolutionFinished);
        connect(providers[i], &QGeoTileProviderOsm::resolutionError, this, &QGeoFileTileCacheOsm::onProviderResolutionFinished);
    }
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

void QGeoFileTileCacheOsm::onProviderResolutionFinished(const QGeoTileProviderOsm *provider)
{
    Q_UNUSED(provider)
    for (int i = 0; i < m_providers.size(); i++) {
        if (m_providers[i]->isHighDpi() != m_highDpi[i]) {
            int mapId = m_providers[i]->mapType().mapId();
            m_highDpi[i] = m_providers[i]->isHighDpi();

            // reload cache for mapId i
            dropTiles(mapId);
            loadTiles(mapId);

            // send signal to clear scene in all maps created through this provider that use the reloaded tiles
            emit mapDataUpdated(mapId);
        }
    }
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

void QGeoFileTileCacheOsm::dropTiles(int mapId)
{
    QList<QGeoTileSpec> keys;
    keys = textureCache_.keys();
    for (const QGeoTileSpec &k : keys)
        if (k.mapId() == mapId)
            textureCache_.remove(k);

    keys = memoryCache_.keys();
    for (const QGeoTileSpec &k : keys)
        if (k.mapId() == mapId)
            memoryCache_.remove(k);

    keys = diskCache_.keys();
    for (const QGeoTileSpec &k : keys)
        if (k.mapId() == mapId)
            diskCache_.remove(k);
}

void QGeoFileTileCacheOsm::loadTiles(int mapId)
{
    QStringList formats;
    formats << QLatin1String("*.*");

    QDir dir(directory_);
    QStringList files = dir.entryList(formats, QDir::Files);

    for (int i = 0; i < files.size(); ++i) {
        QGeoTileSpec spec = filenameToTileSpec(files.at(i));
        if (spec.zoom() == -1 || spec.mapId() != mapId)
            continue;
        QString filename = dir.filePath(files.at(i));
        addToDiskCache(spec, filename);
    }
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

QString QGeoFileTileCacheOsm::tileSpecToFilename(const QGeoTileSpec &spec, const QString &format, const QString &directory) const
{
    int providerId = spec.mapId() - 1;
    if (providerId < 0 || providerId >= m_providers.size())
        return QString();
    QString filename = spec.plugin();
    filename += QLatin1String("-");
    filename += (m_providers[providerId]->isHighDpi()) ? QLatin1Char('h') : QLatin1Char('l');
    filename += QLatin1String("-");
    filename += QString::number(spec.mapId());
    filename += QLatin1String("-");
    filename += QString::number(spec.zoom());
    filename += QLatin1String("-");
    filename += QString::number(spec.x());
    filename += QLatin1String("-");
    filename += QString::number(spec.y());

    //Append version if real version number to ensure backwards compatibility and eviction of old tiles
    if (spec.version() != -1) {
        filename += QLatin1String("-");
        filename += QString::number(spec.version());
    }

    filename += QLatin1String(".");
    filename += format;

    QDir dir = QDir(directory);

    return dir.filePath(filename);
}

QGeoTileSpec QGeoFileTileCacheOsm::filenameToTileSpec(const QString &filename) const
{
    QGeoTileSpec emptySpec;

    QStringList parts = filename.split('.');

    if (parts.length() != 2)
        return emptySpec;

    QString name = parts.at(0);
    QStringList fields = name.split('-');

    int length = fields.length();
    if (length != 6 && length != 7)
        return emptySpec;

    QList<int> numbers;

    bool ok = false;
    for (int i = 2; i < length; ++i) {
        ok = false;
        int value = fields.at(i).toInt(&ok);
        if (!ok)
            return emptySpec;
        numbers.append(value);
    }

    bool highDpi = m_providers[numbers.at(0) - 1]->isHighDpi();
    if (highDpi && fields.at(1) != QLatin1Char('h'))
        return emptySpec;
    else if (!highDpi && fields.at(1) != QLatin1Char('l'))
        return emptySpec;

    //File name without version, append default
    if (numbers.length() < 5)
        numbers.append(-1);

    return QGeoTileSpec(fields.at(0),
                    numbers.at(0),
                    numbers.at(1),
                    numbers.at(2),
                    numbers.at(3),
                    numbers.at(4));
}

QT_END_NAMESPACE
