/****************************************************************************
**
** Copyright (C) 2015 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "geocluetypes.h"

const QDBusArgument &dbus_argument_helper(const QDBusArgument &arg, Accuracy &accuracy)
{
    arg.beginStructure();
    qint32 level;
    arg >> level;
    accuracy.m_level = static_cast<Accuracy::Level>(level);
    arg >> accuracy.m_horizontal;
    arg >> accuracy.m_vertical;
    arg.endStructure();

    return arg;
}

QT_BEGIN_NAMESPACE

QDBusArgument &operator<<(QDBusArgument &arg, const Accuracy &accuracy)
{
    arg.beginStructure();
    arg << qint32(accuracy.level());
    arg << accuracy.horizontal();
    arg << accuracy.vertical();
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Accuracy &accuracy)
{
    return dbus_argument_helper(arg, accuracy);
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QGeoSatelliteInfo &si)
{
    qint32 a;

    argument.beginStructure();
    argument >> a;
    si.setSatelliteIdentifier(a);
    argument >> a;
    si.setAttribute(QGeoSatelliteInfo::Elevation, a);
    argument >> a;
    si.setAttribute(QGeoSatelliteInfo::Azimuth, a);
    argument >> a;
    si.setSignalStrength(a);
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QList<QGeoSatelliteInfo> &sis)
{
    sis.clear();

    argument.beginArray();
    while (!argument.atEnd()) {
        QGeoSatelliteInfo si;
        argument >> si;
        sis.append(si);
    }
    argument.endArray();

    return argument;
}

QT_END_NAMESPACE
