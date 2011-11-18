/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tileview.h"

#include "tilecache.h"
#include "tile.h"
#include "cameradata.h"

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
#include <qopenglcontext.h>

#include <cmath>

#include <QDebug>


static void ensureContext(QWindow &win, QOpenGLContext &ctx)
{
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    ctx.setFormat(format);
    ctx.create();
    // TODO: is it possible that the platform will downgrade the actual
    // format, or will it just fail if it can't deliver the actual format
    format = ctx.format();
    ctx.makeCurrent(&win);
}

TileView::TileView(TileCache *tileCache) :
        tileCache_(tileCache)
{
    serviceProvider_ = new QGeoServiceProvider("nokia");
    map_ = new Map(tileCache, this);
    map_->setMappingManager(serviceProvider_->mappingManager());

    connect(map_,
            SIGNAL(updateRequired()),
            this,
            SLOT(update()));

    setCamera(map_->glCamera());
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
    Q_UNUSED(painter);
    qDebug() << "-----------------------" << __FUNCTION__;
    // TODO fixme

    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid()) {
        qWarning("Example will not render; GL implementation not valid.");
        return;
    }
    QGLPainter glpainter(&glw);
    glpainter.begin();
    map_->paintGL(&glpainter);
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

void TileView::wheelEvent(QWheelEvent *e)
{
    CameraData cameraData = map_->cameraData();
    if (e->delta() > 0) {
        if (e->modifiers() & Qt::ShiftModifier) {
            cameraData.setDistance(cameraData.distance() / 2.0);
            cameraData.setZoomLevel(cameraData.zoomLevel() + 1);
            map_->setCameraData(cameraData);
            if (!map_->autoUpdate())
                map_->update();
        } else {
            cameraData.setDistance(cameraData.distance() / 1.1);
        }
    } else {
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
    }
    map_->setCameraData(cameraData);
    update();

    e->accept();
}

void TileView::mousePressEvent(QMouseEvent *e)
{
    switch (e->button()) {
    case Qt::LeftButton:
        mousePos_ = e->pos();
//        QGeoCoordinate coord = map_->screenPositionToCoordinate(mousePos_);
//        qDebug() << __FUNCTION__
//                 << "mousePos " << mousePos_
//                 << " -> coord " << coord
//                 << " -> screen " << map_->coordinateToScreenPosition(coord);
        break;
    default:
        break;
    }
    e->accept();
}

void TileView::mouseReleaseEvent(QMouseEvent *)
{
}

void TileView::mouseMoveEvent(QMouseEvent *e)
{
    CameraData cameraData = map_->cameraData();

    if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        // Pixel pan support?!
        QPoint delta = (mousePos_ - e->pos()) / 50;

        QGeoCoordinate coord = cameraData.center();
        coord.setLatitude(coord.latitude() + delta.y());
        coord.setLongitude(coord.longitude() - delta.x());
        cameraData.setCenter(coord);

        map_->setCameraData(cameraData);
        update();
    }
    e->accept();
}
