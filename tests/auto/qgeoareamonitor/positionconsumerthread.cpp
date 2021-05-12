/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QSignalSpy>
#include <QtPositioning/qgeoareamonitorsource.h>
#include "positionconsumerthread.h"

PositionConsumerThread::PositionConsumerThread(QGeoAreaMonitorSource *source, QObject *parent)
    : QThread(parent), m_source(source)
{
}

PositionConsumerThread::~PositionConsumerThread()
{
    stopProcessing();
    wait();
}

int PositionConsumerThread::detectedEnterCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_detectedEnterCount;
}

int PositionConsumerThread::detectedExitCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_detectedExitCount;
}

void PositionConsumerThread::stopProcessing()
{
    m_mutex.lock();
    m_waitCondition.wakeOne();
    m_mutex.unlock();
}

void PositionConsumerThread::run()
{
    QSignalSpy enterSpy(m_source, &QGeoAreaMonitorSource::areaEntered);
    QSignalSpy exitSpy(m_source, &QGeoAreaMonitorSource::areaExited);

    m_mutex.lock();
    m_waitCondition.wait(&m_mutex);
    m_detectedEnterCount = enterSpy.count();
    m_detectedExitCount = exitSpy.count();
    m_mutex.unlock();
}
