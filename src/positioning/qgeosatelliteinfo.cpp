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
#include "qgeosatelliteinfo.h"
#include "private/qgeosatelliteinfo_p.h"

#include <QHash>
#include <QDebug>
#include <QDataStream>

QT_BEGIN_NAMESPACE

/*!
    \class QGeoSatelliteInfo
    \inmodule QtPositioning
    \ingroup QtPositioning-positioning
    \ingroup shared
    \since 5.2

    \brief The QGeoSatelliteInfo class contains basic information about a satellite.

    \sa QGeoSatelliteInfoSource
*/

/*!
    \enum QGeoSatelliteInfo::Attribute
    Defines the attributes for the satellite information.
    \value Elevation The elevation of the satellite, in degrees.
    \value Azimuth The azimuth to true north, in degrees.
*/

/*!
    \enum QGeoSatelliteInfo::SatelliteSystem
    Defines the GNSS system of the satellite.
    \value Undefined Not defined.
    \value GPS Global Positioning System (USA).
    \value GLONASS Global Positioning System (Russia).
    \value GALILEO Global navigation satellite system (EU).
    \value BEIDOU BeiDou navigation satellite system (China).
    \value QZSS Quasi-Zenith Satellite System (Japan).
    \value Multiple This type normally indicates that the information is
           received from a device that supports multiple satellite systems, and
           the satellite system is not explicitly specified. Depending on the
           data source, you might use other information to determine the actual
           system type. One example of the usage of this type is an NMEA $GNGSA
           message, which contains the IDs of the satellites being used, but
           does not explicitly mention their system types.
    \value CustomType The first type that can be used for user purposes. For
           example when reimplementing NMEA data parsing in
           \l QNmeaSatelliteInfoSource. User can add more types using
           \c {CustomType + 1}, \c {CustomType + 2} and so on.
*/

/*!
    Creates a satellite information object.
*/
QGeoSatelliteInfo::QGeoSatelliteInfo()
        : d(new QGeoSatelliteInfoPrivate)
{
    d->signal = -1;
    d->satId = -1;
    d->system = QGeoSatelliteInfo::Undefined;
}

/*!
    Creates a satellite information object with the values of \a other.
*/

QGeoSatelliteInfo::QGeoSatelliteInfo(const QGeoSatelliteInfo &other)
        : d(other.d)
{
}

QGeoSatelliteInfo::QGeoSatelliteInfo(QGeoSatelliteInfoPrivate &dd) : d(&dd)
{
}

/*!
    \fn QGeoSatelliteInfo::QGeoSatelliteInfo(QGeoSatelliteInfo &&other) noexcept
    \since 6.2

    Creates a satellite information object by moving from \a other.

    Note that a moved-from QGeoSatelliteInfo can only be destroyed or
    assigned to. The effect of calling other functions than the destructor
    or one of the assignment operators is undefined.
*/

/*!
    Destroys a satellite information object.
*/
QGeoSatelliteInfo::~QGeoSatelliteInfo()
{
}

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QGeoSatelliteInfoPrivate)

/*!
    Assigns the values from \a other to this object.
*/
QGeoSatelliteInfo &QGeoSatelliteInfo::operator=(const QGeoSatelliteInfo & other)
{
    if (this == &other)
        return *this;

    d = other.d;
    return *this;
}

/*!
    \fn QGeoSatelliteInfo &QGeoSatelliteInfo::operator=(QGeoSatelliteInfo &&other) noexcept
    \since 6.2

    Move-assigns the value from \a other to this object

    Note that a moved-from QGeoSatelliteInfo can only be destroyed or
    assigned to. The effect of calling other functions than the destructor
    or one of the assignment operators is undefined.
*/

/*!
    \fn bool QGeoSatelliteInfo::operator==(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs)

    Returns \c true if all the parameters of the \a lhs satellite are the same
    as those of \a rhs. Otherwise returns \c false.
*/

