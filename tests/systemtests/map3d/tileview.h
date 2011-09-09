#ifndef TILEVIEW_H
#define TILEVIEW_H

#include <qglview.h>

#include "cameradata.h"
#include "map.h"

class QGLPainter;
class QGLSceneNode;
class TileCache;
class Tile;
class TileSpec;
class MapSphere;
class QGeoServiceProvider;
class ScriptEngine;

class TileView : public QGLView
{
    Q_OBJECT
public:
    explicit TileView(TileCache *tileCache, QWidget *parent = 0);
    ~TileView();

    Map *map();

    void paintGL(QGLPainter *painter);

protected:
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *);

//    void wheelEvent(QWheelEvent *e);
//    void mousePressEvent(QMouseEvent *e);
//    void mouseReleaseEvent(QMouseEvent *e);
//    void mouseMoveEvent(QMouseEvent *e);

private:
    void updateAspectRatio();

    QGeoServiceProvider *serviceProvider_;
    TileCache *tileCache_;
    Map *map_;
    QPoint mousePos_;
};

#endif // TILEVIEW_H
