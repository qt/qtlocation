/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#include "qgeomap_p.h"
#include "qgeomapdata_p.h"
#include "qgeocameracapabilities_p.h"

QT_BEGIN_NAMESPACE

QGeoMap::QGeoMap(QGeoMapData *mapData, QObject *parent)
    : QObject(parent),
      m_mapData(mapData)
{
    connect(m_mapData, SIGNAL(cameraDataChanged(QGeoCameraData)), this, SIGNAL(cameraDataChanged(QGeoCameraData)));
    connect(m_mapData, SIGNAL(updateRequired()), this, SIGNAL(updateRequired()));
    connect(m_mapData, SIGNAL(activeMapTypeChanged()), this, SIGNAL(activeMapTypeChanged()));
    connect(m_mapData, SIGNAL(copyrightsChanged(QImage)), this, SIGNAL(copyrightsChanged(QImage)));
    connect(m_mapData, SIGNAL(copyrightsChanged(QString)), this, SIGNAL(copyrightsChanged(QString)));
}

QGeoMap::~QGeoMap()
{
    delete m_mapData;
}

QGeoMapController *QGeoMap::mapController()
{
    return m_mapData->mapController();
}

QSGNode *QGeoMap::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    return m_mapData->updateSceneGraph(oldNode, window);
}

void QGeoMap::resize(int width, int height)
{
    m_mapData->resize(width, height);
}

int QGeoMap::width() const
{
    return m_mapData->width();
}

int QGeoMap::height() const
{
    return m_mapData->height();
}

QGeoCameraCapabilities QGeoMap::cameraCapabilities() const
{
    return m_mapData->cameraCapabilities();
}

void QGeoMap::setCameraData(const QGeoCameraData &cameraData)
{
    m_mapData->setCameraData(cameraData);
}

void QGeoMap::cameraStopped()
{
    m_mapData->prefetchData();
}

QGeoCameraData QGeoMap::cameraData() const
{
    return m_mapData->cameraData();
}

QGeoCoordinate QGeoMap::itemPositionToCoordinate(const QDoubleVector2D &pos, bool clipToViewport) const
{
    return m_mapData->itemPositionToCoordinate(pos, clipToViewport);
}

QDoubleVector2D QGeoMap::coordinateToItemPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    return m_mapData->coordinateToItemPosition(coordinate, clipToViewport);
}

void QGeoMap::update()
{
    emit m_mapData->update();
}

void QGeoMap::setActiveMapType(const QGeoMapType type)
{
    m_mapData->setActiveMapType(type);
}

const QGeoMapType QGeoMap::activeMapType() const
{
    return m_mapData->activeMapType();
}

QString QGeoMap::pluginString()
{
    return m_mapData->pluginString();
}

#include "moc_qgeomap_p.cpp"

QT_END_NAMESPACE
