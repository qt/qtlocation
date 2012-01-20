/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MAPNODE_H
#define MAPNODE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQuick/QSGNode>
#include <QtQuick/QSGTextureMaterial>
#include <QtQuick/private/qsgtexture_p.h>
#include <Qt3D/QGLFramebufferObjectSurface>

QT_BEGIN_NAMESPACE

class QGeoMap;
class QOpenGLFramebufferObject;
class QGLPainter;

class MapNode : public QSGGeometryNode
{
public:
    MapNode(QGeoMap *m_map);
    ~MapNode();

    void setSize(const QSize &size);
    QSize size() const {
        return m_size;
    }
    void update();
    void preprocess();

private:
    void init();
    void updateFBO();
    void saveRenderState();
    void restoreRenderState();
    void restoreDefaults(QGLPainter *painter);

private:
    QOpenGLFramebufferObject *m_fbo;
    QGLFramebufferObjectSurface *m_fboSurface;
    QSGOpaqueTextureMaterial m_opagueMaterial;
    QSGTextureMaterial m_material;
    QSGGeometry m_geometry;
    QSGPlainTexture *m_texture;
    QGeoMap *m_map;
    QSize m_size;
    bool m_initialized;
    bool m_dirtyFBO;
};

QT_END_NAMESPACE

#endif
