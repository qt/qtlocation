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

#ifndef GEOCLUETYPES_H
#define GEOCLUETYPES_H

#include <QtDBus/QDBusArgument>
#include <QtPositioning/QGeoSatelliteInfo>

class Accuracy
{
public:
    enum Level {
        None = 0,
        Country,
        Region,
        Locality,
        PostalCode,
        Street,
        Detailed
    };

    Accuracy()
    :   m_level(None), m_horizontal(0), m_vertical(0)
    {
    }

    inline Level level() const { return m_level; }
    inline double horizontal() const { return m_horizontal; }
    inline double vertical() const { return m_vertical; }

private:
    Level m_level;
    double m_horizontal;
    double m_vertical;

    friend const QDBusArgument &dbus_argument_helper(const QDBusArgument &arg, Accuracy &accuracy);
};

Q_DECLARE_METATYPE(Accuracy)
Q_DECLARE_METATYPE(QList<QGeoSatelliteInfo>)


QT_BEGIN_NAMESPACE

Q_DECLARE_TYPEINFO(Accuracy, Q_MOVABLE_TYPE);

QDBusArgument &operator<<(QDBusArgument &arg, const Accuracy &accuracy);
const QDBusArgument &operator>>(const QDBusArgument &arg, Accuracy &accuracy);

const QDBusArgument &operator>>(const QDBusArgument &arg, QGeoSatelliteInfo &si);
const QDBusArgument &operator>>(const QDBusArgument &arg, QList<QGeoSatelliteInfo> &sis);

QT_END_NAMESPACE

#endif // GEOCLUETYPES_H

