/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/location

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qgeocoordinate.h>

QT_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoPositionInfo)

class tst_PositionPlugin : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void availableSources();
    void create();
    void getUpdates();
};

void tst_PositionPlugin::initTestCase()
{
    qRegisterMetaType<QGeoPositionInfo>();
}

void tst_PositionPlugin::availableSources()
{
    QVERIFY(QGeoPositionInfoSource::availableSources().contains("test.source"));
    QVERIFY(QGeoSatelliteInfoSource::availableSources().contains("test.source"));
}

void tst_PositionPlugin::create()
{
    QGeoPositionInfoSource *src = 0;
    src = QGeoPositionInfoSource::createSource("test.source", 0);
    QVERIFY(src != 0);

    QVERIFY(src->minimumUpdateInterval() == 1000);
}

void tst_PositionPlugin::getUpdates()
{
    QGeoPositionInfoSource *src = QGeoPositionInfoSource::createSource("test.source", 0);
    src->setUpdateInterval(1000);

    QSignalSpy spy(src, SIGNAL(positionUpdated(QGeoPositionInfo)));
    src->startUpdates();
    QTest::qWait(1500);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0].size(), 1);

    QGeoPositionInfo info = qvariant_cast<QGeoPositionInfo>(spy[0][0]);
    QCOMPARE(info.coordinate().latitude(), 0.1);
    QCOMPARE(info.coordinate().longitude(), 0.1);
}



QTEST_GUILESS_MAIN(tst_PositionPlugin)
#include "tst_positionplugin.moc"
