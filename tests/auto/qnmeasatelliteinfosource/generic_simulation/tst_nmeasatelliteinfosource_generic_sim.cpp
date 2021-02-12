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
#include <QtPositioning/QNmeaSatelliteInfoSource>
#include "../../qgeosatelliteinfosource/testqgeosatelliteinfosource_p.h"
#include "../../utils/qlocationtestutils_p.h"

class UnlimitedNmeaStream : public QIODevice
{
    Q_OBJECT

public:
    UnlimitedNmeaStream(QObject *parent) : QIODevice(parent) {}

protected:
    qint64 readData(char *data, qint64 maxSize) override
    {
        if (maxSize == 0)
            return 0;
        QByteArray bytes;
        if (genSatInView) {
            increaseSatId();
            bytes = QLocationTestUtils::createGsvVariableSentence(satId).toLatin1();
        } else {
            bytes = QLocationTestUtils::createGsaVariableSentence(satId).toLatin1();
        }
        genSatInView = !genSatInView;
        qint64 sz = qMin(qint64(bytes.size()), maxSize);
        memcpy(data, bytes.constData(), sz);
        return sz;
    }

    qint64 writeData(const char *, qint64) override
    {
        return -1;
    }

    qint64 bytesAvailable() const override
    {
        return 1024 + QIODevice::bytesAvailable();
    }

private:
    void increaseSatId()
    {
        if (++satId == 0)
            satId = 1;
    }

    quint8 satId = 0;
    bool genSatInView = true;
};

class tst_QNmeaSatelliteInfoSource_Generic_Simulation : public TestQGeoSatelliteInfoSource
{
    Q_OBJECT
protected:
    QGeoSatelliteInfoSource *createTestSource() override
    {
        QNmeaSatelliteInfoSource *source =
                new QNmeaSatelliteInfoSource(QNmeaSatelliteInfoSource::UpdateMode::SimulationMode);
        source->setDevice(new UnlimitedNmeaStream(source));
        return source;
    }
};

#include "tst_nmeasatelliteinfosource_generic_sim.moc"

QTEST_GUILESS_MAIN(tst_QNmeaSatelliteInfoSource_Generic_Simulation)
