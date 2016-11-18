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

#include "qgeomapmapboxgl.h"
#include "qsgmapboxglnode.h"

#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtLocation/private/qgeomap_p_p.h>
#include <QtLocation/private/qgeoprojection_p.h>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGImageNode>
#include <QtQuick/private/qsgtexture_p.h>

#include <QMapboxGL>

#include <cmath>

// FIXME: Expose from Mapbox GL constants
#define MBGL_TILE_SIZE 512.0

namespace {

static const double invLog2 = 1.0 / std::log(2.0);

static double zoomLevelFrom256(double zoomLevelFor256, double tileSize)
{
    return std::log(std::pow(2.0, zoomLevelFor256) * 256.0 / tileSize) * invLog2;
}

} // namespace

class QGeoMapMapboxGLPrivate : public QGeoMapPrivate
{
    Q_DECLARE_PUBLIC(QGeoMapMapboxGL)

public:
    QGeoMapMapboxGLPrivate(QGeoMappingManagerEngineMapboxGL *engine);

    ~QGeoMapMapboxGLPrivate();

    QSGNode *updateSceneGraph(QSGNode *oldNode, QQuickWindow *window);

    /* Data members */
    enum SyncState : int {
        NoSync = 0,
        ViewportSync   = 1 << 0,
        CameraDataSync = 1 << 1,
        MapTypeSync    = 1 << 2
    };
    Q_DECLARE_FLAGS(SyncStates, SyncState);

    QMapboxGLSettings m_settings;

    QTimer m_refresh;
    bool m_shouldRefresh = true;

    SyncStates m_syncState = NoSync;

protected:
    void changeViewportSize(const QSize &size) Q_DECL_OVERRIDE;
    void changeCameraData(const QGeoCameraData &oldCameraData) Q_DECL_OVERRIDE;
    void changeActiveMapType(const QGeoMapType mapType) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QGeoMapMapboxGLPrivate)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoMapMapboxGLPrivate::SyncStates)

QGeoMapMapboxGLPrivate::QGeoMapMapboxGLPrivate(QGeoMappingManagerEngineMapboxGL *engine)
    : QGeoMapPrivate(engine, new QGeoProjectionWebMercator)
{
}

QGeoMapMapboxGLPrivate::~QGeoMapMapboxGLPrivate()
{
}

QSGNode *QGeoMapMapboxGLPrivate::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    Q_Q(QGeoMapMapboxGL);

    static bool warned;
    if (!warned) {
        if (window->openglContext()->thread() != QCoreApplication::instance()->thread()) {
            qWarning() << "Threaded rendering is not supported by Mapbox GL plugin.";
            QMetaObject::invokeMethod(&m_refresh, "start", Qt::QueuedConnection);
        }

        warned = true;
    }

    QSGMapboxGLNode *mbglNode = static_cast<QSGMapboxGLNode *>(oldNode);
    if (!mbglNode) {
        mbglNode = new QSGMapboxGLNode(m_settings, m_viewportSize, window->devicePixelRatio(), q);
        m_syncState = MapTypeSync | CameraDataSync | ViewportSync;
    }

    QMapboxGL *map = mbglNode->map();

    if (m_syncState & MapTypeSync) {
        map->setStyleUrl(m_activeMapType.name());
    }

    if (m_syncState & CameraDataSync) {
        map->setZoom(zoomLevelFrom256(m_cameraData.zoomLevel() , MBGL_TILE_SIZE));
        map->setBearing(m_cameraData.bearing());
        map->setPitch(m_cameraData.tilt());

        QGeoCoordinate coordinate = m_cameraData.center();
        map->setCoordinate(QMapbox::Coordinate(coordinate.latitude(), coordinate.longitude()));
    }

    if (m_syncState & ViewportSync) {
        mbglNode->resize(m_viewportSize, window->devicePixelRatio());
    }

    mbglNode->render();

    m_syncState = NoSync;

    return mbglNode;
}

void QGeoMapMapboxGLPrivate::changeViewportSize(const QSize &)
{
    Q_Q(QGeoMapMapboxGL);

    m_syncState = m_syncState | ViewportSync;
    emit q->sgNodeChanged();
}

void QGeoMapMapboxGLPrivate::changeCameraData(const QGeoCameraData &)
{
    Q_Q(QGeoMapMapboxGL);

    m_syncState = m_syncState | CameraDataSync;
    emit q->sgNodeChanged();
}

void QGeoMapMapboxGLPrivate::changeActiveMapType(const QGeoMapType)
{
    Q_Q(QGeoMapMapboxGL);

    m_syncState = m_syncState | MapTypeSync;
    emit q->sgNodeChanged();
}

/*
 * QGeoMapMapboxGL implementation
 */

QGeoMapMapboxGL::QGeoMapMapboxGL(QGeoMappingManagerEngineMapboxGL *engine, QObject *parent)
    :   QGeoMap(*new QGeoMapMapboxGLPrivate(engine), parent), m_engine(engine)
{
    Q_D(QGeoMapMapboxGL);

    connect(&d->m_refresh, &QTimer::timeout, this, &QGeoMap::sgNodeChanged);
    d->m_refresh.setInterval(250);
}

QGeoMapMapboxGL::~QGeoMapMapboxGL()
{
}

void QGeoMapMapboxGL::setMapboxGLSettings(const QMapboxGLSettings& settings)
{
    Q_D(QGeoMapMapboxGL);

    d->m_settings = settings;
}

QSGNode *QGeoMapMapboxGL::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    Q_D(QGeoMapMapboxGL);
    return d->updateSceneGraph(oldNode, window);
}
