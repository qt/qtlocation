/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
#include "mapitem.h"

#include <Qt3D/qglscenenode.h>
#include <Qt3D/qgltexture2d.h>
#include <Qt3D/qglmaterial.h>

MapItem::MapItem()
        : zoom_(8.0),
        visible_(true),
        textureId_(0), // invalid value according to specs
        textureDirty_(false),
        glResources_(0),
        needsUpdate_(false)
{
}

MapItem::~MapItem()
{
    // map item will not release the GL resources, but it is Map's responsibility
    // to do that (needs to be done in correct thread)
}

void MapItem::setCoordinate(const QGeoCoordinate &coordinate)
{
    coordinate_ = coordinate;
}

QGeoCoordinate MapItem::coordinate() const
{
    return coordinate_;
}

void MapItem::setAnchor(const QPointF &anchor)
{
    anchor_ = anchor;
}

MapItemGLResources* MapItem::glResources()
{
    return glResources_;
}

void MapItem::setGLResources(MapItemGLResources* resources)
{
    glResources_ = resources;
}

void MapItem::setTextureId(GLuint textureId)
{
    needsUpdate_ = true;
    if (textureId_ == textureId)
        return;
    textureId_ = textureId;
    textureDirty_ = true;
}

GLuint MapItem::textureId()
{
    return textureId_;
}

// Note: this function 'must be only' / 'is guaranteed only to be called'
// when running in the thread that has valid GL context (rendering thread).
void MapItem::update()
{
    if (!textureDirty_ || !glResources_->sceneNode)
        return;
    textureDirty_ = false;
    // TODO do we want to provide some default texture if we don't have a texture ID?
    // Also: prepare for case when texture ID is invalidated (after first being valid).
    if (textureId_ != 0) {
        glResources_->texture = QGLTexture2D::fromTextureId(textureId_, size_.toSize()); // todo optimize out the .toSize() call
        glResources_->sceneNode->material()->setTexture(glResources_->texture);
        glResources_->sceneNode->setEffect(QGL::LitDecalTexture2D);
    }
}

void MapItem::setNeedsUpdate(bool needsUpdate)
{
    needsUpdate_ = needsUpdate;
}

bool MapItem::needsUpdate() const
{
    return needsUpdate_;
}

QGLTexture2D* MapItem::texture()
{
    return glResources_->texture;
}

QPointF MapItem::anchor() const
{
    return anchor_;
}

void MapItem::setSize(const QSizeF &size)
{
    size_ = size;
}

QSizeF MapItem::size() const
{
    return size_;
}

void MapItem::setZoom(double zoom)
{
    zoom_ = zoom;
}

double MapItem::zoom() const
{
    return zoom_;
}

void MapItem::setSceneNode(QGLSceneNode *sceneNode)
{
    glResources_->sceneNode = sceneNode;
}

QGLSceneNode* MapItem::sceneNode() const
{
    if (!glResources_)
        return 0;
    return glResources_->sceneNode;
}

// TODO make this into a property and update the item tree when it changes?
void MapItem::setBounds(const QRect &bounds)
{
    bounds_ = bounds;
}

QRect MapItem::bounds() const
{
    return bounds_;
}

void MapItem::setVisibleFromViewport(bool visible)
{
    visible_ = visible;
}

bool MapItem::visibleFromViewport() const
{
    return visible_;
}
