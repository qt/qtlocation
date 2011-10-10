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
        glResources_(0)
{
    glResources_ = new MapItemGLResources;
    // TODO optimize the defaultMaterial be common for all
    glResources_->defaultMaterial = new QGLMaterial();
    glResources_->defaultMaterial->setColor(QColor(255, 128, 0));
    glResources_->defaultMaterial->setSpecularColor(QColor(255, 128, 0));
    glResources_->defaultMaterial->setDiffuseColor(QColor(255, 128, 0));
    glResources_->defaultMaterial->setShininess(1.0);
}

MapItem::~MapItem()
{
    // merely deletes the struct; map item will not release the
    // GL resources, but it is Map's responsibility to do that
    delete glResources_;
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
    glResources_ = 0;
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
    if (glResources_->sceneNode == sceneNode)
        return;
    glResources_->sceneNode = sceneNode;
    if (glResources_->sceneNode && !glResources_->sceneNode->material()) {
        // todo figure out default material setting if
        // texture for any reason disappears. this crashes:
        //sceneNode_->setMaterial(defaultMaterial_);
    }
}

QGLSceneNode* MapItem::sceneNode() const
{
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

MapItemTree::MapItemTree()
        : root_(new AAInterval2DTree<MapItem*>()) {}

MapItemTree::~MapItemTree()
{
    delete root_;
}

void MapItemTree::insert(MapItem *item)
{
    if (item && item->bounds().isValid())
        root_->insert(item->bounds(), item);
}

void MapItemTree::remove(MapItem *item)
{
    if (item && item->bounds().isValid())
        root_->remove(item->bounds(), item);
}

bool MapItemTree::isEmpty() const
{
    return root_->isEmpty();
}

int MapItemTree::size() const
{
    return root_->items().size();
}

QList<MapItem*> MapItemTree::items() const
{
    return root_->items();
}

QList<MapItem*> MapItemTree::itemsAt(const QPoint &point) const
{
    return root_->itemsAt(point);
}

QList<MapItem*> MapItemTree::itemsWithin(const QRect &viewport) const
{
    return root_->itemsWithin(viewport);
}

// TODO make static private
bool checkPoint(const QPoint &p, const QRect &r)
{
    return ((r.x() <= p.x())
            && (p.x() <= r.x() + r.width())
            && (r.y() <= p.y())
            && (p.y() <= r.y() + r.height()));
}

bool checkOverlap(const QRect& r1, const QRect &r2)
{
    return (((r1.x() <= r2.x() + r2.width()) && (r2.x() <= r1.x() + r1.width()))
            &&
            ((r1.y() <= r2.y() + r2.height()) && (r2.y() <= r1.y() + r1.height())));
}

int getIndex(const QPoint &p, const QRect &oldRec, const QRect &newRect)
{
    int result = 0;

    if (oldRec.isValid() && checkPoint(p, oldRec))
        result += 1;

    if (checkPoint(p, newRect))
        result += 2;

    return result;
}

void MapItemTree::makeVisible(const QRect& viewport, QList<MapItem*> &added, QList<MapItem*> &removed)
{
    added.clear();
    removed.clear();

    QList<int> x;

    if (viewport_.isValid()) {
        x << viewport_.x();
        if (viewport_.width() != 0)
            x << viewport_.x() + viewport_.width();
    }

    x << viewport.x();
    if (viewport.width() != 0)
        x << viewport.x() + viewport.width();

    qSort(x.begin(), x.end());

    for (int i = x.size() - 1; i > 0; --i) {
        int x1 = x.at(i);
        int x2 = x.at(i - 1);
        if (x1 == x2) {
            x.removeAt(i);
        }
    }

    QList<int> y;

    if (viewport_.isValid()) {
        y << viewport_.y();
        if (viewport_.height() != 0)
            y << viewport_.y() + viewport_.height();
    }

    y << viewport.y();
    if (viewport.height() != 0)
        y << viewport.y() + viewport.height();

    qSort(y.begin(), y.end());

    for (int i = y.size() - 1; i > 0; --i) {
        int y1 = y.at(i);
        int y2 = y.at(i - 1);
        if (y1 == y2) {
            y.removeAt(i);
        }
    }

    QList<QRect> wasVisible;
    QList<QRect> isVisible;
    QRect overlap;

    int xsize = x.size();
    int ysize = y.size();

    for (int j = 0; j < ysize - 1; ++j) {
        int y1 = y.at(j);
        int y2 = y.at(j + 1);
        for (int i = 0; i < xsize - 1; ++i) {
            int x1 = x.at(i);
            int x2 = x.at(i + 1);

            QRect r = QRect(x1, y1, x2 - x1, y2 - y1);

            QHash<int, int> counts;
            counts[0] = 0;
            counts[1] = 0;
            counts[2] = 0;
            counts[3] = 0;

            counts[getIndex(QPoint(x1, y1), viewport_, viewport)] += 1;
            counts[getIndex(QPoint(x1, y2), viewport_, viewport)] += 1;
            counts[getIndex(QPoint(x2, y1), viewport_, viewport)] += 1;
            counts[getIndex(QPoint(x2, y2), viewport_, viewport)] += 1;

            if (counts[0] != 0) {
                continue;
            }
            if (counts[3] == 4) {
                overlap = r;
                continue;
            }

            if (counts[1] != 0) {
                if (counts[2] != 0) {
                    continue;
                } else {
                    wasVisible << r;
                }
            } else {
                if (counts[2] != 0) {
                    isVisible << r;
                } else {
                    continue;
                }
            }
        }
    }

    for (int i = 0; i < wasVisible.size(); ++i) {

        QList<MapItem*> items = itemsWithin(wasVisible.at(i));

        for (int j = 0; j < items.size(); ++j) {

            MapItem *item = items.at(j);

            bool inMiddleGround = false;
            if (overlap.isValid())
                inMiddleGround = checkOverlap(item->bounds(), overlap);

            if (item->visibleFromViewport() && !inMiddleGround) {
                item->setVisibleFromViewport(false);
                removed << item;
            }
        }
    }

    if (overlap.isValid())
        isVisible << overlap;

    for (int i = 0; i < isVisible.size(); ++i) {

        QList<MapItem*> items = itemsWithin(isVisible.at(i));

        for (int j = 0; j < items.size(); ++j) {

            MapItem *item = items.at(j);

            if (!item->visibleFromViewport()) {
                item->setVisibleFromViewport(true);
                added << item;
            }
        }
    }

    viewport_ = viewport;
}
