/****************************************************************************
**
** Copyright (C) 2018 Denis Shienkov <denis.shienkov@gmail.com>
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

QT_BEGIN_NAMESPACE

QDBusArgument &operator<<(QDBusArgument &arg, const Timestamp &ts)
{
    arg.beginStructure();
    arg << ts.m_seconds;
    arg << ts.m_microseconds;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Timestamp &ts)
{
    arg.beginStructure();
    arg >> ts.m_seconds;
    arg >> ts.m_microseconds;
    arg.endStructure();
    return arg;
}

QT_END_NAMESPACE
