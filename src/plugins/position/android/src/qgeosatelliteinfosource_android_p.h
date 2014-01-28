/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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


#ifndef QGEOSATELLITEINFOSOURCEANDROID_H
#define QGEOSATELLITEINFOSOURCEANDROID_H

#include <QGeoSatelliteInfoSource>
#include <QTimer>

class QGeoSatelliteInfoSourceAndroid : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    explicit QGeoSatelliteInfoSourceAndroid(QObject *parent = 0);
    ~QGeoSatelliteInfoSourceAndroid();

    //From QGeoSatelliteInfoSource
    void setUpdateInterval(int msec);
    int minimumUpdateInterval() const;

    Error error() const;

public Q_SLOTS:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 0);

    void processSatelliteUpdateInView(const QList<QGeoSatelliteInfo> &satsInView, bool isSingleUpdate);
    void processSatelliteUpdateInUse(const QList<QGeoSatelliteInfo> &satsInUse, bool isSingleUpdate);

    void locationProviderDisabled();
private Q_SLOTS:
    void requestTimeout();

private:
    void reconfigureRunningSystem();

    Error m_error;
    int androidClassKeyForUpdate;
    int androidClassKeyForSingleRequest;
    bool updatesRunning;

    QTimer requestTimer;
    QList<QGeoSatelliteInfo> m_satsInUse;
    QList<QGeoSatelliteInfo> m_satsInView;

};

#endif // QGEOSATELLITEINFOSOURCEANDROID_H
