/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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
#ifndef QGEOSATELLITEINFO_H
#define QGEOSATELLITEINFO_H

#include <QtPositioning/qpositioningglobal.h>
#include <QtCore/QSharedData>
#include <QtCore/QMetaType>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

class QDebug;
class QDataStream;

class QGeoSatelliteInfo;
Q_POSITIONING_EXPORT size_t qHash(const QGeoSatelliteInfo &key, size_t seed = 0) noexcept;
namespace QTest
{

Q_POSITIONING_EXPORT char *toString(const QGeoSatelliteInfo &info);

} // namespace QTest

class QGeoSatelliteInfoPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoSatelliteInfoPrivate, Q_POSITIONING_EXPORT)

class Q_POSITIONING_EXPORT QGeoSatelliteInfo
{
public:
    enum Attribute {
        Elevation,
        Azimuth
    };

    enum SatelliteSystem {
        Undefined = 0x00,
        GPS = 0x01,
        GLONASS = 0x02,
        GALILEO = 0x03,
        BEIDOU = 0x04,
        QZSS = 0x05,
        Multiple = 0xFF,
        CustomType = 0x100
    };

    QGeoSatelliteInfo();
    QGeoSatelliteInfo(const QGeoSatelliteInfo &other);
    QGeoSatelliteInfo(QGeoSatelliteInfoPrivate &dd);
    QGeoSatelliteInfo(QGeoSatelliteInfo &&other) noexcept = default;
    ~QGeoSatelliteInfo();

    QGeoSatelliteInfo &operator=(const QGeoSatelliteInfo &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QGeoSatelliteInfo)

    void swap(QGeoSatelliteInfo &other) noexcept { d.swap(other.d); }

    friend bool operator==(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs)
    {
        return !equals(lhs, rhs);
    }

    void setSatelliteSystem(SatelliteSystem system);
    SatelliteSystem satelliteSystem() const;

    void setSatelliteIdentifier(int satId);
    int satelliteIdentifier() const;

    void setSignalStrength(int signalStrength);
    int signalStrength() const;

    void setAttribute(Attribute attribute, qreal value);
    qreal attribute(Attribute attribute) const;
    void removeAttribute(Attribute attribute);

    bool hasAttribute(Attribute attribute) const;

    void detach();

private:
    static bool equals(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs);
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const QGeoSatelliteInfo &info)
    {
        return debugStreaming(dbg, info);
    }
    static QDebug debugStreaming(QDebug dbg, const QGeoSatelliteInfo &info);
#endif
#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoSatelliteInfo &info)
    {
        return dataStreamOut(stream, info);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoSatelliteInfo &info)
    {
        return dataStreamIn(stream, info);
    }
    static QDataStream &dataStreamOut(QDataStream &stream, const QGeoSatelliteInfo &info);
    static QDataStream &dataStreamIn(QDataStream &stream, QGeoSatelliteInfo &info);
#endif
    QExplicitlySharedDataPointer<QGeoSatelliteInfoPrivate> d;
    friend class QGeoSatelliteInfoPrivate;

    friend Q_POSITIONING_EXPORT size_t qHash(const QGeoSatelliteInfo &key, size_t seed) noexcept;
    friend Q_POSITIONING_EXPORT char *QTest::toString(const QGeoSatelliteInfo &info);
};

Q_DECLARE_SHARED(QGeoSatelliteInfo)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoSatelliteInfo)

#endif
