/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomaptype_p.h"
#include <qnumeric.h>
#include <QtQml/qqml.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmltype MapType
    \instantiates QDeclarativeGeoMapType
    \inherits QObject
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0

    \brief The MapType type holds information about a map type.

    This includes the map type's \l name and \l description, the \l style and
    a flag to indicate if the map type is optimized for mobile devices (\l mobile).
*/

QDeclarativeGeoMapType::QDeclarativeGeoMapType(const QGeoMapType mapType, QObject *parent)
    : QObject(parent),
      mapType_(mapType) {}

QDeclarativeGeoMapType::~QDeclarativeGeoMapType() {}

/*!
    \qmlproperty enumeration MapType::style

    This read-only property gives access to the style of the map type.

    \list
    \li MapType.NoMap - No map.
    \li MapType.StreetMap - A street map.
    \li MapType.SatelliteMapDay - A map with day-time satellite imagery.
    \li MapType.SatelliteMapNight - A map with night-time satellite imagery.
    \li MapType.TerrainMap - A terrain map.
    \li MapType.HybridMap - A map with satellite imagery and street information.
    \li MapType.GrayStreetMap - A gray-shaded street map.
    \li MapType::PedestrianMap - A street map suitable for pedestriants.
    \li MapType::CarNavigationMap - A street map suitable for car navigation.
    \li MapType.CustomMap - A custom map type.
    \endlist
*/
QDeclarativeGeoMapType::MapStyle QDeclarativeGeoMapType::style() const
{
    return QDeclarativeGeoMapType::MapStyle(mapType_.style());
}

/*!
    \qmlproperty string MapType::name

    This read-only property holds the name of the map type as a single formatted string.
*/
QString QDeclarativeGeoMapType::name() const
{
    return mapType_.name();
}

/*!
    \qmlproperty string MapType::description

    This read-only property holds the description of the map type as a single formatted string.
*/
QString QDeclarativeGeoMapType::description() const
{
    return mapType_.description();
}

/*!
    \qmlproperty bool MapType::mobile

    \brief Whether the map type is optimized for the use on a mobile device.

    Map types for mobile devices usually have higher constrast to counteract the
    effects of sunlight and a reduced color for improved readability.
*/
bool QDeclarativeGeoMapType::mobile() const
{
    return mapType_.mobile();
}

#include "moc_qdeclarativegeomaptype_p.cpp"

QT_END_NAMESPACE
