#ifndef TILESPHERE_H
#define TILESPHERE_H

#include <QObject>

class QGLSceneNode;
class QGeometryData;

class Tile;
class TileCache;

class TileSphere : public QObject
{
    Q_OBJECT
public:
    TileSphere(QObject *parent, TileCache *tileCache, int zoom = 4, double radius = 1.0);

    void setZoom(int zoom);
    int zoom() const;

    void setRadius(double radius);
    double radius() const;

    void limitView(int x1, int y1, int x2, int y2);
    void clearLimit();

    QGLSceneNode* sphereSceneNode() const;

public slots:
    void update();

private slots:
    void prefetchingFinished();

signals:
    void sphereUpdated();

private:
    QGeometryData tileSpecToSphereQuad(int x, int y) const;
    QGLSceneNode* tileToSceneNode(const Tile &tile) const;

    void prefetchSphere() const;
    void buildSphere() const;

    TileCache *tileCache_;
    int zoom_;
    double radius_;

    bool viewLimited_;
    int viewX1_;
    int viewY1_;
    int viewX2_;
    int viewY2_;

    QGLSceneNode* sphereNode_;
};

#endif // TILESPHERE_H
