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
#include <QtTest/QtTest>
#include <QtTest/private/qpropertytesthelper_p.h>
#include <QtPositioningQuick/private/qdeclarativepositionsource_p.h>

QT_USE_NAMESPACE

class tst_DeclarativePositionSource : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void nameBinding();
    void supportedMethodsBinding();
    void sourceErrorBinding();
    void validBinding();
    void positionBinding();

    void intervalOnSourceDependency();
    void preferredMethodsOnSourceDependency();

    void updateAfterStart();
    void startAfterUpdate();
    void stopAfterUpdate();
    void startStopAfterUpdate();
    void updateTimedOut();
    void updateWithStartTimedOut();
    void startUpdateStopWithNoIntervals();

private:
    std::unique_ptr<QDeclarativePositionSource> m_positionSource = nullptr;
};

void tst_DeclarativePositionSource::init()
{
    // create a fresh instance of QDeclarativePositionSource before each test
    m_positionSource.reset(new QDeclarativePositionSource);
    m_positionSource->componentComplete(); // simulate QML loading
}

void tst_DeclarativePositionSource::nameBinding()
{
    m_positionSource->setName("test.source");
    QTestPrivate::testReadWritePropertyBasics<QDeclarativePositionSource, QString>(
            *m_positionSource.get(), "invalid source", "test.source", "name");
}

void tst_DeclarativePositionSource::supportedMethodsBinding()
{
    // Invalid source has no positioning methods.
    // "test.source" has all positioning methods.
    m_positionSource->setName("invalid name");
    QTestPrivate::testReadOnlyPropertyBasics<QDeclarativePositionSource,
                                             QDeclarativePositionSource::PositioningMethods>(
            *m_positionSource.get(), QDeclarativePositionSource::NoPositioningMethods,
            QDeclarativePositionSource::AllPositioningMethods, "supportedPositioningMethods",
            [&]() { m_positionSource->setName("test.source"); });
}

void tst_DeclarativePositionSource::sourceErrorBinding()
{
    // "dummy.source" has a minimum update interval of 100, and calling
    // update() with a smaller timeout immediately result in a timeout error
    m_positionSource->setName("dummy.source");
    QTestPrivate::testReadOnlyPropertyBasics<QDeclarativePositionSource,
                                             QDeclarativePositionSource::SourceError>(
            *m_positionSource.get(), QDeclarativePositionSource::NoError,
            QDeclarativePositionSource::UpdateTimeoutError, "sourceError",
            [&]() { m_positionSource->update(10); });
    if (QTest::currentTestFailed())
        return;

    // Test that we can't bind sourceError to smth, as it's read-only
    QProperty<QDeclarativePositionSource::SourceError> errorSetter;
    m_positionSource->bindableSourceError().setBinding(Qt::makePropertyBinding(errorSetter));
    QCOMPARE(m_positionSource->bindableSourceError().hasBinding(), false);
}

void tst_DeclarativePositionSource::validBinding()
{
    // Invalid source name results in no position source -> invalid object.
    // Setting the name to "test.source" results in creating a new position
    // source -> valid object
    m_positionSource->setName("invalid name");
    QTestPrivate::testReadOnlyPropertyBasics<QDeclarativePositionSource, bool>(
            *m_positionSource.get(), false, true, "valid",
            [&]() { m_positionSource->setName("test.source"); });
}

static char *printPosition(const QDeclarativePosition *position)
{
    // For this test we need to print only coordinate, so that we get a nice
    // error message if the below test fails.
    QString str;
    QDebug dbg(&str);
    dbg << position->coordinate();
    const auto dataArray = str.toLatin1();
    const auto msgSize = dataArray.size() + 1;
    char *msg = new char[msgSize];
    memset(msg, 0, msgSize);
    qsnprintf(msg, msgSize, "%s", dataArray.constData());
    return msg;
}

