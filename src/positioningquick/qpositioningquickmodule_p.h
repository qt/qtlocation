/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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
#ifndef QT_POSITIONINGQUICKMODULE_P_H
#define QT_POSITIONINGQUICKMODULE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQml/qqml.h>
#include "qpositioningquickglobal_p.h"

#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoAddress>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoPath>
#include <QtPositioning/QGeoPolygon>
#include <QtPositioning/QGeoLocation>
#include <QtPositioning/QGeoShape>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/private/qgeocoordinateobject_p.h>

QT_BEGIN_NAMESPACE

struct QGeoCoordinateForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoCoordinate)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoAddressForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoAddress)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoRectangleForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoRectangle)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoCircleForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoCircle)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoPathForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoPath)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoPolygonForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoPolygon)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoLocationForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoLocation)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoShapeForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoShape)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoCoordinateObjectForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoCoordinateObject)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

struct QGeoPositionInfoForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoPositionInfo)
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(5, 0)
};

QT_END_NAMESPACE

#endif // QT_POSITIONINGQUICKMODULE_P_H
