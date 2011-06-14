#include "tilecache.h"

#include <QDir>
#include <QRegExp>

#include <QDebug>

TileCache::TileCache(const QString &directory,
                     QObject *parent)
    : QObject(parent),
      directory_(directory),
      waitingForPrefetchedTiles_(false),
      tilesOutstanding_(0)
{
    if (directory_.isEmpty()) {
        QDir home = QDir::home();
        if (!home.exists(".tilecache"))
            home.mkdir(".tilecache");
        directory_ = home.filePath(".tilecache");
    }

    provider_ = new TileProvider(this);
    connect(provider_,
            SIGNAL(tileReady(Tile)),
            this,
            SLOT(insert(Tile)));
    connect(provider_,
            SIGNAL(tileError(TileSpec)),
            this,
            SLOT(handleError(TileSpec)));
    loadTiles();
}

void TileCache::prefetch(const TileSpec &spec)
{
    if (!cache_.contains(spec)) {
        provider_->tileRequest(spec);
        ++tilesOutstanding_;
    }
}

void TileCache::waitForPrefetchedTiles()
{
    if (tilesOutstanding_ == 0)
        emit prefetchingFinished();
    else
        waitingForPrefetchedTiles_ = true;
}

Tile TileCache::get(const TileSpec &spec) const
{
    return cache_.value(spec);
}

void TileCache::insert(const Tile &tile)
{
    QString filename = tileSpecToFilename(tile.tileSpec());
    tile.pixmap().save(filename, "png");
    cache_.insert(tile.tileSpec(), tile);

    updateWaitingStatus();
}

void TileCache::handleError(const TileSpec &)
{
    updateWaitingStatus();
}

void TileCache::updateWaitingStatus()
{
    --tilesOutstanding_;

    if (waitingForPrefetchedTiles_ && (tilesOutstanding_ == 0)) {
        emit prefetchingFinished();
        waitingForPrefetchedTiles_ = false;
    }
}

void TileCache::loadTiles()
{
    QStringList formats;
    formats << "*.png";

    QDir dir(directory_);
    QStringList files = dir.entryList(formats);

    for (int i = 0; i < files.size(); ++i) {
        TileSpec spec = filenameToTileSpec(files.at(i));
        if (spec.zoom() == -1)
            continue;

        QPixmap pixmap;
        pixmap.load(dir.filePath(files.at(i)), "png");
        Tile tile(spec, pixmap);
        cache_.insert(tile.tileSpec(), tile);
    }
}

QString TileCache::tileSpecToFilename(const TileSpec &spec) const
{
    QString filename = QString::number(spec.zoom());
    filename += "-";
    filename += QString::number(spec.x());
    filename += "-";
    filename += QString::number(spec.y());
    filename += ".png";

    QDir dir = QDir(directory_);

    return dir.filePath(filename);
}

TileSpec TileCache::filenameToTileSpec(const QString &filename) const
{
    TileSpec spec;
    QRegExp r("(\\d+)-(\\d+)-(\\d+).png");

    int index = r.indexIn(filename);
    if (index != -1) {
        bool ok = false;

        int zoom = r.cap(1).toInt(&ok);
        if (!ok)
            return spec;

        ok = false;
        int x = r.cap(2).toInt(&ok);
        if (!ok)
            return spec;

        ok = false;
        int y = r.cap(3).toInt(&ok);
        if (!ok)
            return spec;

        spec.setZoom(zoom);
        spec.setX(x);
        spec.setY(y);
    }

    return spec;
}
