/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomapitemsoverlay.h"
#include "qgeomappingmanagerengineitemsoverlay.h"
#include <QtLocation/private/qgeomap_p_p.h>
#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgrectanglenode.h>
#include <QtQuick/qquickwindow.h>

QT_BEGIN_NAMESPACE

class QGeoMapItemsOverlayPrivate : public QGeoMapPrivate
{
    Q_DECLARE_PUBLIC(QGeoMapItemsOverlay)
public:
    QGeoMapItemsOverlayPrivate(QGeoMappingManagerEngineItemsOverlay *engine, QGeoMapItemsOverlay *map);
    virtual ~QGeoMapItemsOverlayPrivate();

    void updateObjectsGeometry();

    void setVisibleArea(const QRectF &visibleArea) override;
    QRectF visibleArea() const override;

protected:
    void changeViewportSize(const QSize &size) override;
    void changeCameraData(const QGeoCameraData &oldCameraData) override;
    void changeActiveMapType(const QGeoMapType &mapType) override;

    QRectF m_visibleArea;
};

QGeoMapItemsOverlay::QGeoMapItemsOverlay(QGeoMappingManagerEngineItemsOverlay *engine, QObject *parent)
    : QGeoMap(*(new QGeoMapItemsOverlayPrivate(engine, this)), parent)
{

}

QGeoMapItemsOverlay::~QGeoMapItemsOverlay()
{
}

QGeoMap::Capabilities QGeoMapItemsOverlay::capabilities() const
{
    return Capabilities(SupportsVisibleRegion
                        | SupportsSetBearing
                        | SupportsAnchoringCoordinate);
}

void QGeoMapItemsOverlayPrivate::setVisibleArea(const QRectF &visibleArea)
{
    Q_Q(QGeoMapItemsOverlay);
    const QRectF va = clampVisibleArea(visibleArea);
    if (va == m_visibleArea)
        return;

    m_visibleArea = va;
    m_geoProjection->setVisibleArea(va);

    q->sgNodeChanged();
}

QRectF QGeoMapItemsOverlayPrivate::visibleArea() const
{
    return m_visibleArea;
}

QSGNode *QGeoMapItemsOverlay::updateSceneGraph(QSGNode *node, QQuickWindow *)
{
    return node;
}

QGeoMapItemsOverlayPrivate::QGeoMapItemsOverlayPrivate(QGeoMappingManagerEngineItemsOverlay *engine, QGeoMapItemsOverlay *map)
    : QGeoMapPrivate(engine, new QGeoProjectionWebMercator)
{
    Q_UNUSED(map);
}

QGeoMapItemsOverlayPrivate::~QGeoMapItemsOverlayPrivate()
{
}

void QGeoMapItemsOverlayPrivate::updateObjectsGeometry()
{
}

void QGeoMapItemsOverlayPrivate::changeViewportSize(const QSize &/*size*/)
{
    updateObjectsGeometry();
}

void QGeoMapItemsOverlayPrivate::changeCameraData(const QGeoCameraData &/*oldCameraData*/)
{
    updateObjectsGeometry();
}

void QGeoMapItemsOverlayPrivate::changeActiveMapType(const QGeoMapType &/*mapType*/)
{
    updateObjectsGeometry();
}

QT_END_NAMESPACE




