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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeotiledmap_p.h>
#include <QtLocation/private/qgeomappingmanager_p.h>
#include <QtLocation/private/qgeomapcontroller_p.h>
#include <QtLocation/private/qgeocameracapabilities_p.h>


QT_USE_NAMESPACE

class QGeoTiledMapTest: public QGeoTiledMap
{
    Q_OBJECT
public:
    QGeoTiledMapTest(QGeoTiledMappingManagerEngine *engine, QObject *parent = 0)
        : QGeoTiledMap(engine, parent) {}
public:
    using QGeoTiledMap::setCameraData;
};

class tst_QGeoMapController : public QObject
{
    Q_OBJECT

public:
    tst_QGeoMapController();
    ~tst_QGeoMapController();

private:
    void clearSignalSpies();

private Q_SLOTS:
    void initTestCase();
    void signalsConstructedTest();
    void constructorTest_data();
    void constructorTest();
    void centerTest();
    void bearingTest();
    void tiltTest();
    void rollTest();
    void panTest();
    void zoomTest();

private:
    QScopedPointer<QGeoTiledMapTest> m_map;
    QScopedPointer<QSignalSpy> m_signalCenterChanged;
    QScopedPointer<QSignalSpy> m_signalBearingChanged;
    QScopedPointer<QSignalSpy> m_signalTiltChanged;
    QScopedPointer<QSignalSpy> m_signalRollChanged;
    QScopedPointer<QSignalSpy> m_signalZoomChanged;
};

tst_QGeoMapController::tst_QGeoMapController()
{
}

tst_QGeoMapController::~tst_QGeoMapController()
{
}

void tst_QGeoMapController::initTestCase()
{
    // Set custom path since CI doesn't install test plugins
#ifdef Q_OS_WIN
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() +
                                     QStringLiteral("/../../../../plugins"));
#else
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() +
                                     QStringLiteral("/../../../plugins"));
#endif
    QGeoServiceProvider *provider = new QGeoServiceProvider("qmlgeo.test.plugin");
    provider->setAllowExperimental(true);
    QGeoMappingManager *mappingManager = provider->mappingManager();
    QVERIFY2(provider->error() == QGeoServiceProvider::NoError, "Could not load plugin: " + provider->errorString().toLatin1());

    m_map.reset(static_cast<QGeoTiledMapTest*>(mappingManager->createMap(this)));
    QVERIFY(m_map);
    m_map->resize(100, 100);

    m_signalCenterChanged.reset(new QSignalSpy(m_map->mapController(), &QGeoMapController::centerChanged));
    m_signalBearingChanged.reset(new QSignalSpy(m_map->mapController(), &QGeoMapController::bearingChanged));
    m_signalTiltChanged.reset(new QSignalSpy(m_map->mapController(), &QGeoMapController::tiltChanged));
    m_signalRollChanged.reset(new QSignalSpy(m_map->mapController(), &QGeoMapController::rollChanged));
    m_signalZoomChanged.reset(new QSignalSpy(m_map->mapController(), &QGeoMapController::zoomChanged));
}

void tst_QGeoMapController::clearSignalSpies()
{
    m_signalCenterChanged->clear();
    m_signalBearingChanged->clear();
    m_signalTiltChanged->clear();
    m_signalRollChanged->clear();
    m_signalZoomChanged->clear();
}

void tst_QGeoMapController::signalsConstructedTest()
{
    QVERIFY(m_signalCenterChanged->isValid());
    QVERIFY(m_signalBearingChanged->isValid());
    QVERIFY(m_signalTiltChanged->isValid());
    QVERIFY(m_signalRollChanged->isValid());
    QVERIFY(m_signalZoomChanged->isValid());
}

