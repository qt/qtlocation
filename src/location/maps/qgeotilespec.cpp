/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#include "qgeotilespec_p.h"
#include "qgeotilespec_p_p.h"

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QT_DEFINE_QSDP_SPECIALIZATION_DTOR(QGeoTileSpecPrivate)

QGeoTileSpec::QGeoTileSpec()
    : d(new QGeoTileSpecPrivate())
{
}

QGeoTileSpec::QGeoTileSpec(const QString &plugin, int mapId, int zoom, int x, int y, int version)
        : d(new QGeoTileSpecPrivate(plugin, mapId, zoom, x, y, version))
{
}

QGeoTileSpec::QGeoTileSpec(const QGeoTileSpec &other) noexcept = default;

QGeoTileSpec::~QGeoTileSpec() = default;

QGeoTileSpec &QGeoTileSpec::operator=(const QGeoTileSpec &other) noexcept
{
    if (this == &other)
        return *this;

    d = other.d;
    return *this;
}

QString QGeoTileSpec::plugin() const
{
    return d->plugin_;
}

void QGeoTileSpec::setZoom(int zoom)
{
    d->zoom_ = zoom;
}

int QGeoTileSpec::zoom() const
{
    return d->zoom_;
}

void QGeoTileSpec::setX(int x)
{
    d->x_ = x;
}

int QGeoTileSpec::x() const
{
    return d->x_;
}

void QGeoTileSpec::setY(int y)
{
    d->y_ = y;
}

int QGeoTileSpec::y() const
{
    return d->y_;
}

void QGeoTileSpec::setMapId(int mapId)
{
    d->mapId_ = mapId;
}

int QGeoTileSpec::mapId() const
{
    return d->mapId_;
}

void QGeoTileSpec::setVersion(int version)
{
    d->version_ = version;
}

int QGeoTileSpec::version() const
{
    return d->version_;
}

bool QGeoTileSpec::isEqual(const QGeoTileSpec &rhs) const noexcept
{
    return (*(d.constData()) == *(rhs.d.constData()));
}

bool QGeoTileSpec::isLess(const QGeoTileSpec &rhs) const noexcept
{
    return (*(d.constData()) < *(rhs.d.constData()));
}

unsigned int qHash(const QGeoTileSpec &spec)
{
    unsigned int result = (qHash(spec.plugin()) * 13) % 31;
    result += ((spec.mapId() * 17) % 31) << 5;
    result += ((spec.zoom() * 19) % 31) << 10;
    result += ((spec.x() * 23) % 31) << 15;
    result += ((spec.y() * 29) % 31) << 20;
    result += (spec.version() % 3) << 25;
    return result;
}

QDebug operator<< (QDebug dbg, const QGeoTileSpec &spec)
{
    dbg << spec.plugin() << spec.mapId() << spec.zoom() << spec.x() << spec.y() << spec.version();
    return dbg;
}

bool QGeoTileSpecPrivate::operator<(const QGeoTileSpecPrivate &rhs) const
{
    if (plugin_ < rhs.plugin_)
        return true;
    if (plugin_ > rhs.plugin_)
        return false;

    if (mapId_ < rhs.mapId_)
        return true;
    if (mapId_ > rhs.mapId_)
        return false;

    if (zoom_ < rhs.zoom_)
        return true;
    if (zoom_ > rhs.zoom_)
        return false;

    if (x_ < rhs.x_)
        return true;
    if (x_ > rhs.x_)
        return false;

    if (y_ < rhs.y_)
        return true;
    if (y_ > rhs.y_)
        return false;

    return (version_ < rhs.version_);
}

QT_END_NAMESPACE
