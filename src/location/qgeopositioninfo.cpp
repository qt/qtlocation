/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeopositioninfo.h"

#include <QHash>
#include <QDebug>
#include <QDataStream>

QTM_BEGIN_NAMESPACE

class QGeoPositionInfoPrivate
{
public:
    QDateTime timestamp;
    QGeoCoordinate coord;
    QHash<int, qreal> doubleAttribs;
};

/*!
    \class QGeoPositionInfo
    \brief The QGeoPositionInfo class contains information gathered on a global position, direction and velocity at a particular point in time.

    \inmodule QtLocation
    \since 1.0

    \ingroup location

    A QGeoPositionInfo contains, at a minimum, a geographical coordinate and
    a timestamp. It may also have heading and speed measurements as well as
    estimates of the accuracy of the provided data.

    \sa QGeoPositionInfoSource
*/

/*!
    \enum QGeoPositionInfo::Attribute
    Defines the attributes for positional information.

    \value Direction The bearing to true north from the direction of travel, in degrees.
    \value GroundSpeed The ground speed, in metres/sec.
    \value VerticalSpeed The vertical speed, in metres/sec.
    \value MagneticVariation The angle between the horizontal component of the magnetic field and true north, in degrees. Also known as magnetic declination. A positive value indicates a clockwise direction from true north and a negative value indicates a counter-clockwise direction.
    \value HorizontalAccuracy The accuracy of the provided latitude-longitude value, in metres.
    \value VerticalAccuracy The accuracy of the provided altitude value, in metres.
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
        : d(new QGeoPositionInfoPrivate)
{
    operator=(other);
}

/*!
    Destroys a QGeoPositionInfo object.
*/
QGeoPositionInfo::~QGeoPositionInfo()
{
    delete d;
}

/*!
    Assigns the values from \a other to this QGeoPositionInfo.
*/
QGeoPositionInfo &QGeoPositionInfo::operator=(const QGeoPositionInfo & other)
{
    if (this == &other)
        return *this;

    d->timestamp = other.d->timestamp;
    d->coord = other.d->coord;
    d->doubleAttribs = other.d->doubleAttribs;

    return *this;
}

/*!
    Returns true if all of this object's values are the same as those of
    \a other.
*/
bool QGeoPositionInfo::operator==(const QGeoPositionInfo &other) const
{
    return d->timestamp == other.d->timestamp
           && d->coord == other.d->coord
           && d->doubleAttribs == other.d->doubleAttribs;
}

/*!
    \fn bool QGeoPositionInfo::operator!=(const QGeoPositionInfo &other) const

    Returns true if any of this object's values are not the same as those of
    \a other.
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
    d->doubleAttribs[int(attribute)] = value;
}

/*!
    Returns the value of the specified \a attribute as a qreal value.

    Returns -1 if the value has not been set, although this may also
    be a legitimate value for some attributes.

    The function hasAttribute() should be used to determine whether or
    not a value has been set for an attribute.

    \sa hasAttribute(), setAttribute()
*/
qreal QGeoPositionInfo::attribute(Attribute attribute) const
{
    if (d->doubleAttribs.contains(int(attribute)))
        return d->doubleAttribs[int(attribute)];
    return -1;
}

/*!
    Removes the specified \a attribute and its value.
*/
void QGeoPositionInfo::removeAttribute(Attribute attribute)
{
    d->doubleAttribs.remove(int(attribute));
}

/*!
    Returns true if the specified \a attribute is present for this
    QGeoPositionInfo object.
*/
bool QGeoPositionInfo::hasAttribute(Attribute attribute) const
{
    return d->doubleAttribs.contains(int(attribute));
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGeoPositionInfo &info)
{
    dbg.nospace() << "QGeoPositionInfo(" << info.d->timestamp;
    dbg.nospace() << ", ";
    dbg.nospace() << info.d->coord;

    QList<int> attribs = info.d->doubleAttribs.keys();
    for (int i = 0; i < attribs.count(); i++) {
        dbg.nospace() << ", ";
        switch (attribs[i]) {
            case QGeoPositionInfo::Direction:
                dbg.nospace() << "Direction=";
                break;
            case QGeoPositionInfo::GroundSpeed:
                dbg.nospace() << "GroundSpeed=";
                break;
            case QGeoPositionInfo::VerticalSpeed:
                dbg.nospace() << "VerticalSpeed=";
                break;
            case QGeoPositionInfo::MagneticVariation:
                dbg.nospace() << "MagneticVariation=";
                break;
            case QGeoPositionInfo::HorizontalAccuracy:
                dbg.nospace() << "HorizontalAccuracy=";
                break;
            case QGeoPositionInfo::VerticalAccuracy:
                dbg.nospace() << "VerticalAccuracy=";
                break;
        }
        dbg.nospace() << info.d->doubleAttribs[attribs[i]];
    }
    dbg.nospace() << ')';
    return dbg;
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
    \fn QDataStream &operator<<(QDataStream &stream, const QGeoPositionInfo &info)
    \relates QGeoPositionInfo

    Writes the given \a info to the specified \a stream.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator<<(QDataStream &stream, const QGeoPositionInfo &info)
{
    stream << info.d->timestamp;
    stream << info.d->coord;
    stream << info.d->doubleAttribs;
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
    \fn QDataStream &operator>>(QDataStream &stream, QGeoPositionInfo &info)
    \relates QGeoPositionInfo

    Reads a coordinate from the specified \a stream into the given
    \a info.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator>>(QDataStream &stream, QGeoPositionInfo &info)
{
    stream >> info.d->timestamp;
    stream >> info.d->coord;
    stream >> info.d->doubleAttribs;
    return stream;
}
#endif

QTM_END_NAMESPACE
