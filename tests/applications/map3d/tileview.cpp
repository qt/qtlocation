/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tileview.h"

#include "tilecache.h"
#include "tile.h"
#include "cameradata.h"
#include "mapitem.h"

#include "qgeocoordinate.h"

#include "qgeoserviceprovider.h"
#include "qgeomappingmanager.h"

#include <qglscenenode.h>
#include <qglbuilder.h>
#include <qgeometrydata.h>

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>

#include <QColor>

#include <QPropertyAnimation>

#include <cmath>

#include <QDebug>

TileView::TileView(TileCache *tileCache, QWidget *parent) :
        QGLView(parent),
        tileCache_(tileCache)
{
    serviceProvider_ = new QGeoServiceProvider("nokia");
    map_ = new Map(tileCache, this);
    map_->setMappingManager(serviceProvider_->mappingManager());
    map_->setAutoUpdate(false);

    connect(map_,
            SIGNAL(updateRequired()),
            this,
            SLOT(update()));

    setCamera(map_->glCamera());


    MapItem *item1 = new MapItem;
    item1->setCoordinate(QGeoCoordinate(-27, 153));
    item1->setSize(QSizeF(128, 128));
    item1->setZoom(5);
    map_->addMapItem(item1);

    MapItem *item2 = new MapItem;
    item2->setCoordinate(QGeoCoordinate(-27, 153));
    item2->setSize(QSizeF(128, 128));
    item2->setAnchor(QPointF(128, 128));
    item2->setZoom(5);
    map_->addMapItem(item2);

    MapItem *item3 = new MapItem;
    item3->setCoordinate(QGeoCoordinate(-27, 153));
    item3->setSize(QSizeF(256.0, 256.0));
    item3->setAnchor(QPointF(-256.0, -256.0));
    item3->setZoom(6);
    map_->addMapItem(item3);
}

TileView::~TileView()
{
    delete serviceProvider_;
}

Map* TileView::map()
{
    return map_;
}

void TileView::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

void TileView::paintGL(QGLPainter *painter)
{
    map_->paintGL(painter);
//    QGLSceneNode *node = map_->sceneNodeForRendering();
//    if (node) {
//        node->draw(painter);
//        map_->sceneNodeRenderingDone();
//    }
}

void TileView::showEvent(QShowEvent *)
{
    updateAspectRatio();
}

void TileView::resizeEvent(QResizeEvent *event)
{
    QGLView::resizeEvent(event);
    updateAspectRatio();
}

void TileView::updateAspectRatio()
{
    map_->resize(width(), height());
}

void TileView::enterEvent(QEvent *)
{
    setFocus();
    grabKeyboard();
}

void TileView::leaveEvent(QEvent *)
{
    releaseKeyboard();
}

void TileView::keyPressEvent(QKeyEvent *e)
{
    CameraData cameraData = map_->cameraData();
    if (e->key() == Qt::Key_Left) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLongitude(coord.longitude() - 1);
            cameraData.setCenter(coord);
        } else {
            cameraData.setBearing(cameraData.bearing() - 5.0);
        }
    } else if (e->key() == Qt::Key_Right) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLongitude(coord.longitude() + 1);
            cameraData.setCenter(coord);
        } else {
            cameraData.setBearing(cameraData.bearing() + 5.0);
        }
    } else if (e->key() == Qt::Key_Up) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLatitude(coord.latitude() + 1);
            cameraData.setCenter(coord);
        } else {
            cameraData.setTilt(cameraData.tilt() - 5.0);
        }
    } else if (e->key() == Qt::Key_Down) {
        if (e->modifiers() & Qt::ShiftModifier) {
            QGeoCoordinate coord = cameraData.center();
            coord.setLatitude(coord.latitude() - 1);
            cameraData.setCenter(coord);
        } else {
            cameraData.setTilt(cameraData.tilt() + 5.0);
        }
    } else if (e->key() == Qt::Key_Plus) {
        if (e->modifiers() & Qt::ShiftModifier) {
            cameraData.setDistance(cameraData.distance() / 2.0);
            cameraData.setZoomLevel(cameraData.zoomLevel() + 1);
            map_->setCameraData(cameraData);
            if (!map_->autoUpdate())
                map_->update();
        } else {
            cameraData.setDistance(cameraData.distance() / 1.1);
        }
    } else if (e->key() == Qt::Key_Minus) {
        if (e->modifiers() & Qt::ShiftModifier) {
            if (cameraData.zoomLevel() != 1)
                cameraData.setZoomLevel(cameraData.zoomLevel() - 1);
            cameraData.setDistance(cameraData.distance() * 2.0);
            map_->setCameraData(cameraData);
            if (!map_->autoUpdate())
                map_->update();
        } else {
            cameraData.setDistance(cameraData.distance() * 1.1);
        }
    } else if (e->key() == Qt::Key_U) {
        map_->setCameraData(cameraData);
        if (!map_->autoUpdate())
            map_->update();
    }
    map_->setCameraData(cameraData);
    update();
}

//void TileView::wheelEvent(QWheelEvent *e)
//{
//    CameraData cameraData = map_->cameraData();
//    if (e->delta() > 0) {
//        if (e->modifiers() & Qt::ShiftModifier) {
//            cameraData.setDistance(cameraData.distance() / 2.0);
//            cameraData.setZoomLevel(cameraData.zoomLevel() + 1);
//            map_->setCameraData(cameraData);
//            if (!map_->autoUpdate())
//                map_->update();
//        } else {
//            cameraData.setDistance(cameraData.distance() / 1.1);
//        }
//    } else {
//        if (e->modifiers() & Qt::ShiftModifier) {
//            if (cameraData.zoomLevel() != 1)
//                cameraData.setZoomLevel(cameraData.zoomLevel() - 1);
//            cameraData.setDistance(cameraData.distance() * 2.0);
//            map_->setCameraData(cameraData);
//            if (!map_->autoUpdate())
//                map_->update();
//        } else {
//            cameraData.setDistance(cameraData.distance() * 1.1);
//        }
//    }
//    map_->setCameraData(cameraData);
//    update();

//    e->accept();
//}
//
//void TileView::mousePressEvent(QMouseEvent *e)
//{
//    switch (e->button()) {
//    case Qt::LeftButton:
//        mousePos_ = e->pos();
////        QGeoCoordinate coord = map_->screenPositionToCoordinate(mousePos_);
////        qDebug() << __FUNCTION__
////                 << "mousePos " << mousePos_
////                 << " -> coord " << coord
////                 << " -> screen " << map_->coordinateToScreenPosition(coord);
//        break;
//    }
//    e->accept();
//}

//void TileView::mouseReleaseEvent(QMouseEvent *e)
//{
//}

//void TileView::mouseMoveEvent(QMouseEvent *e)
//{
//    CameraData cameraData = map_->cameraData();

//    if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)
//    {
//        // Pixel pan support?!
//        QPoint delta = (mousePos_ - e->pos()) / 50;

//        QGeoCoordinate coord = cameraData.center();
//        coord.setLatitude(coord.latitude() + delta.y());
//        coord.setLongitude(coord.longitude() - delta.x());
//        cameraData.setCenter(coord);

//        map_->setCameraData(cameraData);
//        update();
//    }
//    e->accept();
//}