void tst_QGeoMapController::constructorTest_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<double>("bearing");
    QTest::addColumn<double>("tilt");
    QTest::addColumn<double>("roll");
    QTest::addColumn<double>("zoom");
    QTest::newRow("zeros") << QGeoCoordinate() << 0.0 << 0.0 << 0.0 << 0.0;
    QTest::newRow("valid") << QGeoCoordinate(10.0, 20.5, 30.8) << 0.1 << 0.2 << 0.3 << 2.0;
    QTest::newRow("negative values") << QGeoCoordinate(-50, -20, 100) << -0.1 << -0.2 << -0.3 << 1.0;
}

void tst_QGeoMapController::constructorTest()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(double, bearing);
    QFETCH(double, tilt);
    QFETCH(double, roll);
    QFETCH(double, zoom);

    // test whether the map controller picks up the camera data
    // from the map during construction
    QGeoCameraData cameraData;
    cameraData.setCenter(center);
    cameraData.setBearing(bearing);
    cameraData.setTilt(tilt);
    cameraData.setRoll(roll);
    cameraData.setZoomLevel(zoom);
    m_map->setCameraData(cameraData);
    QGeoMapController mapController(m_map.data());

    // make sure the values come out the same
    // also make sure the values match what they were actually set to
    QCOMPARE(mapController.center(), cameraData.center());
    QCOMPARE(mapController.center(), center);
    QCOMPARE(mapController.zoom(), cameraData.zoomLevel());
    QCOMPARE(mapController.zoom(), zoom);

    if (m_map->cameraCapabilities().supportsBearing()){
        QCOMPARE(mapController.bearing(), cameraData.bearing());
        QCOMPARE(mapController.bearing(), bearing);
    }
    if (m_map->cameraCapabilities().supportsTilting()){
        QCOMPARE(mapController.tilt(), cameraData.tilt());
        QCOMPARE(mapController.tilt(), tilt);
    }
    if (m_map->cameraCapabilities().supportsRolling()){
        QCOMPARE(mapController.roll(), cameraData.roll());
        QCOMPARE(mapController.roll(), roll);
    }
}

void tst_QGeoMapController::centerTest()
{
    QGeoCameraData cameraData;
    cameraData.setCenter(QGeoCoordinate(10.0,-20.4,30.8));
    m_map->setCameraData(cameraData);
    QGeoMapController mapController(m_map.data());
    QCOMPARE(mapController.center(),QGeoCoordinate(10.0,-20.4,30.8));

    QGeoCoordinate coord(10.0,20.4,30.8);
    clearSignalSpies();
    mapController.setCenter(coord);

    // check correct signal is triggered
    QCOMPARE(m_signalCenterChanged->count(),1);
    QCOMPARE(m_signalBearingChanged->count(),0);
    QCOMPARE(m_signalTiltChanged->count(),0);
    QCOMPARE(m_signalRollChanged->count(),0);
    QCOMPARE(m_signalZoomChanged->count(),0);

    QCOMPARE(mapController.center(),QGeoCoordinate(10.0,20.4,30.8));

    mapController.setCenter(QGeoCoordinate(10.0,20.4,30.9));
    QCOMPARE(mapController.center(),QGeoCoordinate(10.0,20.4,30.9));
}

void tst_QGeoMapController::bearingTest()
{
    if (m_map->cameraCapabilities().supportsBearing()){
        qreal bearing = 1.4;

        QGeoCameraData cameraData;
        cameraData.setBearing(bearing);
        m_map->setCameraData(cameraData);
        QGeoMapController mapController(m_map.data());
        QCOMPARE(mapController.bearing(),bearing);

        clearSignalSpies();
        mapController.setBearing(-1.5);
        QCOMPARE(mapController.bearing(),-1.5);

        // check correct signal is triggered
        QCOMPARE(m_signalCenterChanged->count(),0);
        QCOMPARE(m_signalBearingChanged->count(),1);
        QCOMPARE(m_signalTiltChanged->count(),0);
        QCOMPARE(m_signalRollChanged->count(),0);
        QCOMPARE(m_signalZoomChanged->count(),0);
    }
}

