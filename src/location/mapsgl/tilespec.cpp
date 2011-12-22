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
#include "tilespec.h"
#include "tilespec_p.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

TileSpec::TileSpec()
    : d_ptr(new TileSpecPrivate()) {}

TileSpec::TileSpec(const QString &plugin, int mapId, int zoom, int x, int y)
        : d_ptr(new TileSpecPrivate(plugin, mapId, zoom, x, y)) {}

TileSpec::TileSpec(const TileSpec &other)
    : d_ptr(new TileSpecPrivate(*(other.d_ptr))) {}

TileSpec::~TileSpec() {
    delete d_ptr;
}

TileSpec& TileSpec::operator = (const TileSpec &other)
{
    d_ptr->operator=(*(other.d_ptr));
    return *this;
}

QString TileSpec::plugin() const
{
    Q_D(const TileSpec);
    return d->plugin_;
}

void TileSpec::setZoom(int zoom)
{
    Q_D(TileSpec);
    d->zoom_ = zoom;
}

int TileSpec::zoom() const
{
    Q_D(const TileSpec);
    return d->zoom_;
}

void TileSpec::setX(int x)
{
    Q_D(TileSpec);
    d->x_ = x;
}

int TileSpec::x() const
{
    Q_D(const TileSpec);
    return d->x_;
}

void TileSpec::setY(int y)
{
    Q_D(TileSpec);
    d->y_ = y;
}

int TileSpec::y() const
{
    Q_D(const TileSpec);
    return d->y_;
}

void TileSpec::setMapId(int mapId)
{
    Q_D(TileSpec);
    d->mapId_ = mapId;
}

int TileSpec::mapId() const
{
    Q_D(const TileSpec);
    return d->mapId_;
}

bool TileSpec::operator == (const TileSpec &rhs) const
{
    return (d_ptr->operator == (*(rhs.d_ptr)));
}

bool TileSpec::operator < (const TileSpec &rhs) const
{
    return (d_ptr->operator < (*(rhs.d_ptr)));
}

unsigned int qHash(const TileSpec &spec)
{
    unsigned int result = (qHash(spec.plugin()) * 13) % 31;
    result += ((spec.mapId() * 17) % 31) << 5;
    result += ((spec.zoom() * 19) % 31) << 10;
    result += ((spec.x() * 23) % 31) << 15;
    result += ((spec.y() * 29) % 31) << 20;
    return result;
}

QDebug operator<< (QDebug dbg, const TileSpec &spec)
{
    dbg << spec.plugin() << spec.mapId() << spec.zoom() << spec.x() << spec.y();
    return dbg;
}

TileSpecPrivate::TileSpecPrivate()
    : mapId_(0),
    zoom_(-1),
    x_(-1),
    y_(-1) {}

TileSpecPrivate::TileSpecPrivate(const TileSpecPrivate &other)
    : plugin_(other.plugin_),
      mapId_(other.mapId_),
      zoom_(other.zoom_),
      x_(other.x_),
      y_(other.y_) {}

TileSpecPrivate::TileSpecPrivate(const QString &plugin, int mapId, int zoom, int x, int y)
    : plugin_(plugin),
      mapId_(mapId),
      zoom_(zoom),
      x_(x),
      y_(y) {}

TileSpecPrivate::~TileSpecPrivate() {}

TileSpecPrivate& TileSpecPrivate::operator = (const TileSpecPrivate &other)
{
    plugin_ = other.plugin_;
    mapId_ = other.mapId_;
    zoom_ = other.zoom_;
    x_ = other.x_;
    y_ = other.y_;
    return *this;
}

bool TileSpecPrivate::operator == (const TileSpecPrivate &rhs) const
{
    if (plugin_ != rhs.plugin_)
        return false;

    if (mapId_ != rhs.mapId_)
        return false;

    if (zoom_ != rhs.zoom_)
        return false;

    if (x_ != rhs.x_)
        return false;

    if (y_ != rhs.y_)
        return false;

    return true;
}

bool TileSpecPrivate::operator < (const TileSpecPrivate &rhs) const
{
    if (plugin_ < rhs.plugin_)
        return true;
    if (plugin_ > rhs.plugin_)
        return false;

    if (zoom_ < rhs.zoom_)
        return true;
    if (zoom_ > rhs.zoom_)
        return false;

    if (x_ < rhs.x_)
        return true;
    if (x_ > rhs.x_)
        return false;

    return (y_ < rhs.y_);
}

QT_END_NAMESPACE
