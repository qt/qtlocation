/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "locationlabssingleton_p.h"

/*!
    \qmltype QtLocationLabs
    \instantiates LocationLabsSingleton
    \inqmlmodule Qt.labs.location
    \since 5.13

    \brief The QtLocationLabs global object provides experimental, tech-preview functions for working with
           location-based types in QML.

    \qml
    import Qt.labs.location 1.0

    Map {
        id: map
        MouseArea {
            anchors.fill: parent
            onClicked: {
                var mapObjects = QtLocationLabs.mapObjectsAt(map.toCoordinate(Qt.point(mouseX,mouseY)), map)
            }
        }
    }
    \endqml
*/

LocationLabsSingleton::LocationLabsSingleton(QObject *parent)
:   QObject(parent)
{
}

/*!
    \qmlmethod coordinate ::QtLocationLabs::mapObjectsAt(coordinate, map)

    Returns the map objects in \a map covering \a coordinate.
*/
QList<QObject *> LocationLabsSingleton::mapObjectsAt(const QGeoCoordinate &coordinate, QDeclarativeGeoMap *map) const
{
    if (!map)
        return QList<QObject *>();
    return map->map()->mapObjectsAt(coordinate);
}