void tst_QGeoMapController::tiltTest()
{
    if (m_map->cameraCapabilities().supportsTilting()){
        qreal tilt = m_map->cameraCapabilities().maximumTilt();

        QGeoCameraData cameraData;
        cameraData.setTilt(tilt);
        m_map->setCameraData(cameraData);
        QGeoMapController mapController(m_map.data());
        QCOMPARE(mapController.tilt(),tilt);

        tilt = m_map->cameraCapabilities().minimumTilt();
        clearSignalSpies();
        mapController.setTilt(tilt);
        QCOMPARE(mapController.tilt(),tilt);

        // check correct signal is triggered
        QCOMPARE(m_signalCenterChanged->count(),0);
        QCOMPARE(m_signalBearingChanged->count(),0);
        QCOMPARE(m_signalTiltChanged->count(),1);
        QCOMPARE(m_signalRollChanged->count(),0);
        QCOMPARE(m_signalZoomChanged->count(),0);
    }
}

void tst_QGeoMapController::rollTest()
{
    if (m_map->cameraCapabilities().supportsRolling()){
        qreal roll = 1.4;

        QGeoCameraData cameraData;
        cameraData.setRoll(roll);
        m_map->setCameraData(cameraData);
        QGeoMapController mapController(m_map.data());
        QCOMPARE(mapController.roll(),roll);

        clearSignalSpies();
        mapController.setRoll(-1.5);
        QCOMPARE(mapController.roll(),-1.5);

        // check correct signal is triggered
        QCOMPARE(m_signalCenterChanged->count(),0);
        QCOMPARE(m_signalBearingChanged->count(),0);
        QCOMPARE(m_signalTiltChanged->count(),0);
        QCOMPARE(m_signalRollChanged->count(),1);
        QCOMPARE(m_signalZoomChanged->count(),0);
    }
}

void tst_QGeoMapController::panTest()
{
    QGeoMapController mapController(m_map.data());

    mapController.setCenter(QGeoCoordinate(-1.0,-2.4,3.8));

    // check that pan of zero leaves the camera centre unaltered
    mapController.pan(0, 0);
    QCOMPARE(mapController.center().altitude(), 3.8);
    QCOMPARE(mapController.center().latitude(), -1.0);
    QCOMPARE(mapController.center().longitude(), -2.4);

    qreal dx = 13.1;
    qreal dy = -9.3;
    clearSignalSpies();
    mapController.pan(dx, dy);

    // rather than verify the exact new position, we check that the position has changed and the altitude
    // is unaffected
    QCOMPARE(mapController.center().altitude(), 3.8);
    QVERIFY(qFuzzyCompare(mapController.center().latitude(), -1.0) == false);
    QVERIFY(qFuzzyCompare(mapController.center().longitude(), -2.4) == false);

    // check correct signal is triggered
    QCOMPARE(m_signalCenterChanged->count(),1);
    QCOMPARE(m_signalBearingChanged->count(),0);
    QCOMPARE(m_signalTiltChanged->count(),0);
    QCOMPARE(m_signalRollChanged->count(),0);
    QCOMPARE(m_signalZoomChanged->count(),0);
}

void tst_QGeoMapController::zoomTest()
{
    QGeoCameraData cameraData;
    cameraData.setZoomLevel(1.4);
    m_map->setCameraData(cameraData);
    QGeoMapController mapController(m_map.data());

    QCOMPARE(mapController.zoom(),1.4);
    mapController.setZoom(1.4);
    QCOMPARE(mapController.zoom(),1.4);
    clearSignalSpies();
    mapController.setZoom(1.5);
    QCOMPARE(mapController.zoom(),1.5);

    // check correct signal is triggered
    QCOMPARE(m_signalCenterChanged->count(),0);
    QCOMPARE(m_signalBearingChanged->count(),0);
    QCOMPARE(m_signalTiltChanged->count(),0);
    QCOMPARE(m_signalRollChanged->count(),0);
    QCOMPARE(m_signalZoomChanged->count(),1);
}


QTEST_MAIN(tst_QGeoMapController)

#include "tst_qgeomapcontroller.moc"
