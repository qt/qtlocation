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
#include <QtPositioningQuick/private/qdeclarativepositionsource_p.h>

QT_USE_NAMESPACE

class tst_DeclarativePositionSource : public QObject
{
    Q_OBJECT

private slots:
    void init();

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