void tst_DeclarativePositionSource::positionBinding()
{
    // "test.source" udpates its position, starting from (0, 0) coordinate, and
    // adding 0.1 lat and 0.1 log at every step
    m_positionSource->setName("test.source");
    m_positionSource->setUpdateInterval(1000);
    const QGeoCoordinate c1(0, 0);
    const QGeoCoordinate c2(0.1, 0.1, 0);
    QCOMPARE(m_positionSource->position()->coordinate(), c1);

    QGeoPositionInfo posInfo;
    posInfo.setCoordinate(c1);
    QDeclarativePosition pos1;
    pos1.setPosition(posInfo);

    posInfo.setCoordinate(c2);
    QDeclarativePosition pos2;
    pos2.setPosition(posInfo);

    QTestPrivate::testReadOnlyPropertyBasics<QDeclarativePositionSource, QDeclarativePosition *>(
            *m_positionSource.get(), &pos1, &pos2, "position",
            [&]() {
                m_positionSource->update();
                QTest::qWait(1500); // the update will happen after 1000 ms
            },
            [](const QDeclarativePosition *p1, const QDeclarativePosition *p2) {
                return p1->coordinate() == p2->coordinate();
            },
            printPosition);
    if (QTest::currentTestFailed())
        return;

    QProperty<QDeclarativePosition *> prop(nullptr);
    m_positionSource->bindablePosition().setBinding(Qt::makePropertyBinding(prop));
    // We can't have bindings on read-only properties
    QCOMPARE(m_positionSource->bindablePosition().hasBinding(), false);
}

void tst_DeclarativePositionSource::intervalOnSourceDependency()
{
    // updateInterval can be modified if the new source does not support that
    // large one, or implements some calculation
    m_positionSource->setName("invalid_source"); // reset the source

    QSignalSpy intervalSpy(m_positionSource.get(),
                           &QDeclarativePositionSource::updateIntervalChanged);

    m_positionSource->setUpdateInterval(200);
    QCOMPARE(m_positionSource->updateInterval(), 200);
    QCOMPARE(intervalSpy.count(), 1);

    // "test.source" has a minimum update interval of 1000
    m_positionSource->setName("test.source");
    QCOMPARE(m_positionSource->updateInterval(), 1000);
    QCOMPARE(intervalSpy.count(), 2);

    // "dummy.source" has a minimum update interval of 100, so we expect our
    // desired interval to be applied
    m_positionSource->setName("dummy.source");
    QCOMPARE(m_positionSource->updateInterval(), 200);
    QCOMPARE(intervalSpy.count(), 3);
}

void tst_DeclarativePositionSource::preferredMethodsOnSourceDependency()
{
    m_positionSource->setName("invalid_source"); // reset the source

    QSignalSpy methodsSpy(m_positionSource.get(),
                          &QDeclarativePositionSource::preferredPositioningMethodsChanged);

    m_positionSource->setPreferredPositioningMethods(
            QDeclarativePositionSource::SatellitePositioningMethods);
    QCOMPARE(m_positionSource->preferredPositioningMethods(),
             QDeclarativePositionSource::SatellitePositioningMethods);
    QCOMPARE(methodsSpy.count(), 1);

    // "dummy.source" has only non-satellite methods, so they will be used
    m_positionSource->setName("dummy.source");
    QCOMPARE(m_positionSource->preferredPositioningMethods(),
             QDeclarativePositionSource::NonSatellitePositioningMethods);
    QCOMPARE(methodsSpy.count(), 2);

    // "test.source" has all positioning methods, so satellite will be used,
    // as we initially wanted
    m_positionSource->setName("test.source");
    QCOMPARE(m_positionSource->preferredPositioningMethods(),
             QDeclarativePositionSource::SatellitePositioningMethods);
    QCOMPARE(methodsSpy.count(), 3);
}

void tst_DeclarativePositionSource::updateAfterStart()
{
    // When update() is called after start(), it should not invalidate any
    // state. The active state must still be true when the single update() is
    // completed.

    m_positionSource->setName("test.source");

    QCOMPARE(m_positionSource->isActive(), false);

    m_positionSource->start();
    QCOMPARE(m_positionSource->isActive(), true);

    m_positionSource->update(1100);
    QTest::qWait(1500); // wait for the single update to complete
    QCOMPARE(m_positionSource->isActive(), true);

    m_positionSource->stop();
    QCOMPARE(m_positionSource->isActive(), false);
}