/*!
    \fn bool QGeoSatelliteInfo::operator!=(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs)

    Returns \c true if any of the parameters of the \a lhs satellite are not
    the same as those of \a rhs. Otherwise returns \c false.
*/

/*!
    Sets the Satellite System (GPS, GLONASS, ...) to \a system.
*/
void QGeoSatelliteInfo::setSatelliteSystem(SatelliteSystem system)
{
    d.detach();
    d->system = system;
}

/*!
    Returns the Satellite System (GPS, GLONASS, ...)

    \note This value can be used together with \l satelliteIdentifier()
    to uniquely identify a satellite.

    \sa satelliteIdentifier()
*/
QGeoSatelliteInfo::SatelliteSystem QGeoSatelliteInfo::satelliteSystem() const
{
    return d->system;
}

/*!
    Sets the satellite identifier number to \a satId.

    The satellite identifier number can be used to identify a satellite within
    the satellite system.

    The actual value may vary, depending on the platform and the selected
    backend.

    For example, if \e nmea plugin is used, the satellite identifier for GPS
    satellite system represents the PRN (Pseudo-random noise) number, and the
    satellite identifier for GLONASS satellite system represents the slot
    number.
*/
void QGeoSatelliteInfo::setSatelliteIdentifier(int satId)
{
    d.detach();
    d->satId = satId;
}

/*!
    Returns the satellite identifier number.

    The satellite identifier number can be used to identify a satellite within
    the satellite system.

    The actual value may vary, depending on the platform and the selected
    backend.

    For example, if \e nmea plugin is used, the satellite identifier for GPS
    satellite system represents the PRN (Pseudo-random noise) number, and the
    satellite identifier for GLONASS satellite system represents the slot
    number.

    For NMEA-based backends the satellite identifier can be used to determine
    the satellite system type if it is not available from other sources.
    You can refer to \l {https://gpsd.gitlab.io/gpsd/NMEA.html#_satellite_ids}
    {satellite IDs list} to check the ID ranges for different satellite systems.

    \note Depending on the platform and the selected backend, the satellite
    identifier ranges for different satellite systems may intersect. To uniquely
    identify a satellite, a combination of satelliteIndetifier() and
    \l satelliteSystem() must be used.

    \sa satelliteSystem()
*/
int QGeoSatelliteInfo::satelliteIdentifier() const
{
    return d->satId;
}

/*!
    Sets the signal strength to \a signalStrength, in decibels.
*/
void QGeoSatelliteInfo::setSignalStrength(int signalStrength)
{
    d.detach();
    d->signal = signalStrength;
}

/*!
    Returns the signal strength, or -1 if the value has not been set.
*/
int QGeoSatelliteInfo::signalStrength() const
{
    return d->signal;
}

/*!
    Sets the value for \a attribute to \a value.
*/
void QGeoSatelliteInfo::setAttribute(Attribute attribute, qreal value)
{
    d.detach();
    d->doubleAttribs[int(attribute)] = value;
}

/*!
    Returns the value of the specified \a attribute as a qreal value.

    Returns -1 if the value has not been set.

    \sa hasAttribute(), setAttribute()
*/
qreal QGeoSatelliteInfo::attribute(Attribute attribute) const
{
    if (d->doubleAttribs.contains(int(attribute)))
        return d->doubleAttribs[int(attribute)];
    return -1;
}

/*!
    Removes the specified \a attribute and its value.
*/
void QGeoSatelliteInfo::removeAttribute(Attribute attribute)
{
    d.detach();
    d->doubleAttribs.remove(int(attribute));
}

/*!
    Returns true if the specified \a attribute is present in this update.
*/
bool QGeoSatelliteInfo::hasAttribute(Attribute attribute) const
{
    return d->doubleAttribs.contains(int(attribute));
}

