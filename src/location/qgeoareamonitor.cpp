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
#include "qgeoareamonitor.h"

#if defined(Q_OS_SYMBIAN) && defined(QT_LOCATION_S60_MONITORING)
#include "qgeoareamonitor_s60_p.h"
#endif
#include "qgeoareamonitor_polling_p.h"

/*!
    \class QGeoAreaMonitor
    \brief The QGeoAreaMonitor class enables the detection of proximity
    changes for a specified set of coordinates.

    \inmodule QtLocation
    \since 1.0

    \ingroup location

    A QGeoAreaMonitor emits signals when the current position is in
    range, or has moved out of range, of a specified circular area.
    The area is specified by a coordinate (the center point) and a
    radius (in meters).

    For example:

    \code
        public:
            MyClass::MyClass()
            {
                QGeoAreaMonitor *monitor = QGeoAreaMonitor::createDefaultMonitor();
                connect(monitor, SIGNAL(areaEntered(QGeoPositionInfo)),
                        this, SLOT(areaEntered(QGeoPositionInfo)));
                connect(monitor, SIGNAL(areaExited(QGeoPositionInfo)),
                        this, SLOT(areaExited(QGeoPositionInfo)));

                QGeoCoordinate bigBenLocation(51.50104, -0.124632);
                monitor->setCenter(bigBenLocation);
                monitor->setRadius(100);
            }

        public Q_SLOTS:
            void areaEntered(const QGeoPositionInfo &update)
            {
                qDebug() << "Now within 100 meters, current position is" << update.coordinate();
            }

            void areaExited(const QGeoPositionInfo &update)
            {
                qDebug() << "No longer within 100 meters, current position is" << update.coordinate();
            }
    \endcode
*/

QTM_BEGIN_NAMESPACE

class QGeoAreaMonitorPrivate
{
public:
    QGeoCoordinate coord;
    qreal radius;
};


/*!
    Creates a monitor with the given \a parent.
*/
QGeoAreaMonitor::QGeoAreaMonitor(QObject *parent)
        : QObject(parent),
        d(new QGeoAreaMonitorPrivate)
{
    d->radius = qreal(0.0);
}

/*!
    Destroys the monitor.
*/
QGeoAreaMonitor::~QGeoAreaMonitor()
{
    delete d;
}

/*!
    \property QGeoAreaMonitor::center
    \brief holds the center of the area to be monitored.

    When the center is set, if the radius has already been set and
    the current position is within the monitored area, areaEntered()
    is emitted immediately.

    By default, contains an invalid coordinate.

    Note: Subclass implementations must call the base implementation of
    setCenter() so that center() returns the correct value.
*/
void QGeoAreaMonitor::setCenter(const QGeoCoordinate &coordinate)
{
    d->coord = coordinate;
}

QGeoCoordinate QGeoAreaMonitor::center() const
{
    return d->coord;
}

/*!
    \property QGeoAreaMonitor::radius
    \brief holds the radius of the area to be monitored, in meters.

    If the specified radius is less than the minimum supported radius, the
    radius is set to the minimum radius.

    When this property is set, if the center coordinate has already been set and
    the current position is within the monitored area, areaEntered()
    is emitted immediately.

    By default, this property is 0.

    Note: Subclass implementations must call the base implementation of
    setRadius() so that radius() returns the correct value.
*/
void QGeoAreaMonitor::setRadius(qreal radius)
{
    d->radius = radius;
}

qreal QGeoAreaMonitor::radius() const
{
    return d->radius;
}

/*!
    Creates and returns a monitor with the given \a parent that
    monitors areas using resources on the underlying system.

    Returns 0 if the system has no support for position monitoring.

    Note: Symbian applications will need to have the Location capability
    otherwise this will return 0.
*/
QGeoAreaMonitor *QGeoAreaMonitor::createDefaultMonitor(QObject *parent)
{
    // Native Symbian area monitor is temporarily disabled,
    // see http://bugreports.qt.nokia.com/browse/QTMOBILITY-1059
//#if defined(Q_OS_SYMBIAN) && defined(QT_LOCATION_S60_MONITORING)
//    QGeoAreaMonitor *ret = NULL;
//    TRAPD(error, ret = QGeoAreaMonitorS60::NewL(parent));
//    return ret;
//#else
    QGeoAreaMonitorPolling *ret = NULL;
    ret = new QGeoAreaMonitorPolling(parent);
    if (ret && ret->isValid())
        return ret;
//#endif
    return 0;
}

/*!
    \fn void QGeoAreaMonitor::areaEntered(const QGeoPositionInfo &update);

    Emitted when the current position has moved from a position outside the
    monitored area to a position within the monitored area.

    The \a update holds the new position.
*/

/*!
    \fn void QGeoAreaMonitor::areaExited(const QGeoPositionInfo &update);

    Emitted when the current position has moved from a position within the
    monitored area to a position outside the monitored area.

    The \a update holds the new position.
*/

QTM_END_NAMESPACE
