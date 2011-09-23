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
      sceneNode_(0),
      textureId_(0), // invalid value according to specs
      textureDirty_(false),
      texture_(0),
      defaultMaterial_(0)
{
    // TODO optimize the defaultMaterial be common for all
    defaultMaterial_ = new QGLMaterial();
    defaultMaterial_->setColor(QColor(255, 128, 0));
    defaultMaterial_->setSpecularColor(QColor(255, 128, 0));
    defaultMaterial_->setDiffuseColor(QColor(255, 128, 0));
    defaultMaterial_->setShininess(1.0);
}

MapItem::~MapItem()
{
    delete defaultMaterial_;
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

void MapItem::setTextureId(GLuint textureId)
{
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
// TODO refactor this function, poorly written
void MapItem::update()
{
    if (!textureDirty_ || !sceneNode_)
        return;
    textureDirty_ = false;
    if (texture_) {
        // todo this will likely crash..
        sceneNode_->material()->setTexture(0);
        sceneNode_->material()->setColor(QColor(255, 128, 0));
        sceneNode_->material()->setSpecularColor(QColor(255, 128, 0));
        sceneNode_->material()->setDiffuseColor(QColor(255, 128, 0));
        sceneNode_->material()->setShininess(1.0);
        sceneNode_->setEffect(QGL::LitMaterial);
        delete texture_;
        texture_ = 0;
    }
    if (textureId_ != 0) {
        texture_ = QGLTexture2D::fromTextureId(textureId_, size_.toSize()); // todo optimize out the .toSize() call
        sceneNode_->material()->setTexture(texture_);
        sceneNode_->setEffect(QGL::LitDecalTexture2D);
    }
}

QGLTexture2D* MapItem::texture()
{
    return texture_;
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
    if (sceneNode_ == sceneNode)
        return;
    sceneNode_ = sceneNode;
    if (sceneNode_ && !sceneNode_->material()) {
        // todo figure out default material setting if
        // texture for any reason disappears. this crashes:
        //sceneNode_->setMaterial(defaultMaterial_);
    }
}

QGLSceneNode* MapItem::sceneNode() const
{
    return sceneNode_;
}
