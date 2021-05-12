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

#ifndef POSITIONCONSUMERTHREAD_H
#define POSITIONCONSUMERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

QT_BEGIN_NAMESPACE
class QGeoAreaMonitorSource;
QT_END_NAMESPACE

// This class is created to test the behavior of QGeoAreaMonitorPolling class,
// that reimplements the connectNotify() and disconnetNotify() methods, by
// triggering these methods from multiple threads.
// The thread creates two QSignalSpy instances, that connect to the signals of
// QGeoAreaMonitorSource. Once constructed, they trigger connectNotify().
// Once destroyed, they trigger disconnectNotify.
// With the previous implementation of these overridden methods, that could lead
// to a deadlock in a rare case.
class PositionConsumerThread : public QThread
{
    Q_OBJECT
public:
    explicit PositionConsumerThread(QGeoAreaMonitorSource *source, QObject *parent = nullptr);
    ~PositionConsumerThread();

    int detectedEnterCount() const;
    int detectedExitCount() const;

public slots:
    void stopProcessing();

protected:
    void run() override;

private:
    QGeoAreaMonitorSource *m_source;

    int m_detectedEnterCount = 0;
    int m_detectedExitCount = 0;

    mutable QMutex m_mutex;
    QWaitCondition m_waitCondition;
};

#endif // POSITIONCONSUMERTHREAD_H
