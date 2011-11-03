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

#include "maptype.h"

QT_BEGIN_NAMESPACE

MapType::MapType() :
    style_(MapType::NoMap), name_(QLatin1String("")),
    description_(QLatin1String("")), mobile_(false), mapId_(0) {}

MapType::MapType(MapType::MapStyle style, const QString &name, const QString &description, bool mobile, int mapId) :
    style_(style), name_(name),
    description_(description), mobile_(mobile), mapId_(mapId) {}

MapType::~MapType() {}

bool MapType::operator == (const MapType &other) const
{
    return ((style_ == other.style_)
            && (name_ == other.name_)
            && (description_ == other.description_)
            && (mobile_ == other.mobile_)
            && (mapId_ == other.mapId_));
}

bool MapType::operator != (const MapType &other) const
{
    return !(operator ==(other));
}

MapType::MapStyle MapType::style() const
{
    return style_;
}

QString MapType::name() const
{
    return name_;
}

QString MapType::description() const
{
    return description_;
}

bool MapType::mobile() const
{
    return mobile_;
}

int MapType::mapId() const
{
    return mapId_;
}

QT_END_NAMESPACE
