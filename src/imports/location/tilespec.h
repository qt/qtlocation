#ifndef TILESPEC_H
#define TILESPEC_H

class TileSpec
{
public:
    TileSpec();
    TileSpec(int zoom, int x, int y);

    void setZoom(int zoom);
    int zoom() const;

    void setX(int x);
    int x() const;

    void setY(int y);
    int y() const;

    bool operator < (const TileSpec &rhs) const;

private:
    int zoom_;
    int x_;
    int y_;
};

#endif
