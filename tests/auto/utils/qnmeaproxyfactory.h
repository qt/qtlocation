/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#ifndef QNMEAPROXYFACTORY_H
#define QNMEAPROXYFACTORY_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
class QIODevice;
class QNmeaPositionInfoSource;
class QNmeaSatelliteInfoSource;
class QGeoPositionInfoSource;
class QGeoSatelliteInfoSource;
QT_END_NAMESPACE

class QNmeaPositionInfoSourceProxy : public QObject
{
    Q_OBJECT
public:
    QNmeaPositionInfoSourceProxy(QNmeaPositionInfoSource *source, QIODevice *outDevice);
    ~QNmeaPositionInfoSourceProxy();

    QGeoPositionInfoSource *source() const;

    void feedUpdate(const QDateTime &dt);

    void feedBytes(const QByteArray &bytes);

    int updateIntervalErrorMargin() const { return 50; }

private:
    QNmeaPositionInfoSource *m_source;
    QIODevice *m_outDevice;
};

class QNmeaSatelliteInfoSourceProxy : public QObject
{
    Q_OBJECT
public:
    QNmeaSatelliteInfoSourceProxy(QNmeaSatelliteInfoSource *source, QIODevice *outDevice);
    ~QNmeaSatelliteInfoSourceProxy();

    QGeoSatelliteInfoSource *source() const;

    void feedBytes(const QByteArray &bytes);

private:
    QNmeaSatelliteInfoSource *m_source;
    QIODevice *m_outDevice;
};

class QNmeaProxyFactory : public QObject
{
    Q_OBJECT
public:
    QNmeaProxyFactory();

    // proxy is created as child of source
    QNmeaPositionInfoSourceProxy *createPositionInfoSourceProxy(QNmeaPositionInfoSource *source);
    QNmeaSatelliteInfoSourceProxy *createSatelliteInfoSourceProxy(QNmeaSatelliteInfoSource *source);

private:
    QIODevice *createServerConnection(QTcpSocket *client);

    QTcpServer *m_server;
};

#endif
