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
#include <QSignalSpy>
#include <QtPositioning/QNmeaSatelliteInfoSource>
#include "../../utils/qnmeaproxyfactory.h"

class DummyNmeaSatelliteInfoSource : public QNmeaSatelliteInfoSource
{
    Q_OBJECT

public:
    DummyNmeaSatelliteInfoSource(QObject *parent = 0);

protected:
    QGeoSatelliteInfo::SatelliteSystem parseSatellitesInUseFromNmea(const char *data, int size,
                                                                    QList<int> &pnrsInUse) override;
    SatelliteInfoParseStatus parseSatelliteInfoFromNmea(const char *data, int size,
                                                        QList<QGeoSatelliteInfo> &infos,
                                                        QGeoSatelliteInfo::SatelliteSystem &system) override;
};

DummyNmeaSatelliteInfoSource::DummyNmeaSatelliteInfoSource(QObject *parent)
    : QNmeaSatelliteInfoSource(QNmeaSatelliteInfoSource::UpdateMode::RealTimeMode, parent)
{
}

QGeoSatelliteInfo::SatelliteSystem
DummyNmeaSatelliteInfoSource::parseSatellitesInUseFromNmea(const char *data, int size,
                                                           QList<int> &pnrsInUse)
{
    // expected format: "USE:num1;num2;num3\n"
    // example: "USE:1;3;4;7\n"
    if (!data || !size)
        return QGeoSatelliteInfo::Undefined;

    QString str = QLatin1String(data, size).toString();
    if (!str.startsWith("USE:"))
        return QGeoSatelliteInfo::Undefined;

    const QStringList sl = str.mid(4).split(";", Qt::SkipEmptyParts);

    if (sl.empty())
        return QGeoSatelliteInfo::Undefined;

    for (const auto &str : sl) {
        bool ok = false;
        int value = str.toInt(&ok);
        if (ok) {
            pnrsInUse.push_back(value);
        }
    }
    return QGeoSatelliteInfo::GPS;
}

QNmeaSatelliteInfoSource::SatelliteInfoParseStatus
DummyNmeaSatelliteInfoSource::parseSatelliteInfoFromNmea(const char *data, int size,
                                                         QList<QGeoSatelliteInfo> &infos,
                                                         QGeoSatelliteInfo::SatelliteSystem &system)
{
    // expected format: "INFO:system,identifier;system,identifier;system,identifier\n"
    // example: "INFO:1,5;1,7;1,15\n"
    if (!data || !size)
        return NotParsed;

    QString str = QLatin1String(data, size).toString();
    if (!str.startsWith("INFO:"))
        return NotParsed;

    QStringList sat_infos = str.mid(5).split(";", Qt::SkipEmptyParts);

    if (sat_infos.empty())
        return NotParsed;

    for (const auto &sat_info : sat_infos) {
        QStringList parameters = sat_info.split(",", Qt::SkipEmptyParts);
        if (parameters.size() == 2) {
            QGeoSatelliteInfo info;
            info.setSatelliteSystem(
                    static_cast<QGeoSatelliteInfo::SatelliteSystem>(parameters[0].toInt()));
            info.setSatelliteIdentifier(parameters[1].toInt());
            infos.push_back(info);
        }
    }

    system = infos.isEmpty() ? QGeoSatelliteInfo::Undefined : infos.front().satelliteSystem();

    return FullyParsed;
}

class tst_DummyNmeaSatelliteInfoSource : public QObject
{
    Q_OBJECT

private slots:
    void testOverloadedParseFunction();
};

void tst_DummyNmeaSatelliteInfoSource::testOverloadedParseFunction()
{
    DummyNmeaSatelliteInfoSource source;
    QNmeaProxyFactory factory;
    QScopedPointer<QNmeaSatelliteInfoSourceProxy> proxy(
            factory.createSatelliteInfoSourceProxy(&source));

    QSignalSpy inUseSpy(proxy->source(), &QNmeaSatelliteInfoSource::satellitesInUseUpdated);
    QSignalSpy inViewSpy(proxy->source(), &QNmeaSatelliteInfoSource::satellitesInViewUpdated);

    proxy->source()->startUpdates();

    // first we need to send all satellites
    proxy->feedBytes("INFO:1,5;1,7;1,15\n");
    // then - used ones
    proxy->feedBytes("USE:5;15\n");

    QTRY_VERIFY_WITH_TIMEOUT(inUseSpy.count() == 1, 10000);
    QTRY_VERIFY_WITH_TIMEOUT(inViewSpy.count() == 1, 10000);

    QGeoSatelliteInfo info_1_5;
    info_1_5.setSatelliteSystem(QGeoSatelliteInfo::GPS);
    info_1_5.setSatelliteIdentifier(5);

    QGeoSatelliteInfo info_1_7;
    info_1_7.setSatelliteSystem(QGeoSatelliteInfo::GPS);
    info_1_7.setSatelliteIdentifier(7);

    QGeoSatelliteInfo info_1_15;
    info_1_15.setSatelliteSystem(QGeoSatelliteInfo::GPS);
    info_1_15.setSatelliteIdentifier(15);

    const QList<QGeoSatelliteInfo> desiredInView = { info_1_5, info_1_7, info_1_15 };
    const QList<QGeoSatelliteInfo> desiredInUse = { info_1_5, info_1_15 };

    const QList<QGeoSatelliteInfo> inViewList =
            inViewSpy.at(0).at(0).value<QList<QGeoSatelliteInfo>>();
    const QList<QGeoSatelliteInfo> inUseList =
            inUseSpy.at(0).at(0).value<QList<QGeoSatelliteInfo>>();

    QCOMPARE(inViewList, desiredInView);
    QCOMPARE(inUseList, desiredInUse);
}

#include "tst_dummynmeasatelliteinfosource.moc"

QTEST_GUILESS_MAIN(tst_DummyNmeaSatelliteInfoSource);
