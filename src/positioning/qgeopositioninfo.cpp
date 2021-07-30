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
#include "qgeopositioninfo.h"
#include "private/qgeopositioninfo_p.h"
#include <QHash>
#include <QDebug>
#include <QDataStream>
#include <QtCore/QtNumeric>

#include <algorithm>

QT_BEGIN_NAMESPACE

/*!
    \class QGeoPositionInfo
    \inmodule QtPositioning
    \ingroup QtPositioning-positioning
    \ingroup shared
    \since 5.2

    \brief The QGeoPositionInfo class contains information gathered on a global position, direction and velocity at a particular point in time.

    A QGeoPositionInfo contains, at a minimum, a geographical coordinate and
    a timestamp. It may also have heading and speed measurements as well as
    estimates of the accuracy of the provided data.

    \sa QGeoPositionInfoSource
*/

/*!
    \enum QGeoPositionInfo::Attribute
    Defines the attributes for positional information.

    \value Direction The bearing measured in degrees clockwise from true north to the direction of travel.
    \value GroundSpeed The ground speed, in meters/sec.
    \value VerticalSpeed The vertical speed, in meters/sec.
    \value MagneticVariation The angle between the horizontal component of the magnetic field and true north, in degrees. Also known as magnetic declination. A positive value indicates a clockwise direction from true north and a negative value indicates a counter-clockwise direction.
    \value HorizontalAccuracy The accuracy of the provided latitude-longitude value, in meters.
    \value VerticalAccuracy The accuracy of the provided altitude value, in meters.

    NMEA protocol also suggests another type of accuracy - PositionAccuracy,
    which is a 3D accuracy value. Qt does not provide a separate attribute for
    it. If you need this value, you can calculate it based on the following
    formula:

    \c {PositionAccuracy} \sup 2 \c {= HorizontalAccuracy} \sup 2 \c { +
    VerticalAccuracy} \sup 2
*/

/*!
    Creates an invalid QGeoPositionInfo object.

    \sa isValid()
*/
QGeoPositionInfo::QGeoPositionInfo()
        : d(new QGeoPositionInfoPrivate)
{
}

/*!
    Creates a QGeoPositionInfo for the given \a coordinate and \a timestamp.
*/
QGeoPositionInfo::QGeoPositionInfo(const QGeoCoordinate &coordinate, const QDateTime &timestamp)
        : d(new QGeoPositionInfoPrivate)
{
    d->timestamp = timestamp;
    d->coord = coordinate;
}

/*!
    Creates a QGeoPositionInfo with the values of \a other.
*/
QGeoPositionInfo::QGeoPositionInfo(const QGeoPositionInfo &other)
        : d(other.d)
{
}

/*!
    \fn QGeoPositionInfo::QGeoPositionInfo(QGeoPositionInfo &&other) noexcept
    \since 6.2

    Creates a QGeoPositionInfo object by moving from \a other.

    Note that a moved-from QGeoPositionInfo can only be destroyed or
    assigned to. The effect of calling other functions than the destructor
    or one of the assignment operators is undefined.
*/

QGeoPositionInfo::QGeoPositionInfo(QGeoPositionInfoPrivate &dd) : d(&dd)
{
}

/*!
    Destroys a QGeoPositionInfo object.
*/
QGeoPositionInfo::~QGeoPositionInfo()
{
}

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QGeoPositionInfoPrivate)

/*!
    Assigns the values from \a other to this QGeoPositionInfo.
*/
QGeoPositionInfo &QGeoPositionInfo::operator=(const QGeoPositionInfo & other)
{
    if (this == &other)
        return *this;

    d = other.d;
    return *this;
}

/*!
    \fn QGeoPositionInfo &QGeoPositionInfo::operator=(QGeoPositionInfo &&other) noexcept
    \since 6.2

    Move-assigns the values from \a other to this object.

    Note that a moved-from QGeoPositionInfo can only be destroyed or
    assigned to. The effect of calling other functions than the destructor
    or one of the assignment operators is undefined.
*/

