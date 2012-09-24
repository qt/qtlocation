/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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

//TESTED_COMPONENT=src/location

#include <QTest>
#include <QMetaType>
#include <QSignalSpy>

#include <limits.h>
#include <float.h>

#include <qgeoareamonitor.h>
#include <qgeopositioninfo.h>
#include <qgeopositioninfosource.h>


QT_USE_NAMESPACE
#define MINIMUM_TIMEOUT 5000

Q_DECLARE_METATYPE(QGeoPositionInfo)


class MyPositionAreaMonitor : public QGeoAreaMonitor
{
    Q_OBJECT
public:
    MyPositionAreaMonitor(QObject* parent = 0) : QGeoAreaMonitor(parent) {}
    ~MyPositionAreaMonitor() {}
};


static qreal tst_qgeoareamonitor_minimumRadius()
{
   // tests should not be run with minimum radius in this case
    return 0;
}


class tst_QGeoAreaMonitor : public QObject
{
    Q_OBJECT

private:
    QGeoAreaMonitor* monitor;


private slots:
    void initTestCase()
    {
        qRegisterMetaType<QGeoPositionInfo>();
    }

    void init()
    {
        monitor = 0;
    }

    void cleanup()
    {
        delete monitor;
        monitor = 0;
    }

    void cleanupTestCase()
    {
    }

    void coordinate()
    {
        MyPositionAreaMonitor mon;
        QCOMPARE(mon.center(), QGeoCoordinate());
    }

    void radius()
    {
        MyPositionAreaMonitor mon;
        qFuzzyCompare(mon.radius(), qreal(0.0));
    }

    void constructor_withoutParent()
    {
        MyPositionAreaMonitor *myMonitor = new MyPositionAreaMonitor();
        delete myMonitor;
    }

    void constructor_withParent()
    {
        QObject* parent = new QObject;
        new MyPositionAreaMonitor(parent);
        delete parent;
    }

    void createDefaultMonitor()
    {
        QObject* parent = new QObject;
        QGeoAreaMonitor* obj = QGeoAreaMonitor::createDefaultMonitor(parent);
        QVERIFY(obj != 0);
        delete parent;
    }

    //TC_ID_4_x_1
    void center_createDefaultMonitor()
    {
        monitor = QGeoAreaMonitor::createDefaultMonitor(0);
        QVERIFY(monitor != 0);
        QVERIFY(monitor->center().isValid() == false);
    }

    //TC_ID_4_x_1
    void radius_createDefaultMonitor()
    {
        monitor = QGeoAreaMonitor::createDefaultMonitor(0);
        QVERIFY(monitor != 0);
        qFuzzyCompare(monitor->radius(), qreal(0.0));
    }

    void setCenter()
    {
        QFETCH(QGeoCoordinate, coord);
        QFETCH(bool, validity);

        monitor = QGeoAreaMonitor::createDefaultMonitor(0);
        QVERIFY(monitor != 0);

        monitor->setCenter(coord);
        QCOMPARE(monitor->center().isValid(), validity);
        if (validity)
            QCOMPARE(monitor->center(), coord);
    }

    void setCenter_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");
        QTest::addColumn<bool>("validity");

        QTest::newRow("valid coordinate") << QGeoCoordinate(34.56, -12.4, 56) << true;   //TC_ID_4_x_1
        QTest::newRow("invalid coordinate") << QGeoCoordinate(173.2, -195.8) << false;      //TC_ID_4_x_2
        QTest::newRow("uninitialised coordinate") << QGeoCoordinate() << false;                   //TC_ID_4_x_3
    }

    void setCenter_twice()
    {
        QFETCH(QGeoCoordinate, first_coord);
        QFETCH(QGeoCoordinate, second_coord);
        QFETCH(QGeoCoordinate, expected_coord);

        monitor = QGeoAreaMonitor::createDefaultMonitor(0);
        QVERIFY(monitor != 0);

        monitor->setCenter(first_coord);
        monitor->setCenter(second_coord);
        QCOMPARE(monitor->center(), expected_coord);
    }

    void setCenter_twice_data()
    {
        QTest::addColumn<QGeoCoordinate>("first_coord");
        QTest::addColumn<QGeoCoordinate>("second_coord");
        QTest::addColumn<QGeoCoordinate>("expected_coord");

        //TC_ID_4_x_4
        QTest::newRow("setting first valid then invalid coordinate")
                << QGeoCoordinate(34.56, -12.4, 56) << QGeoCoordinate(173.2, -195.8)
                << QGeoCoordinate(34.56, -12.4, 56);
        //TC_ID_4_x_5
        QTest::newRow("setting first invalid then valid coordinate")
                << QGeoCoordinate(173.2, -195.8) << QGeoCoordinate(34.56, -12.4, 56)
                << QGeoCoordinate(34.56, -12.4, 56);
        //TC_ID_4_x_6
        QTest::newRow("setting first valid then valid coordinate")
                << QGeoCoordinate(-12.56, 101.4, 56) << QGeoCoordinate(34.56, -12.4, 56)
                << QGeoCoordinate(34.56, -12.4, 56);
    }

    void setRadius()
    {
        QFETCH(qreal, radius);
        QFETCH(qreal, expected_radius);

        monitor = QGeoAreaMonitor::createDefaultMonitor(0);
        QVERIFY(monitor != 0);

        monitor->setRadius(radius);
        qFuzzyCompare(monitor->radius(), expected_radius);
    }

    void setRadius_data()
    {
        QTest::addColumn<qreal>("radius");
        QTest::addColumn<qreal>("expected_radius");

        QTest::newRow("zero radius") << qreal(0.0) << tst_qgeoareamonitor_minimumRadius();             //TC_ID_4_x_1
        QTest::newRow("radius ok") << qreal(1000.58) << qreal(1000.58);             //TC_ID_4_x_2
        QTest::newRow("negative radius") << qreal(-876.58) << tst_qgeoareamonitor_minimumRadius();      //TC_ID_4_x_3
        QTest::newRow("float min") << qreal(FLT_MIN) << tst_qgeoareamonitor_minimumRadius();      //TC_ID_4
        QTest::newRow("float max") << qreal(FLT_MAX) << qreal(FLT_MAX);             //TC_ID_4_x_4_x_5
    }

    //TC_ID_4_x_6
    void setRadius_twice()
    {
        monitor = QGeoAreaMonitor::createDefaultMonitor(0);
        QVERIFY(monitor != 0);

        monitor->setRadius(qreal(1235.985));
        monitor->setRadius(qreal(6754.075));

        qFuzzyCompare(monitor->radius(), qreal(6754.075f));
    }
};


QTEST_GUILESS_MAIN(tst_QGeoAreaMonitor)
#include "tst_qgeoareamonitor.moc"
