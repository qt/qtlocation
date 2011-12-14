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

#include "mapnode_p.h"
#include "map.h"
#include <Qt3D/qglpainter.h>
#include <Qt3D/qglframebufferobjectsurface.h>
#include <QtGui/QOpenGLFramebufferObject>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

MapNode::MapNode(Map* map): QSGGeometryNode(),
        m_fbo(0),
        m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4),
        m_texture(0),
        m_map(map),
        m_initialized(false),
        m_dirtyFBO(false)
{
    setMaterial(&m_material);
    setOpaqueMaterial(&m_opagueMaterial);
    setGeometry(&m_geometry);
    // setFlag(UsePreprocess);
}

MapNode::~MapNode()
{
    delete m_fbo;
}

void MapNode::saveRenderState()
{
    //TODO: implement me
}

void MapNode::restoreRenderState()
{
    //TODO: implement me
}

void
MapNode::preprocess()
{
    restoreRenderState();
    //TODO: implement me , texture update
    saveRenderState();
}

void MapNode::update()
{
    restoreRenderState();

    if (!m_initialized)
        init();

    if (m_dirtyFBO)
        updateFBO();

    saveRenderState();
}

void MapNode::updateFBO()
{
    QGLPainter painter;

    if (!painter.begin()) {
        qWarning() << "GL graphics system is not active, can not update FBO for map";
        return;
    }

    painter.pushSurface(&m_fboSurface);

    glClearColor(0.9, 0.9, 0.9, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    painter.disableEffect();
#ifdef GL_RESCALE_NORMAL
    // Scale normals by a scale factor derived from modelview matrix.
    // Note: normals need to be unit length.
    glEnable(GL_RESCALE_NORMAL);
#endif

#if !defined(QT_OPENGL_ES_2)
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
#endif

    // Set the default effect for the scene.
    painter.setStandardEffect(QGL::LitMaterial);
    painter.setFaceColor(QGL::AllFaces, Qt::white);

#if defined(QT_OPENGL_ES)
    glClearDepthf(1);
#else
    glClearDepth(1);
#endif

#ifdef QSGSHADEREFFECTSOURCE_AVAILABLE
    // Update any map objects that may have dirty textures
    for (int i = 0; i < mapItems_.count(); ++i) {
        mapItems_.at(i)->updateItem();
    }
#endif

#if defined(GL_LINE_SMOOTH) && defined(GL_LINE_SMOOTH_HINT)
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
#endif

    // No stereo rendering, set the eye as neutral
    painter.setEye(QGL::NoEye);
    // TODO this needs to be figured out (or confirmed as invalid thing).
    // Currently applied transforms for this Map3D element - how to get/apply current transforms?
    // QTransform transform = painter->combinedTransform();
    // Then we get the rectangle that is gotten by applying the QTransform on the rect
    // --> this is the viewport for Map3D
    // QRect viewport = transform.mapRect(boundingRect()).toRect();

    // boundingRect is in local coordinates. We need to map it to the scene coordinates
    // in order to render to correct area.

    if (m_map->glCamera()) {
        painter.setCamera(m_map->glCamera());
    } else {
        QGLCamera defCamera;
        painter.setCamera(&defCamera);
    }
    painter.projectionMatrix().scale(1, -1, 1);
    m_map->paintGL(&painter);

    restoreDefaults(&painter);

    // Draw the children items
    painter.popSurface();
    // QSG does not expect anyone to alter gl context state; restore defaults.
    // Default heaps of things, because we cannot be sure what the Qt3D internally
    // sets.

#if defined(QT_OPENGL_ES)
    glClearDepthf(0);
#else
    glClearDepth(0);
#endif

    painter.end();

    QSGGeometry::updateTexturedRectGeometry(&m_geometry,
                                            QRectF(0, 0, m_fbo->size().width(), m_fbo->size().height()),
                                            QRectF(0, 0, 1, 1));
    delete m_texture;
    m_texture = new QSGPlainTexture();
    m_texture->setOwnsTexture(false);
    m_texture->setTextureSize(m_fbo->size());
    m_texture->setTextureId(m_fbo->texture());
    m_opagueMaterial.setTexture(m_texture);
    m_material.setTexture(m_texture);
}

void MapNode::setSize(const QSize &size)
{
    if (size == m_size)
        return;
    m_size = size;
    m_dirtyFBO = true;
    m_initialized = false;
    markDirty(DirtyGeometry);
}

void MapNode::init()
{
    delete m_fbo;
    m_fbo = new QOpenGLFramebufferObject(m_size.width(), m_size.height());
    m_fboSurface.setFramebufferObject(m_fbo);
    m_initialized = true;
}

void MapNode::restoreDefaults(QGLPainter *painter)
{
    // Disable the effect to return control to the GL paint engine.
    painter->disableEffect();

    // Try to restore the default options
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    // Set the default depth buffer options.
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
    glDepthRangef(0.0f, 1.0f);
#else
    glDepthRange(0.0f, 1.0f);
#endif
    // Set the default blend options.
    glDisable(GL_BLEND);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter->glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter->glBlendEquation(GL_FUNC_ADD);
    else if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

