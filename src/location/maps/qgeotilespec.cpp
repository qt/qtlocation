// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
