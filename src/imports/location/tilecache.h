#ifndef TILECACHE_H
#define TILECACHE_H

#include <QObject>
#include <QMap>

#include "tilespec.h"
#include "tile.h"
#include "tileprovider.h"

class TileCache : public QObject
{
    Q_OBJECT
public:
    TileCache(const QString &directory = QString(), QObject *parent = 0);

    void prefetch(const TileSpec &spec);
    void waitForPrefetchedTiles();

    Tile get(const TileSpec &spec) const;

signals:
    void prefetchingFinished();

private slots:
    void insert(const Tile &tile);
    void handleError(const TileSpec &spec);

private:
    void loadTiles();

    void updateWaitingStatus();

    QString tileSpecToFilename(const TileSpec &spec) const;
    TileSpec filenameToTileSpec(const QString &filename) const;

    QString directory_;
    TileProvider *provider_;

    bool waitingForPrefetchedTiles_;
    int tilesOutstanding_;

    QMap<TileSpec, Tile> cache_;
};

#endif // TILECACHE_H
