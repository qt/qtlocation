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

#include "qgeomapdata_p.h"
#include "qgeomapdata_p_p.h"
#include "qgeocameracapabilities_p.h"
#include "qgeomapcontroller_p.h"
#include "qgeomappingmanagerengine_p.h"

QT_BEGIN_NAMESPACE

QGeoMapData::QGeoMapData(QGeoMappingManagerEngine *engine, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoMapDataPrivate(engine, this)) {}

QGeoMapData::~QGeoMapData()
{
    delete d_ptr;
}

QGeoMapController *QGeoMapData::mapController()
{
    Q_D(QGeoMapData);
    return d->mapController();
}

void QGeoMapData::resize(int width, int height)
{
    Q_D(QGeoMapData);
    d->resize(width, height);

    // always emit this signal to trigger items to redraw
    emit cameraDataChanged(d->cameraData());
}

int QGeoMapData::width() const
{
    Q_D(const QGeoMapData);
    return d->width();
}

int QGeoMapData::height() const
{
    Q_D(const QGeoMapData);
    return d->height();
}

void QGeoMapData::setCameraData(const QGeoCameraData &cameraData)
{
    Q_D(QGeoMapData);
    if (cameraData == d->cameraData())
        return;

    d->setCameraData(cameraData);
    update();

    emit cameraDataChanged(d->cameraData());
}

QGeoCameraData QGeoMapData::cameraData() const
{
    Q_D(const QGeoMapData);
    return d->cameraData();
}

void QGeoMapData::update()
{
    emit updateRequired();
}

void QGeoMapData::setActiveMapType(const QGeoMapType type)
{
    Q_D(QGeoMapData);
    d->setActiveMapType(type);
}

const QGeoMapType QGeoMapData::activeMapType() const
{
    Q_D(const QGeoMapData);
    return d->activeMapType();
}

QString QGeoMapData::pluginString()
{
    Q_D(QGeoMapData);
    return d->pluginString();
}

QGeoCameraCapabilities QGeoMapData::cameraCapabilities()
{
    Q_D(QGeoMapData);
    if (d->engine())
        return d->engine()->cameraCapabilities();
    else
        return QGeoCameraCapabilities();
}

QGeoMappingManagerEngine *QGeoMapData::engine()
{
    Q_D(QGeoMapData);
    return d->engine();
}

QGeoMapDataPrivate::QGeoMapDataPrivate(QGeoMappingManagerEngine *engine, QGeoMapData *parent)
    : m_width(0),
      m_height(0),
      m_aspectRatio(0.0),
      m_map(parent),
      m_engine(engine),
      m_controller(0),
      m_activeMapType(QGeoMapType())
{
    m_pluginString = m_engine->managerName() + QLatin1Char('_') + QString::number(m_engine->managerVersion());
}

QGeoMapDataPrivate::~QGeoMapDataPrivate()
{
    // controller_ is a child of map_, don't need to delete it here

    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

QGeoMappingManagerEngine *QGeoMapDataPrivate::engine() const
{
    return m_engine;
}

QString QGeoMapDataPrivate::pluginString()
{
    return m_pluginString;
}

QGeoMapController *QGeoMapDataPrivate::mapController()
{
    if (!m_controller)
        m_controller = new QGeoMapController(m_map);
    return m_controller;
}

void QGeoMapDataPrivate::setCameraData(const QGeoCameraData &cameraData)
{
    QGeoCameraData oldCameraData = m_cameraData;
    m_cameraData = cameraData;

    if (m_engine) {
        QGeoCameraCapabilities capabilities = m_engine->cameraCapabilities();
        if (m_cameraData.zoomLevel() < capabilities.minimumZoomLevel())
            m_cameraData.setZoomLevel(capabilities.minimumZoomLevel());

        if (m_cameraData.zoomLevel() > capabilities.maximumZoomLevel())
            m_cameraData.setZoomLevel(capabilities.maximumZoomLevel());

        if (!capabilities.supportsBearing())
            m_cameraData.setBearing(0.0);

        if (capabilities.supportsTilting()) {
            if (m_cameraData.tilt() < capabilities.minimumTilt())
                m_cameraData.setTilt(capabilities.minimumTilt());

            if (m_cameraData.tilt() > capabilities.maximumTilt())
                m_cameraData.setTilt(capabilities.maximumTilt());
        } else {
            m_cameraData.setTilt(0.0);
        }

        if (!capabilities.supportsRolling())
            m_cameraData.setRoll(0.0);
    }

    // Do not call this expensive function if the width is 0, since it will get called
    // anyway when it is resized to a width > 0.
    // this is mainly an optimization to the initialization of the geomap, which would otherwise
    // call changeCameraData four or more times
    if (width() > 0)
        m_map->changeCameraData(oldCameraData);
}

QGeoCameraData QGeoMapDataPrivate::cameraData() const
{
    return m_cameraData;
}

void QGeoMapDataPrivate::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_aspectRatio = 1.0 * m_width / m_height;
    m_map->mapResized(width, height);
    setCameraData(m_cameraData);
}

int QGeoMapDataPrivate::width() const
{
    return m_width;
}

int QGeoMapDataPrivate::height() const
{
    return m_height;
}

double QGeoMapDataPrivate::aspectRatio() const
{
    return m_aspectRatio;
}

void QGeoMapDataPrivate::setActiveMapType(const QGeoMapType &type)
{
    m_activeMapType = type;

    m_map->changeActiveMapType(type);
    setCameraData(m_cameraData);

    m_map->update();
}

const QGeoMapType QGeoMapDataPrivate::activeMapType() const
{
  return m_activeMapType;
}

QT_END_NAMESPACE
