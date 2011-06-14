
#include "tilespec.h"

TileSpec::TileSpec()
    : zoom_(-1),
      x_(-1),
      y_(-1) {}

TileSpec::TileSpec(int zoom, int x, int y)
    : zoom_(zoom),
    x_(x),
    y_(y) {}

void TileSpec::setZoom(int zoom)
{
    zoom_ = zoom;
}

int TileSpec::zoom() const
{
    return zoom_;
}

void TileSpec::setX(int x)
{
    x_ = x;
}

int TileSpec::x() const
{
    return x_;
}

void TileSpec::setY(int y)
{
    y_ = y;
}

int TileSpec::y() const
{
    return y_;
}

bool TileSpec::operator < (const TileSpec &rhs) const
{
    if (zoom_ < rhs.zoom_)
        return true;
    if (zoom_ > rhs.zoom_)
        return false;
    if (x_ < rhs.x_)
        return true;
    if (x_ > rhs.x_)
        return false;
    return (y_ < rhs.y_);

}