/*!
    \fn bool QGeoPositionInfo::operator==(const QGeoPositionInfo &lhs, const QGeoPositionInfo &rhs)

    Returns \c true if all of the \a lhs object's values are the same as those
    of \a rhs. Otherwise returns \c false.
*/

/*!
    \fn bool QGeoPositionInfo::operator!=(const QGeoPositionInfo &lhs, const QGeoPositionInfo &rhs)

    Returns \c true if any of the \a lhs object's values are not the same as
    those of \a rhs. Otherwise returns \c false.
*/

/*!
    Returns true if the timestamp() and coordinate() values are both valid.

    \sa QGeoCoordinate::isValid(), QDateTime::isValid()
*/
bool QGeoPositionInfo::isValid() const
{
    return d->timestamp.isValid() && d->coord.isValid();
}

/*!
    Sets the date and time at which this position was reported to \a timestamp.

    The \a timestamp must be in UTC time.

    \sa timestamp()
*/
void QGeoPositionInfo::setTimestamp(const QDateTime &timestamp)
{
    d.detach();
    d->timestamp = timestamp;
}

/*!
    Returns the date and time at which this position was reported, in UTC time.

    Returns an invalid QDateTime if no date/time value has been set.

    \sa setTimestamp()
*/
QDateTime QGeoPositionInfo::timestamp() const
{
    return d->timestamp;
}

/*!
    Sets the coordinate for this position to \a coordinate.

    \sa coordinate()
*/
void QGeoPositionInfo::setCoordinate(const QGeoCoordinate &coordinate)
{
    d.detach();
    d->coord = coordinate;
}

/*!
    Returns the coordinate for this position.

    Returns an invalid coordinate if no coordinate has been set.

    \sa setCoordinate()
*/
QGeoCoordinate QGeoPositionInfo::coordinate() const
{
    return d->coord;
}

/*!
    Sets the value for \a attribute to \a value.

    \sa attribute()
*/
void QGeoPositionInfo::setAttribute(Attribute attribute, qreal value)
{
    d.detach();
    d->doubleAttribs[attribute] = value;
}

/*!
    Returns the value of the specified \a attribute as a qreal value.

    Returns NaN if the value has not been set.

    The function hasAttribute() should be used to determine whether or
    not a value has been set for an attribute.

    \sa hasAttribute(), setAttribute()
*/
qreal QGeoPositionInfo::attribute(Attribute attribute) const
{
    if (d->doubleAttribs.contains(attribute))
        return d->doubleAttribs[attribute];
    return qQNaN();
}

/*!
    Removes the specified \a attribute and its value.
*/
void QGeoPositionInfo::removeAttribute(Attribute attribute)
{
    d.detach();
    d->doubleAttribs.remove(attribute);
}

/*!
    Returns true if the specified \a attribute is present for this
    QGeoPositionInfo object.
*/
bool QGeoPositionInfo::hasAttribute(Attribute attribute) const
{
    return d->doubleAttribs.contains(attribute);
}

/*!
    \internal
*/
void QGeoPositionInfo::detach()
{
    if (d)
        d.detach();
    else
        d = new QGeoPositionInfoPrivate;
}