/*!
    \internal
*/
void QGeoSatelliteInfo::detach()
{
    if (d)
        d.detach();
    else
        d = new QGeoSatelliteInfoPrivate;
}

bool QGeoSatelliteInfo::equals(const QGeoSatelliteInfo &lhs, const QGeoSatelliteInfo &rhs)
{
    return *lhs.d == *rhs.d;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QGeoSatelliteInfo::debugStreaming(QDebug dbg, const QGeoSatelliteInfo &info)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QGeoSatelliteInfo(system=" << info.d->system;
    dbg << ", satId=" << info.d->satId;
    dbg << ", signal-strength=" << info.d->signal;


    QList<int> attribs = info.d->doubleAttribs.keys();
    for (int i = 0; i < attribs.count(); ++i) {
        dbg << ", ";
        switch (attribs[i]) {
            case QGeoSatelliteInfo::Elevation:
                dbg << "Elevation=";
                break;
            case QGeoSatelliteInfo::Azimuth:
                dbg << "Azimuth=";
                break;
        }
        dbg << info.d->doubleAttribs[attribs[i]];
    }
    dbg << ')';
    return dbg;
}
#endif // QT_NO_DEBUG_STREAM

#ifndef QT_NO_DATASTREAM
/*!
    \fn QDataStream &QGeoSatelliteInfo::operator<<(QDataStream &stream, const QGeoSatelliteInfo &info)

    Writes the given \a info to the specified \a stream.

    \sa {Serializing Qt Data Types}

*/

QDataStream &QGeoSatelliteInfo::dataStreamOut(QDataStream &stream, const QGeoSatelliteInfo &info)
{
    stream << info.d->signal;
    stream << info.d->doubleAttribs;
    stream << info.d->satId;
    stream << int(info.d->system);
    return stream;
}
#endif // QT_NO_DATASTREAM

#ifndef QT_NO_DATASTREAM
/*!
    \fn QDataStream &QGeoSatelliteInfo::operator>>(QDataStream &stream, QGeoSatelliteInfo &info)

    Reads satellite information from the specified \a stream into the given
    \a info.

    \sa {Serializing Qt Data Types}
*/

QDataStream &QGeoSatelliteInfo::dataStreamIn(QDataStream &stream, QGeoSatelliteInfo &info)
{
    int system;
    stream >> info.d->signal;
    stream >> info.d->doubleAttribs;
    stream >> info.d->satId;
    stream >> system;
    info.d->system = (QGeoSatelliteInfo::SatelliteSystem)system;
    return stream;
}
#endif // QT_NO_DATASTREAM

QGeoSatelliteInfoPrivate::QGeoSatelliteInfoPrivate() : QSharedData()
{

}

QGeoSatelliteInfoPrivate::QGeoSatelliteInfoPrivate(const QGeoSatelliteInfoPrivate &other)
    : QSharedData(other)
{
    signal = other.signal;
    satId = other.satId;
    system = other.system;
    doubleAttribs = other.doubleAttribs;
}

QGeoSatelliteInfoPrivate::~QGeoSatelliteInfoPrivate() {}

bool QGeoSatelliteInfoPrivate::operator==(const QGeoSatelliteInfoPrivate &other) const
{
    return signal == other.signal
           && satId == other.satId
           && system == other.system
           && doubleAttribs == other.doubleAttribs;
}

QGeoSatelliteInfoPrivate *QGeoSatelliteInfoPrivate::get(const QGeoSatelliteInfo &info)
{
    return info.d.data();
}

size_t qHash(const QGeoSatelliteInfo &key, size_t seed) noexcept
{
    // Other properties and attributes might change
    return qHashMulti(seed, key.d->satId, key.d->system);
}

namespace QTest
{

char *toString(const QGeoSatelliteInfo &info)
{
    QString result;
    QDebug dbg(&result);
    dbg << info;

    return qstrdup(qPrintable(result));
}

}


QT_END_NAMESPACE
