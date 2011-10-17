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
#ifndef MAPITEM_H
#define MAPITEM_H

#include "qgeocoordinate.h"
#include <QPointF>
#include <QSizeF>
#include <QtOpenGL/qgl.h>

#include "intervaltree_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Location)

class QGLTexture2D;
class QGLMaterial;
class QGLSceneNode;

struct MapItemGLResources {
    QGLSceneNode* sceneNode;
    QGLTexture2D* texture;
    QGLMaterial* defaultMaterial;
    MapItemGLResources() : sceneNode(0), texture(0), defaultMaterial(0) {}
};


class Q_LOCATION_EXPORT_TEMP MapItem
{
public:
    MapItem();
    ~MapItem();

    void setCoordinate(const QGeoCoordinate &coordinate);
    QGeoCoordinate coordinate() const;

    void setTextureId(GLuint textureId);
    GLuint textureId();
    QGLTexture2D* texture();
    void update();

    void setAnchor(const QPointF &anchor);
    QPointF anchor() const;

    void setSize(const QSizeF &size);
    QSizeF size() const;

    void setZoom(double zoom);
    double zoom() const;

    void setSceneNode(QGLSceneNode *sceneNode);
    QGLSceneNode* sceneNode() const;

    void setBounds(const QRect &bounds);
    QRect bounds() const;

    void setVisibleFromViewport(bool visible);
    bool visibleFromViewport() const;

    // put to private
    MapItemGLResources* glResources();
    void setGLResources(MapItemGLResources* resources);

private:
    QGeoCoordinate coordinate_;
    QPointF anchor_;
    QSizeF size_;
    double zoom_;
    QRect bounds_;
    bool visible_;
    GLuint textureId_;
    bool textureDirty_;
    MapItemGLResources* glResources_;
};

class Q_LOCATION_EXPORT_TEMP MapItemTree
{
public:
    MapItemTree();
    ~MapItemTree();

    void insert(MapItem *item);
    void remove(MapItem *item);

    bool isEmpty() const;
    int size() const;

    QList<MapItem*> items() const;
    QList<MapItem*> itemsAt(const QPoint &point) const;
    QList<MapItem*> itemsWithin(const QRect &viewport) const;
    void makeVisible(const QRect& viewport, QList<MapItem*> &added, QList<MapItem*> &removed);
private:

    QRect viewport_;
    AAInterval2DTree<MapItem*> *root_;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // MAPITEM_H