bool QGeoPositionInfo::equals(const QGeoPositionInfo &lhs, const QGeoPositionInfo &rhs)
{
    return *lhs.d == *rhs.d;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QGeoPositionInfo::debugStreaming(QDebug dbg, const QGeoPositionInfo &info)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QGeoPositionInfo(" << info.d->timestamp;
    dbg.nospace() << ", "; // timestamp force dbg.space() -> reverting here
    dbg << info.d->coord;

    QList<QGeoPositionInfo::Attribute> attribs = info.d->doubleAttribs.keys();
    std::stable_sort(attribs.begin(), attribs.end()); // Output a sorted list from an unsorted hash.
    for (int i = 0; i < attribs.count(); ++i) {
        dbg << ", ";
        switch (attribs[i]) {
            case QGeoPositionInfo::Direction:
                dbg << "Direction=";
                break;
            case QGeoPositionInfo::GroundSpeed:
                dbg << "GroundSpeed=";
                break;
            case QGeoPositionInfo::VerticalSpeed:
                dbg << "VerticalSpeed=";
                break;
            case QGeoPositionInfo::MagneticVariation:
                dbg << "MagneticVariation=";
                break;
            case QGeoPositionInfo::HorizontalAccuracy:
                dbg << "HorizontalAccuracy=";
                break;
            case QGeoPositionInfo::VerticalAccuracy:
                dbg << "VerticalAccuracy=";
                break;
        }
        dbg << info.d->doubleAttribs[attribs[i]];
    }
    dbg << ')';
    return dbg;
}
#endif


#ifndef QT_NO_DATASTREAM
/*!
    \fn QDataStream &QGeoPositionInfo::operator<<(QDataStream &stream, QGeoPositionInfo::Attribute attr)

    Writes the given \a attr enumeration to the specified \a stream.

    \sa {Serializing Qt Data Types}
*/
QDataStream &QGeoPositionInfo::dataStreamOut(QDataStream &stream, QGeoPositionInfo::Attribute attr)
{
    return stream << int(attr);
}

/*!
    \fn QDataStream &QGeoPositionInfo::operator>>(QDataStream &stream, QGeoPositionInfo::Attribute &attr)

    Reads an attribute enumeration from the specified \a stream info the given \a attr.

    \sa {Serializing Qt Data Types}
*/
QDataStream &QGeoPositionInfo::dataStreamIn(QDataStream &stream, QGeoPositionInfo::Attribute &attr)
{
    int a;
    stream >> a;
    attr = static_cast<QGeoPositionInfo::Attribute>(a);
    return stream;
}

/*!
    \fn QDataStream &QGeoPositionInfo::operator<<(QDataStream &stream, const QGeoPositionInfo &info)

    Writes the given \a info to the specified \a stream.

    \sa {Serializing Qt Data Types}
*/

QDataStream &QGeoPositionInfo::dataStreamOut(QDataStream &stream, const QGeoPositionInfo &info)
{
    stream << info.d->timestamp;
    stream << info.d->coord;
    stream << info.d->doubleAttribs;
    return stream;
}

/*!
    \fn QDataStream &QGeoPositionInfo::operator>>(QDataStream &stream, QGeoPositionInfo &info)

    Reads a coordinate from the specified \a stream into the given
    \a info.

    \sa {Serializing Qt Data Types}
*/

QDataStream &QGeoPositionInfo::dataStreamIn(QDataStream &stream, QGeoPositionInfo &info)
{
    stream >> info.d->timestamp;
    stream >> info.d->coord;
    stream >> info.d->doubleAttribs;
    return stream;
}
#endif

QGeoPositionInfoPrivate::QGeoPositionInfoPrivate() : QSharedData()
{
}

QGeoPositionInfoPrivate::QGeoPositionInfoPrivate(const QGeoPositionInfoPrivate &other)
    : QSharedData(other),
      timestamp(other.timestamp),
      coord(other.coord),
      doubleAttribs(other.doubleAttribs)
{
}

QGeoPositionInfoPrivate::~QGeoPositionInfoPrivate()
{

}

bool QGeoPositionInfoPrivate::operator==(const QGeoPositionInfoPrivate &other) const
{
    return timestamp == other.timestamp
           && coord == other.coord
            && doubleAttribs == other.doubleAttribs;
}

QGeoPositionInfoPrivate *QGeoPositionInfoPrivate::get(const QGeoPositionInfo &info)
{
    return info.d.data();
}

size_t qHash(const QGeoPositionInfo &key, size_t seed) noexcept
{
    return qHashMulti(seed, key.d->coord);
}

namespace QTest
{

char *toString(const QGeoPositionInfo &info)
{
    QString result;
    QDebug dbg(&result);
    dbg << info;

    return qstrdup(qPrintable(result));
}

} // namespace QTest

QT_END_NAMESPACE
