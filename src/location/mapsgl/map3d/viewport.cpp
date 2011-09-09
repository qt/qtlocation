/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
#include "viewport_p.h"

#include "sphere_p.h"

#include <QLinkedList>
#include <QPair>
#include <QMutableLinkedListIterator>

#include <qglscenenode.h>
#include <qglbuilder.h>
#include <qglmaterial.h>
#include <qglsphere.h>

#include <cmath>

QT_BEGIN_NAMESPACE

Viewport::Viewport()
    : viewportTiles_(ViewportTiles(1))
{
//    viewportTiles_.setSphere(sphere_);
}

Viewport::~Viewport()
{
}

void Viewport::setProjection(Projection3D *projection)
{
    viewportCamera_.setProjection(projection);
}

void Viewport::setZoomLevel(int zoomLevel)
{
    viewportTiles_.setZoomLevel(zoomLevel);
}

int Viewport::zoomLevel() const
{
    return viewportTiles_.zoomLevel();
}

//void Viewport::setSphere(const SphereGeometry& sphere)
//{
//    if (sphere_ == sphere)
//        return;

//    sphere_ = sphere;
//    viewportTiles_.setSphere(sphere_);

//    calculateVisibleTiles();
//}

//SphereGeometry Viewport::sphere() const
//{
//    return sphere_;
//}

/*
    Updates the internal QGLCamera, the frustum, and then updates the
    list of visible tiles.
*/
void Viewport::setCameraData(const CameraData &cameraData)
{
    viewportTiles_.setZoomLevel(cameraData.zoomLevel());
    viewportCamera_.setCameraData(cameraData);

    calculateVisibleTiles();
}

QGLCamera* Viewport::camera() const
{
    return viewportCamera_.toGLCamera();
}

QList<TileSpec> Viewport::visibleTiles() const
{
    return visibleTiles_;
}

void Viewport::calculateVisibleTiles()
{
    visibleTiles_ = viewportTiles_.visibleTiles(viewportCamera_);
}

QT_END_NAMESPACE
