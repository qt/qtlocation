/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2017 Mapbox, Inc.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QSGMAPBOXGLNODE_H
#define QSGMAPBOXGLNODE_H

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGRenderNode>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/private/qsgtexture_p.h>
#include <QtGui/QOpenGLFramebufferObject>

#include <QMapboxGL>

class QGeoMapMapboxGL;

class QSGMapboxGLTextureNode : public QSGSimpleTextureNode
{
public:
    QSGMapboxGLTextureNode(const QMapboxGLSettings &, const QSize &, qreal pixelRatio, QGeoMapMapboxGL *geoMap);

    QMapboxGL* map() const;

    void resize(const QSize &size, qreal pixelRatio);
    void render(QQuickWindow *);

private:
    QScopedPointer<QMapboxGL> m_map;
    QScopedPointer<QOpenGLFramebufferObject> m_fbo;
};

class QSGMapboxGLRenderNode : public QSGRenderNode
{
public:
    QSGMapboxGLRenderNode(const QMapboxGLSettings &, const QSize &, qreal pixelRatio, QGeoMapMapboxGL *geoMap);

    QMapboxGL* map() const;

    // QSGRenderNode
    void render(const RenderState *state) override;
    StateFlags changedStates() const override;

private:
    QScopedPointer<QMapboxGL> m_map;
};

#endif // QSGMAPBOXGLNODE_H
