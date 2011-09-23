/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tst_qgeocodingmanager.h"

QT_USE_NAMESPACE


void tst_QGeocodingManager::initTestCase()
{
    tst_QGeocodingManager::loadGeocodingManager();
}

void tst_QGeocodingManager::cleanupTestCase()
{
    delete qgeoserviceprovider;
}

void tst_QGeocodingManager::init()
{
    qRegisterMetaType<QGeocodeReply::Error>("Error");
    qRegisterMetaType<QGeocodeReply*>();

    signalerror = new QSignalSpy(qgeocodingmanager, SIGNAL(error(QGeocodeReply*,QGeocodeReply::Error,QString)));
    signalfinished = new QSignalSpy(qgeocodingmanager, SIGNAL(finished(QGeocodeReply*)));
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );
}

void tst_QGeocodingManager::cleanup()
{
    delete signalerror;
    delete signalfinished;
}

void tst_QGeocodingManager::loadGeocodingManager()
{
    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    QVERIFY(providers.contains("static.geocode.test.plugin"));

    qgeoserviceprovider = new QGeoServiceProvider("static.geocode.test.plugin");
    QVERIFY(qgeoserviceprovider);
    QCOMPARE(qgeoserviceprovider->error(), QGeoServiceProvider::NoError);

    qgeocodingmanager = qgeoserviceprovider->geocodingManager();
    QVERIFY(qgeocodingmanager);
}

void tst_QGeocodingManager::supports()
{
    QVERIFY(qgeocodingmanager->supportsGeocoding());
    QVERIFY(qgeocodingmanager->supportsReverseGeocoding());
}

void tst_QGeocodingManager::locale()
{
    QLocale *german = new QLocale (QLocale::German, QLocale::Germany);
    QLocale *english = new QLocale (QLocale::C, QLocale::AnyCountry);

    //Default Locale from the Search Engine
    QCOMPARE(qgeocodingmanager->locale(),*german);

    qgeocodingmanager->setLocale(*english);

    QCOMPARE(qgeocodingmanager->locale(),*english);

    QVERIFY(qgeocodingmanager->locale() != *german);

    delete german;
    delete english;
}

void tst_QGeocodingManager::name()
{
    QString name = "static.geocode.test.plugin";
    QCOMPARE(qgeocodingmanager->managerName(),name);
}

void tst_QGeocodingManager::version()
{
    int version=3;
    QCOMPARE(qgeocodingmanager->managerVersion(),version);

}

void tst_QGeocodingManager::search()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QString search = "Berlin. Invaliendenstrasse";
    int limit = 10;
    int offset = 2;
    QGeoBoundingBox *bounds = new QGeoBoundingBox ();

    QGeocodeReply * reply = qgeocodingmanager->geocode(search, limit,offset,bounds);

    QCOMPARE(reply->errorString(),search);
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete reply;
    delete bounds;

}

void tst_QGeocodingManager::geocode()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QGeoAddress *address = new QGeoAddress ();
    QString city = "Berlin";
    address->setCity(city);

    QGeoBoundingBox *bounds = new QGeoBoundingBox ();

    QGeocodeReply *reply = qgeocodingmanager->geocode(*address,bounds);

    QCOMPARE(reply->errorString(),city);
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete address;
    delete bounds;
    delete reply;
}

void tst_QGeocodingManager::reverseGeocode()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QGeoCoordinate *coordinate = new QGeoCoordinate (34.34 , 56.65);
    QGeoBoundingBox *bounds = new QGeoBoundingBox ();

    QGeocodeReply *reply = qgeocodingmanager->reverseGeocode(*coordinate,bounds);

    QCOMPARE(reply->errorString(),coordinate->toString());
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete coordinate;
    delete bounds;
    delete reply;


}


QTEST_MAIN(tst_QGeocodingManager)