void tst_DeclarativePositionSource::startAfterUpdate()
{
    // When start() is called after update(), the position source should remain
    // active even when the signle update is completed.

    m_positionSource->setName("test.source");

    QCOMPARE(m_positionSource->isActive(), false);

    m_positionSource->update(1100);
    QCOMPARE(m_positionSource->isActive(), true);

    m_positionSource->start();
    QTest::qWait(1500); // wait for the single update to complete
    QCOMPARE(m_positionSource->isActive(), true);

    m_positionSource->stop();
    QCOMPARE(m_positionSource->isActive(), false);
}

void tst_DeclarativePositionSource::stopAfterUpdate()
{
    // When stop() is called after update(), and the update() is still in
    // progress, the position source should remain active until the update()
    // is completed.

    m_positionSource->setName("test.source");

    QCOMPARE(m_positionSource->isActive(), false);

    m_positionSource->update(1100);
    QCOMPARE(m_positionSource->isActive(), true);

    m_positionSource->stop();
    QCOMPARE(m_positionSource->isActive(), true);

    QTest::qWait(1500); // wait for the single update to complete
    QCOMPARE(m_positionSource->isActive(), false);
}

void tst_DeclarativePositionSource::startStopAfterUpdate()
{
    // Quite artificial example. Calling start() and stop() after update(),
    // while still waiting for the update() to complete, should still result in
    // the position source to be active until the update() is completed.

    m_positionSource->setName("test.source");

    QCOMPARE(m_positionSource->isActive(), false);

    m_positionSource->update(1100);
    QCOMPARE(m_positionSource->isActive(), true);

    m_positionSource->start();
    m_positionSource->stop();

    QCOMPARE(m_positionSource->isActive(), true);

    QTest::qWait(1500); // wait for the single update to complete
    QCOMPARE(m_positionSource->isActive(), false);
}

void tst_DeclarativePositionSource::updateTimedOut()
{
    // This test checks that we reset to inactive state when the single update()
    // request times out without providing the position info

    m_positionSource->setName("test.source");

    QCOMPARE(m_positionSource->isActive(), false);

    m_positionSource->update(200); // to small timeout -> will return an error
    QCOMPARE(m_positionSource->isActive(), true);

    QTRY_COMPARE_WITH_TIMEOUT(m_positionSource->sourceError(),
                              QDeclarativePositionSource::UpdateTimeoutError, 300);
    QCOMPARE(m_positionSource->isActive(), false);
}

void tst_DeclarativePositionSource::updateWithStartTimedOut()
{
    // This test checks that if single update() times out, but the regular
    // updates are running, we still remain in active state.

    m_positionSource->setName("test.source");

    QCOMPARE(m_positionSource->isActive(), false);

    m_positionSource->start();

    m_positionSource->update(200); // to small timeout -> will return an error
    QCOMPARE(m_positionSource->isActive(), true);

    QTRY_COMPARE_WITH_TIMEOUT(m_positionSource->sourceError(),
                              QDeclarativePositionSource::UpdateTimeoutError, 300);
    QCOMPARE(m_positionSource->isActive(), true);

    m_positionSource->stop();
    QCOMPARE(m_positionSource->isActive(), false);
}

void tst_DeclarativePositionSource::startUpdateStopWithNoIntervals()
{
    // This test checks that a sequence of calls start() -> update() -> stop()
    // without any waits between them will result in expected behavior.
    // Specifically, the position source should remain active until it gets
    // the position response.

    m_positionSource->setName("test.source");

    QCOMPARE(m_positionSource->isActive(), false);

    m_positionSource->start();
    m_positionSource->update(1100);
    QCOMPARE(m_positionSource->isActive(), true);
    m_positionSource->stop();
    QCOMPARE(m_positionSource->isActive(), true);

    QTest::qWait(1500); // wait for the single update to complete
    QCOMPARE(m_positionSource->isActive(), false);
}

QTEST_MAIN(tst_DeclarativePositionSource)

#include "tst_qdeclarativepositionsource.moc"
