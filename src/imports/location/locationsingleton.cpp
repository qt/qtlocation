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

#include "locationsingleton.h"

/*!
    \qmltype QtLocation
    \instantiates LocationSingleton
    \inqmlmodule QtLocation 5.0
    \ingroup qml-QtLocation5-common

    \brief The QtLocation global object provides useful functions for working with Qt Location
           types in QML.

    \qml
    import QtLocation 5.0

    Item {
        property variant coordinate: QtLocation.coordinate(-27.5, 153.1)
    }
    \endqml
*/

LocationSingleton::LocationSingleton(QObject *parent)
:   QObject(parent)
{
}

/*!
    \qmlmethod coordinate QtLocation5::QtLocation::coordinate()

    Constructs an invalid coordinate.

    \sa {QtLocation5::coordinate}{coordinate}
*/
QGeoCoordinate LocationSingleton::coordinate() const
{
    return QGeoCoordinate();
}

/*!
    \qmlmethod coordinate QtLocation5::QtLocation::coordinate(real latitude, real longitue, real altitude) const

    Constructs a coordinate with the specified \a latitude, \a longitude and optional \a altitude.
    Both \a latitude and \a longitude must be valid, otherwise an invalid coordinate is returned.

    \sa {QtLocation5::coordinate}{coordinate}
*/
QGeoCoordinate LocationSingleton::coordinate(double latitude, double longitude, double altitude) const
{
    return QGeoCoordinate(latitude, longitude, altitude);
}

