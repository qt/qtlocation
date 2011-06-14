
#include "tile.h"

Tile::Tile() {}
Tile::Tile(const TileSpec &spec, const QPixmap &pixmap)
    : spec_(spec),
    pixmap_(pixmap) {}

void Tile::setTileSpec(const TileSpec &spec)
{
    spec_ = spec;
}

TileSpec Tile::tileSpec() const
{
    return spec_;
}

void Tile::setPixmap(const QPixmap &pixmap)
{
    pixmap_ = pixmap;
}

QPixmap Tile::pixmap() const
{
    return pixmap_;
}
