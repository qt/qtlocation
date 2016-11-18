/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Copyright (C) 2017 Mapbox, Inc.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qsgmapboxglnode.h>
#include <qgeomapmapboxgl.h>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>

QSGMapboxGLNode::QSGMapboxGLNode(const QMapboxGLSettings &settings, const QSize &size, qreal pixelRatio, QGeoMapMapboxGL *geoMap)
        : QSGSimpleTextureNode()
{
    setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);

    m_map.reset(new QMapboxGL(nullptr, settings, size, pixelRatio));

    QObject::connect(m_map.data(), &QMapboxGL::needsRendering, geoMap, &QGeoMap::sgNodeChanged);
    QObject::connect(m_map.data(), &QMapboxGL::copyrightsChanged, geoMap,
            static_cast<void (QGeoMap::*)(const QString &)>(&QGeoMapMapboxGL::copyrightsChanged));
}

void QSGMapboxGLNode::resize(const QSize &size, qreal pixelRatio)
{
    const QSize fbSize = size * pixelRatio;
    m_map->resize(size, fbSize);

    m_fbo.reset(new QOpenGLFramebufferObject(fbSize, QOpenGLFramebufferObject::CombinedDepthStencil));

    QSGPlainTexture *fboTexture = static_cast<QSGPlainTexture *>(texture());
    if (!fboTexture)
        fboTexture = new QSGPlainTexture;

    fboTexture->setTextureId(m_fbo->texture());
    fboTexture->setTextureSize(fbSize);

    if (!texture()) {
        setTexture(fboTexture);
        setOwnsTexture(true);
    }

    setRect(QRectF(QPointF(), size));
    markDirty(QSGNode::DirtyGeometry);
}

void QSGMapboxGLNode::render()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glViewport(0, 0, m_fbo->width(), m_fbo->height());

    m_fbo->bind();
    m_map->render(m_fbo.data());
    m_fbo->release();

    markDirty(QSGNode::DirtyMaterial);
}

QMapboxGL* QSGMapboxGLNode::map() const
{
    return m_map.data();
}
