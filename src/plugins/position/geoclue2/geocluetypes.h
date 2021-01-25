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

#ifndef GEOCLUETYPES_H
#define GEOCLUETYPES_H

#include <QtDBus/QDBusArgument>

class Timestamp
{
public:
    quint64 m_seconds = 0;
    quint64 m_microseconds = 0;
};

Q_DECLARE_METATYPE(Timestamp)

QT_BEGIN_NAMESPACE

Q_DECLARE_TYPEINFO(Timestamp, Q_MOVABLE_TYPE);

QDBusArgument &operator<<(QDBusArgument &arg, const Timestamp &ts);
const QDBusArgument &operator>>(const QDBusArgument &arg, Timestamp &ts);

QT_END_NAMESPACE

#endif // GEOCLUETYPES_H
