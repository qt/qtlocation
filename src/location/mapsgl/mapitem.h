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

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Location)

class QGLTexture2D;
class QGLMaterial;
class QGLSceneNode;

class Q_LOCATION_EXPORT MapItem
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

private:
    QGeoCoordinate coordinate_;
    QPointF anchor_;
    QSizeF size_;
    double zoom_;
    QGLSceneNode* sceneNode_;
    GLuint textureId_;
    bool textureDirty_;
    QGLTexture2D* texture_;
    QGLMaterial* defaultMaterial_;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // MAPITEM_H
