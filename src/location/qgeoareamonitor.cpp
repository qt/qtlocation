/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeoareamonitor.h"

#include "qgeoareamonitor_polling_p.h"

/*!
    \class QGeoAreaMonitor
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \since Qt Location 5.0

    \brief The QGeoAreaMonitor class enables the detection of proximity
    changes for a specified set of coordinates.

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
                if (monitor) {
                    connect(monitor, SIGNAL(areaEntered(QGeoPositionInfo)),
                            this, SLOT(areaEntered(QGeoPositionInfo)));
                    connect(monitor, SIGNAL(areaExited(QGeoPositionInfo)),
                            this, SLOT(areaExited(QGeoPositionInfo)));

                    QGeoCoordinate bigBenLocation(51.50104, -0.124632);
                    monitor->setCenter(bigBenLocation);
                    monitor->setRadius(100);
                } else {
                    qDebug() << "Could not create default area monitor";
                }
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

QT_BEGIN_NAMESPACE

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
*/
QGeoAreaMonitor *QGeoAreaMonitor::createDefaultMonitor(QObject *parent)
{
    QGeoAreaMonitorPolling *ret = NULL;
    ret = new QGeoAreaMonitorPolling(parent);
    if (ret && ret->isValid())
        return ret;
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

QT_END_NAMESPACE
