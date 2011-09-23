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
#include <QDebug>

QT_BEGIN_NAMESPACE

TileSpec::TileSpec()
        : zoom_(-1),
        x_(-1),
        y_(-1) {}

TileSpec::TileSpec(int zoom, int x, int y)
        : zoom_(zoom),
        x_(x),
        y_(y) {}

void TileSpec::setZoom(int zoom)
{
    zoom_ = zoom;
}

int TileSpec::zoom() const
{
    return zoom_;
}

void TileSpec::setX(int x)
{
    x_ = x;
}

int TileSpec::x() const
{
    return x_;
}

void TileSpec::setY(int y)
{
    y_ = y;
}

int TileSpec::y() const
{
    return y_;
}

bool TileSpec::operator == (const TileSpec &rhs) const
{
    if (zoom_ != rhs.zoom_)
        return false;

    if (x_ != rhs.x_)
        return false;

    if (y_ != rhs.y_)
        return false;

    return true;
}

bool TileSpec::operator < (const TileSpec &rhs) const
{
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

unsigned int qHash(const TileSpec &spec)
{
    unsigned int result = (spec.zoom() * 17) % 31;
    result += ((spec.x() * 19) % 31) << 5;
    result += ((spec.y() * 23) % 31) << 10;
    return result;
}

QDebug operator<< (QDebug dbg, const TileSpec &spec)
{
    dbg << spec.zoom() << spec.x() << spec.y();
    return dbg;
}

QT_END_NAMESPACE
