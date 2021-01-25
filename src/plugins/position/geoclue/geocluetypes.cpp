/****************************************************************************
**
** Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Contact: http://www.qt-project.org/legal
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
