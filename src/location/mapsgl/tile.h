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
#ifndef TILE_H
#define TILE_H

#include "qglobal.h"

#include "tilespec.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Location)

class QGLTexture2D;
class QGLSceneNode;

class Q_LOCATION_EXPORT Tile
{
public:
    Tile();
    Tile(const TileSpec &spec);

    bool operator == (const Tile &rhs) const;

    void setTileSpec(const TileSpec &spec);
    TileSpec tileSpec() const;

    void setTexture(QGLTexture2D *texture);
    QGLTexture2D* texture() const;

    void setSceneNode(QGLSceneNode *sceneNode);
    QGLSceneNode *sceneNode() const;

    void bind();
    void setBound(bool bound);
    bool isBound() const;

private:
    TileSpec spec_;
    QGLTexture2D *texture_;
    QGLSceneNode *sceneNode_;
    bool bound_;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // TILE_H
