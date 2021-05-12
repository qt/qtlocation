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

#ifndef LOGFILEPOSITIONSOURCE_H
#define LOGFILEPOSITIONSOURCE_H

#include <QtPositioning/qgeopositioninfosource.h>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class LogFilePositionSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    // This class is optimized to reduce the file IO.
    // Initially it was reading the file line-by-line.
    // It does not modify the data, so it was optimized to just hold the
    // const reference to the pre-existing data, that can now be read once
    // for all the instances of this class (for example, during the
    // initTestCase() call).
    LogFilePositionSource(const QList<QByteArray> &data, QObject *parent = 0);

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;

    PositioningMethods supportedPositioningMethods() const override;
    int minimumUpdateInterval() const override;
    Error error() const override;

signals:
    void noDataLeft();
    void updatesStarted();
    void updatesStopped();

public slots:
    virtual void startUpdates() override;
    virtual void stopUpdates() override;

    virtual void requestUpdate(int timeout = 5000) override;

private slots:
    void readNextPosition();

private:
    bool canReadLine() const;

    QTimer *timer;
    QGeoPositionInfo lastPosition;
    Error lastError = QGeoPositionInfoSource::NoError;
    const QList<QByteArray> &lines;
    qsizetype index = -1;
    bool noDataEmitted = false;
};

#endif
