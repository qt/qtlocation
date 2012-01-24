/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomaptype.h"
#include "qgeomaptype_p.h"

QT_BEGIN_NAMESPACE

QGeoMapType::QGeoMapType()
    : d_ptr(new QGeoMapTypePrivate()) {}

QGeoMapType::QGeoMapType(const QGeoMapType &other)
    : d_ptr(other.d_ptr) {}

QGeoMapType::QGeoMapType(QGeoMapType::MapStyle style, const QString &name, const QString &description, bool mobile, int mapId)
    : d_ptr(new QGeoMapTypePrivate(style, name, description, mobile, mapId)) {}

QGeoMapType::~QGeoMapType() {}

QGeoMapType& QGeoMapType::operator = (const QGeoMapType &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

bool QGeoMapType::operator == (const QGeoMapType &other) const
{
    return (*d_ptr.constData() == *other.d_ptr.constData());
}

bool QGeoMapType::operator != (const QGeoMapType &other) const
{
    return !(operator ==(other));
}

QGeoMapType::MapStyle QGeoMapType::style() const
{
    return d_ptr->style_;
}

QString QGeoMapType::name() const
{
    return d_ptr->name_;
}

QString QGeoMapType::description() const
{
    return d_ptr->description_;
}

bool QGeoMapType::mobile() const
{
    return d_ptr->mobile_;
}

int QGeoMapType::mapId() const
{
    return d_ptr->mapId_;
}

QGeoMapTypePrivate::QGeoMapTypePrivate()
    : style_(QGeoMapType::NoMap),
      name_(QLatin1String("")),
      description_(QLatin1String("")),
      mobile_(false),
      mapId_(0) {}

QGeoMapTypePrivate::QGeoMapTypePrivate(const QGeoMapTypePrivate &other)
    : QSharedData(other),
      style_(other.style_),
      name_(other.name_),
      description_(other.description_),
      mobile_(other.mobile_),
      mapId_(other.mapId_) {}

QGeoMapTypePrivate::QGeoMapTypePrivate(QGeoMapType::MapStyle style, const QString &name, const QString &description, bool mobile, int mapId)
    : style_(style),
      name_(name),
      description_(description),
      mobile_(mobile),
      mapId_(mapId) {}

QGeoMapTypePrivate::~QGeoMapTypePrivate() {}

bool QGeoMapTypePrivate::operator == (const QGeoMapTypePrivate &other) const
{
    return ((style_ == other.style_)
            && (name_ == other.name_)
            && (description_ == other.description_)
            && (mobile_ == other.mobile_)
            && (mapId_ == other.mapId_));
}

QT_END_NAMESPACE
