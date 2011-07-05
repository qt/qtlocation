/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "tst_qgeosearchmanager.h"

QTM_USE_NAMESPACE


void tst_QGeoSearchManager::initTestCase()
{
    tst_QGeoSearchManager::loadSearchManager();
}

void tst_QGeoSearchManager::cleanupTestCase()
{
    delete qgeoserviceprovider;
}

void tst_QGeoSearchManager::init()
{
    qRegisterMetaType<QGeoSearchReply::Error>("Error");
    qRegisterMetaType<QGeoSearchReply*>();

    signalerror = new QSignalSpy(qgeosearchmanager, SIGNAL(error(QGeoSearchReply*,QGeoSearchReply::Error,QString)));
    signalfinished = new QSignalSpy(qgeosearchmanager, SIGNAL(finished(QGeoSearchReply*)));
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );
}

void tst_QGeoSearchManager::cleanup()
{
    delete signalerror;
    delete signalfinished;
}

void tst_QGeoSearchManager::loadSearchManager()
{
    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    QVERIFY(providers.contains("static.geosearch.test.plugin"));

    qgeoserviceprovider = new QGeoServiceProvider("static.geosearch.test.plugin");
    QVERIFY(qgeoserviceprovider);
    QCOMPARE(qgeoserviceprovider->error(), QGeoServiceProvider::NoError);

    qgeosearchmanager = qgeoserviceprovider->searchManager();
    QVERIFY(qgeosearchmanager);
}

void tst_QGeoSearchManager::supports()
{
    QVERIFY(qgeosearchmanager->supportsGeocoding());
    QVERIFY(qgeosearchmanager->supportsReverseGeocoding());
    QCOMPARE(qgeosearchmanager->supportedSearchTypes(),QGeoSearchManager::SearchGeocode);
}

void tst_QGeoSearchManager::landmarkManager()
{
    QVERIFY(qgeosearchmanager->defaultLandmarkManager());

    QString managerName;
#ifdef Q_OS_SYMBIAN
    managerName = "com.nokia.qt.landmarks.engines.symbian";
#elif defined(Q_WS_MAEMO_6) || defined(Q_WS_MEEGO)
    managerName = "com.nokia.qt.landmarks.engines.qsparql";
#else
    managerName = "com.nokia.qt.landmarks.engines.sqlite";
#endif

    QCOMPARE(qgeosearchmanager->defaultLandmarkManager()->managerName(),managerName);
}

void tst_QGeoSearchManager::locale()
{
    QLocale *german = new QLocale (QLocale::German, QLocale::Germany);
    QLocale *english = new QLocale (QLocale::C, QLocale::AnyCountry);

    //Default Locale from the Search Engine
    QCOMPARE(qgeosearchmanager->locale(),*german);

    qgeosearchmanager->setLocale(*english);

    QCOMPARE(qgeosearchmanager->locale(),*english);

    QVERIFY(qgeosearchmanager->locale() != *german);

    delete german;
    delete english;
}

void tst_QGeoSearchManager::name()
{
    QString name = "static.geosearch.test.plugin";
    QCOMPARE(qgeosearchmanager->managerName(),name);
}

void tst_QGeoSearchManager::version()
{
    int version=3;
    QCOMPARE(qgeosearchmanager->managerVersion(),version);

}

void tst_QGeoSearchManager::search()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QFETCH(QGeoSearchManager::SearchType,type);

    QString search = "Berlin. Invaliendenstrasse";
    int limit = 10;
    int offset = 2;
    QGeoBoundingBox *bounds = new QGeoBoundingBox ();

    QGeoSearchReply * reply = qgeosearchmanager->search(search,type,limit,offset,bounds);

    QCOMPARE(reply->errorString(),search);
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete reply;
    delete bounds;

}
void tst_QGeoSearchManager::search_data()
{
    QTest::addColumn<QGeoSearchManager::SearchType>("type");

    QTest::newRow("type1") << QGeoSearchManager::SearchAll;
    QTest::newRow("type2") << QGeoSearchManager::SearchGeocode;
    QTest::newRow("type3") << QGeoSearchManager::SearchLandmarks;
    QTest::newRow("type4") << QGeoSearchManager::SearchNone;
}

void tst_QGeoSearchManager::geocode()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QGeoAddress *address = new QGeoAddress ();
    QString city = "Berlin";
    address->setCity(city);

    QGeoBoundingBox *bounds = new QGeoBoundingBox ();

    QGeoSearchReply *reply = qgeosearchmanager->geocode(*address,bounds);

    QCOMPARE(reply->errorString(),city);
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete address;
    delete bounds;
    delete reply;
}

void tst_QGeoSearchManager::reverseGeocode()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QGeoCoordinate *coordinate = new QGeoCoordinate (34.34 , 56.65);
    QGeoBoundingBox *bounds = new QGeoBoundingBox ();

    QGeoSearchReply *reply = qgeosearchmanager->reverseGeocode(*coordinate,bounds);

    QCOMPARE(reply->errorString(),coordinate->toString());
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete coordinate;
    delete bounds;
    delete reply;


}


QTEST_MAIN(tst_QGeoSearchManager)

