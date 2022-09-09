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

#ifndef QTLOCATION_QMLTYPES_H
#define QTLOCATION_QMLTYPES_H

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

#include <QGeoManeuver>
#include <QGeoRouteSegment>
#include <QPlaceAttribute>
#include <QPlaceUser>
#include <QPlaceSupplier>
#include <QPlaceRatings>
#include <QPlaceIcon>

#include <QtLocation/private/qgeomaptype_p.h>

#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

struct QGeoManeuverForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoManeuver)
    QML_NAMED_ELEMENT(routeManeuver)
};

namespace QGeoManeuverForeignNamespace
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QGeoManeuver)
    QML_NAMED_ELEMENT(RouteManeuver)
};

struct QGeoRouteSegmentForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoRouteSegment)
    QML_NAMED_ELEMENT(routeSegment)
};

struct QGeoMapTypeForeign
{
    Q_GADGET
    QML_FOREIGN(QGeoMapType)
    QML_NAMED_ELEMENT(mapType)
};

namespace QGeoMapTypeForeignNamespace
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QGeoMapType)
    QML_NAMED_ELEMENT(MapType)
};

struct QPlaceUserForeign
{
    Q_GADGET
    QML_FOREIGN(QPlaceUser)
    QML_NAMED_ELEMENT(user)
};

struct QPlaceRatingsForeign
{
    Q_GADGET
    QML_FOREIGN(QPlaceRatings)
    QML_NAMED_ELEMENT(ratings)
};

struct QPlaceAttributeForeign
{
    Q_GADGET
    QML_FOREIGN(QPlaceAttribute)
    QML_NAMED_ELEMENT(placeAttribute)
};

struct QPlaceIconForeign
{
    Q_GADGET
    QML_FOREIGN(QPlaceIcon)
    QML_NAMED_ELEMENT(icon)
};

struct QPlaceSupplierForeign
{
    Q_GADGET
    QML_FOREIGN(QPlaceSupplier)
    QML_NAMED_ELEMENT(supplier)
};

QT_END_NAMESPACE

#endif // QTLOCATION_QMLTYPES_H
