#ifndef TILE_H
#define TILE_H

#include "tilespec.h"

#include <QPixmap>

class Tile
{
public:
    Tile();
    Tile(const TileSpec &spec, const QPixmap &pixmap);

    void setTileSpec(const TileSpec &spec);
    TileSpec tileSpec() const;

    void setPixmap(const QPixmap &pixmap);
    QPixmap pixmap() const;

private:
    TileSpec spec_;
    QPixmap pixmap_;    
};

#endif
