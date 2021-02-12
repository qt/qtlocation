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

#include <QTest>
#include <QTimer>
#include <QtPositioning/QNmeaSatelliteInfoSource>
#include "../../qgeosatelliteinfosource/testqgeosatelliteinfosource_p.h"
#include "../../utils/qnmeaproxyfactory.h"
#include "../../utils/qlocationtestutils_p.h"

class Feeder : public QObject
{
    Q_OBJECT
public:
    Feeder(QObject *parent) : QObject(parent)
    {
    }

    void start(QNmeaSatelliteInfoSourceProxy *proxy)
    {
        m_proxy = proxy;
        QTimer *timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, this, &Feeder::timeout);
        timer->setInterval(proxy->source()->minimumUpdateInterval() * 2);
        timer->start();
    }

public slots:
    void timeout()
    {
        // Here we need to provide different chunks of data, because the signals
        // are emitted only when data changes.
        if (has_data) {
            m_proxy->feedBytes(QLocationTestUtils::createGsvLongSentence().toLatin1());
            m_proxy->feedBytes(QLocationTestUtils::createGsaLongSentence().toLatin1());
        } else {
            m_proxy->feedBytes(QLocationTestUtils::createGsvSentence().toLatin1());
            m_proxy->feedBytes(QLocationTestUtils::createGsaSentence().toLatin1());
        }
        has_data = !has_data;
    }

private:
    QNmeaSatelliteInfoSourceProxy *m_proxy;
    bool has_data = true;
};

class tst_QNmeaSatelliteInfoSource_Generic_Realtime : public TestQGeoSatelliteInfoSource
{
    Q_OBJECT
protected:
    QGeoSatelliteInfoSource *createTestSource() override
    {
        QNmeaSatelliteInfoSource *source =
                new QNmeaSatelliteInfoSource(QNmeaSatelliteInfoSource::UpdateMode::RealTimeMode);
        QNmeaSatelliteInfoSourceProxy *proxy = m_factory.createSatelliteInfoSourceProxy(source);
        Feeder *feeder = new Feeder(source);
        feeder->start(proxy);
        return source;
    }

private:
    QNmeaProxyFactory m_factory;
};

#include "tst_nmeasatelliteinfosource_generic_rt.moc"

QTEST_GUILESS_MAIN(tst_QNmeaSatelliteInfoSource_Generic_Realtime)
