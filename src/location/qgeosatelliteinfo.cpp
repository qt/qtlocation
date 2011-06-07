/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeosatelliteinfo.h"

#include <QHash>
#include <QDebug>
#include <QDataStream>

QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoPrivate
{
public:
    int prn;
    int signal;
    QHash<int, qreal> doubleAttribs;
};


/*!
    \class QGeoSatelliteInfo
    \brief The QGeoSatelliteInfo class contains basic information about a satellite.

    \inmodule QtLocation
    \since 1.0

    \ingroup location

    \sa QGeoSatelliteInfoSource
*/

/*!
    \enum QGeoSatelliteInfo::Attribute
    Defines the attributes for the satellite information.

    \value Elevation The elevation of the satellite, in degrees.
    \value Azimuth The azimuth to true north, in degrees.
*/


/*!
    Creates a satellite information object.
*/
QGeoSatelliteInfo::QGeoSatelliteInfo()
        : d(new QGeoSatelliteInfoPrivate)
{
    d->prn = -1;
    d->signal = -1;
}

/*!
    Creates a satellite information object with the values of \a other.
    \since 1.0
*/

QGeoSatelliteInfo::QGeoSatelliteInfo(const QGeoSatelliteInfo &other)
        : d(new QGeoSatelliteInfoPrivate)
{
    operator=(other);
}

/*!
    Destroys a satellite information object.
*/
QGeoSatelliteInfo::~QGeoSatelliteInfo()
{
    delete d;
}

/*!
    Assigns the values from \a other to this object.
    \since 1.0
*/
QGeoSatelliteInfo &QGeoSatelliteInfo::operator=(const QGeoSatelliteInfo & other)
{
    if (this == &other)
        return *this;

    d->prn = other.d->prn;
    d->signal = other.d->signal;
    d->doubleAttribs = other.d->doubleAttribs;
    return *this;
}

/*!
    Returns true if all the information for this satellite
    are the same as those of \a other.
    \since 1.0
*/
bool QGeoSatelliteInfo::operator==(const QGeoSatelliteInfo &other) const
{
    return d->prn == other.d->prn
           && d->signal == other.d->signal
           && d->doubleAttribs == other.d->doubleAttribs;
}

/*!
    \fn bool QGeoSatelliteInfo::operator!=(const QGeoSatelliteInfo &other) const;

    Returns true if any of the information for this satellite
    are not the same as those of \a other.
    \since 1.0
*/

/*!
    Sets the PRN (Pseudo-random noise) number to \a prn.

    The PRN number can be used to identify a satellite.
    \since 1.0
*/
void QGeoSatelliteInfo::setPrnNumber(int prn)
{
    d->prn = prn;
}

/*!
    Returns the PRN (Pseudo-random noise) number, or -1 if the value has not been set.
    \since 1.0
*/

int QGeoSatelliteInfo::prnNumber() const
{
    return d->prn;
}

/*!
    Sets the signal strength to \a signalStrength, in decibels.
    \since 1.0
*/
void QGeoSatelliteInfo::setSignalStrength(int signalStrength)
{
    d->signal = signalStrength;
}

/*!
    Returns the signal strength, or -1 if the value has not been set.
    \since 1.0
*/
int QGeoSatelliteInfo::signalStrength() const
{
    return d->signal;
}

/*!
    Sets the value for \a attribute to \a value.
    \since 1.0
*/
void QGeoSatelliteInfo::setAttribute(Attribute attribute, qreal value)
{
    d->doubleAttribs[int(attribute)] = value;
}

/*!
    Returns the value of the specified \a attribute as a qreal value.

    Returns -1 if the value has not been set.

    \since 1.0
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
    \since 1.0
*/
void QGeoSatelliteInfo::removeAttribute(Attribute attribute)
{
    d->doubleAttribs.remove(int(attribute));
}

/*!
    Returns true if the specified \a attribute is present in this update.
    \since 1.0
*/
bool QGeoSatelliteInfo::hasAttribute(Attribute attribute) const
{
    return d->doubleAttribs.contains(int(attribute));
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGeoSatelliteInfo &info)
{
    dbg.nospace() << "QGeoSatelliteInfo(PRN=" << info.d->prn;
    dbg.nospace() << ", signal-strength=";
    dbg.nospace() << info.d->signal;

    QList<int> attribs = info.d->doubleAttribs.keys();
    for (int i = 0; i < attribs.count(); i++) {
        dbg.nospace() << ", ";
        switch (attribs[i]) {
            case QGeoSatelliteInfo::Elevation:
                dbg.nospace() << "Elevation=";
                break;
            case QGeoSatelliteInfo::Azimuth:
                dbg.nospace() << "Azimuth=";
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
    \fn QDataStream &operator<<(QDataStream &stream, const QGeoSatelliteInfo &info)
    \relates QGeoSatelliteInfo

    Writes the given \a info to the specified \a stream.

    \since 1.0
    \sa \link datastreamformat.html Format of the QDataStream operators \endlink

*/

QDataStream &operator<<(QDataStream &stream, const QGeoSatelliteInfo &info)
{
    stream << info.d->prn;
    stream << info.d->signal;
    stream << info.d->doubleAttribs;
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
    \fn QDataStream &operator>>(QDataStream &stream, QGeoSatelliteInfo &info)
    \relates QGeoSatelliteInfo

    Reads satellite information from the specified \a stream into the given
    \a info.

    \since 1.0
    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/

QDataStream &operator>>(QDataStream &stream, QGeoSatelliteInfo &info)
{
    stream >> info.d->prn;
    stream >> info.d->signal;
    stream >> info.d->doubleAttribs;
    return stream;
}
#endif

QTM_END_NAMESPACE
