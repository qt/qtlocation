/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "tst_qgeocodingmanager.h"

QT_USE_NAMESPACE


void tst_QGeocodingManager::initTestCase()
{
    /*
     * Set custom path since CI doesn't install test plugins
     */
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath()
                                     + QStringLiteral("/../../../plugins"));
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
    QVERIFY(providers.contains("geocode.test.plugin"));

    qgeoserviceprovider = new QGeoServiceProvider("geocode.test.plugin");
    QVERIFY(qgeoserviceprovider);
    QCOMPARE(qgeoserviceprovider->error(), QGeoServiceProvider::NotSupportedError);

    qgeoserviceprovider->setAllowExperimental(true);
    QCOMPARE(qgeoserviceprovider->error(), QGeoServiceProvider::NoError);
    QCOMPARE(qgeoserviceprovider->geocodingFeatures(),
             QGeoServiceProvider::OfflineGeocodingFeature
             | QGeoServiceProvider::ReverseGeocodingFeature);

    qgeocodingmanager = qgeoserviceprovider->geocodingManager();
    QVERIFY(qgeocodingmanager);
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
    QString name = "geocode.test.plugin";
    QCOMPARE(qgeocodingmanager->managerName(),name);
}

void tst_QGeocodingManager::version()
{
    int version=100;
    QCOMPARE(qgeocodingmanager->managerVersion(),version);

}

void tst_QGeocodingManager::search()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QString search = "Berlin. Invaliendenstrasse";
    int limit = 10;
    int offset = 2;

    QGeocodeReply * reply = qgeocodingmanager->geocode(search, limit,offset);

    QCOMPARE(reply->errorString(),search);
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete reply;
}

void tst_QGeocodingManager::geocode()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QGeoAddress *address = new QGeoAddress ();
    QString city = "Berlin";
    address->setCity(city);

    QGeocodeReply *reply = qgeocodingmanager->geocode(*address);

    QCOMPARE(reply->errorString(),city);
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete address;
    delete reply;
}

void tst_QGeocodingManager::reverseGeocode()
{
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QGeoCoordinate *coordinate = new QGeoCoordinate (34.34 , 56.65);

    QGeocodeReply *reply = qgeocodingmanager->reverseGeocode(*coordinate);

    QCOMPARE(reply->errorString(),coordinate->toString());
    QCOMPARE(signalfinished->count(),1);
    QCOMPARE(signalerror->count(),0);

    delete coordinate;
    delete reply;


}


QTEST_GUILESS_MAIN(tst_QGeocodingManager)

