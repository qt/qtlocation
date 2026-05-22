// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qdir.h>
#include <QtCore/qregularexpression.h>
#include <QtLocation/private/qgeotilespec_p.h>
#include <algorithm>
#include <qgeofiletilecacheohosmapkit.h>
#include <unordered_map>

QT_BEGIN_NAMESPACE

namespace {

constexpr int minScaleFactor = 1;
constexpr int maxScaleFactor = 2;

constexpr int fixedMapId = 0;

class QGeoFileTileCacheOhosMapKit : public QGeoFileTileCache
{
public:
    QGeoFileTileCacheOhosMapKit(
        int scaleFactor, const QString &directory, QObject *parent);

protected:
    QString tileSpecToFilename(
        const QGeoTileSpec &spec, const QString &format, const QString &directory) const override;
    QGeoTileSpec filenameToTileSpec(const QString &filename) const override;

private:
    int m_scaleFactor;
};

QGeoFileTileCacheOhosMapKit::QGeoFileTileCacheOhosMapKit(
    int scaleFactor, const QString &directory, QObject *parent)
    : QGeoFileTileCache(directory, parent)
    , m_scaleFactor(qBound(minScaleFactor, scaleFactor, maxScaleFactor))
{
}

QString QGeoFileTileCacheOhosMapKit::tileSpecToFilename(
    const QGeoTileSpec &spec, const QString &format, const QString &outputDir) const
{
    return QDir(outputDir).filePath(
        QString("%1-%2-%3-%4-%5-@%6x.%7")
            .arg(spec.plugin())
            .arg(fixedMapId)
            .arg(spec.zoom())
            .arg(spec.x())
            .arg(spec.y())
            .arg(m_scaleFactor)
            .arg(format));
}

QGeoTileSpec QGeoFileTileCacheOhosMapKit::filenameToTileSpec(const QString &filename) const
{
    QRegularExpression tileCacheFilenameMatchingRegexp(
        QStringLiteral(
            R"(^(?<pluginName>.*?)-(?<mapId>\d+)-(?<zoom>\d+)-(?<x>\d+)-(?<y>\d+)-@(?<scale>\d+)x)"));

    std::unordered_map<const char *, int> matchIntegerValues = {
        {"mapId", 0},
        {"zoom", 0},
        {"x", 0},
        {"y", 0},
        {"scale", 0},
    };

    QRegularExpressionMatch match = tileCacheFilenameMatchingRegexp.match(filename);

    bool matchValuesValid = false;
    if (match.hasMatch()) {
        matchValuesValid = true;
        for (auto &kv : matchIntegerValues) {
            bool ok;
            kv.second = match.captured(kv.first).toInt(&ok);
            if (!ok) {
                matchValuesValid = false;
                break;
            }
        }
    }

    constexpr int noVersionSpecified = -1;
    return matchValuesValid && matchIntegerValues["scale"] == m_scaleFactor
        ? QGeoTileSpec(
            match.captured("pluginName"),
            matchIntegerValues["mapId"],
            matchIntegerValues["zoom"],
            matchIntegerValues["x"],
            matchIntegerValues["y"],
            noVersionSpecified)
        : QGeoTileSpec();
}

}

QGeoFileTileCache *makeGeoFileTileCacheOhosMapKit(int scaleFactor, const QString &directory, QObject *parent)
{
    return new QGeoFileTileCacheOhosMapKit(scaleFactor, directory, parent);
}

QT_END_NAMESPACE
