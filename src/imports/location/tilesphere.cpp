#include "tilesphere.h"

#include "tilecache.h"
#include "tile.h"

#include <qgeometrydata.h>
#include <qglscenenode.h>
#include <qglmaterial.h>
#include <qglbuilder.h>

#include <cmath>

TileSphere::TileSphere(QObject *parent, TileCache *tileCache, int zoom, double radius)
    : QObject(parent),
      tileCache_(tileCache),
      zoom_(zoom),
      radius_(radius),
      viewLimited_(false)
{
    viewX1_ = 0;
    viewY1_ = 0;
    viewX2_ = (1 << zoom_) - 1;
    viewY2_ = (1 << zoom_) - 1;

    sphereNode_ = new QGLSceneNode(this);
    connect(tileCache_,
            SIGNAL(prefetchingFinished()),
            this,
            SLOT(prefetchingFinished()));
}

void TileSphere::setZoom(int zoom)
{
    if (zoom_ == zoom)
        return;

    zoom_ = zoom;
}

int TileSphere::zoom() const
{
    return zoom_;
}

void TileSphere::setRadius(double radius)
{
    if (radius_ == radius)
        return;

    radius_ = radius;
}

double TileSphere::radius() const
{
    return radius_;
}

void TileSphere::limitView(int x1, int y1, int x2, int y2)
{
    viewLimited_ = true;
    viewX1_ = x1;
    viewY1_ = y1;
    viewX2_ = x2;
    viewY2_ = y2;
}

void TileSphere::clearLimit()
{
    if (!viewLimited_)
        return;

    viewLimited_ = false;
    viewX1_ = 0;
    viewY1_ = 0;
    viewX2_ = (1 << zoom_) - 1;
    viewY2_ = (1 << zoom_) - 1;
}

QGLSceneNode* TileSphere::sphereSceneNode() const
{
    return sphereNode_;
}

void TileSphere::update()
{
    prefetchSphere();
    tileCache_->waitForPrefetchedTiles();
}

void TileSphere::prefetchingFinished()
{
    qDebug() << __FUNCTION__ << "prefetching is finished";
    buildSphere();
    emit sphereUpdated();
}

QGeometryData TileSphere::tileSpecToSphereQuad(int x, int y) const
{
    QGeometryData data;

    const double pi = 3.14159265358;

    double z = 1 << zoom_;

    double lon1 = 2.0 * pi * x / z;
    double lon2 = 2.0 * pi * (x + 1) / z;
    double lat1 = pi * y / z;
    double lat2 = pi * (y + 1) / z;

    double cosLat1 = cos(lat1);
    double cosLat2 = cos(lat2);
    double cosLon1 = cos(pi/2 - lon1);
    double cosLon2 = cos(pi/2 - lon2);

    double sinLat1 = sin(lat1);
    double sinLat2 = sin(lat2);
    double sinLon1 = sin(pi/2 - lon1);
    double sinLon2 = sin(pi/2 - lon2);

    // y and z are flipped here to move from
    // "earth" geometry to GL geometry
    QVector3D v11 = QVector3D(cosLon1 * sinLat1,
                              cosLat1,
                              sinLon1 * sinLat1);

    QVector3D v12 = QVector3D(cosLon1 * sinLat2,
                              cosLat2,
                              sinLon1 * sinLat2);

    QVector3D v21 = QVector3D(cosLon2 * sinLat1,
                              cosLat1,
                              sinLon2 * sinLat1);

    QVector3D v22 = QVector3D(cosLon2 * sinLat2,
                              cosLat2,
                              sinLon2 * sinLat2);


    data.appendVertex(radius_ * v11);
    data.appendNormal(v11);
    data.appendTexCoord(QVector2D(0, 1));

    data.appendVertex(radius_ * v12);
    data.appendNormal(v12);
    data.appendTexCoord(QVector2D(0, 0));

    data.appendVertex(radius_ * v22);
    data.appendNormal(v22);
    data.appendTexCoord(QVector2D(1, 0));

    data.appendVertex(radius_ * v21);
    data.appendNormal(v21);
    data.appendTexCoord(QVector2D(1, 1));

    return data;
}

QGLSceneNode* TileSphere::tileToSceneNode(const Tile &tile) const
{
    if (tile.tileSpec().zoom() == -1)
        return 0;

    QGLBuilder builder;
    builder.addQuads(tileSpecToSphereQuad(tile.tileSpec().x(), tile.tileSpec().y()));

    QGLSceneNode *node = builder.finalizedSceneNode();

    QGLMaterial *mat = new QGLMaterial(node);
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setPixmap(tile.pixmap());
    mat->setTexture(tex);

    node->setMaterial(mat);
    node->setEffect(QGL::LitDecalTexture2D);
    return node;
}

void TileSphere::prefetchSphere() const
{
    int z = 1 << zoom_;

    int x1 = viewX1_;
    int x2 = viewX2_;
    if (x2 < x1)
        x2 += z;

    int y1 = viewY1_;
    int y2 = viewY2_;
    if (y2 < y1)
        y2 += z;

    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            tileCache_->prefetch(TileSpec(zoom_, x % z, y % z));
        }
    }
}

void TileSphere::buildSphere() const
{
    sphereNode_->removeNodes(sphereNode_->children());

    int z = 1 << zoom_;

    int x1 = viewX1_;
    int x2 = viewX2_;
    if (x2 < x1)
        x2 += z;

    int y1 = viewY1_;
    int y2 = viewY2_;
    if (y2 < y1)
        y2 += z;

    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            QGLSceneNode *node = tileToSceneNode(tileCache_->get(TileSpec(zoom_, x % z, y % z)));
            if (node)
                sphereNode_->addNode(node);
        }
    }
}
