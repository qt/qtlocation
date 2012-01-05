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

#include <QString>
#include <QtTest/QtTest>

#include <qgeoserviceprovider.h>
#include <qplacemanager.h>

QT_USE_NAMESPACE

class tst_QPlaceManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void compatiblePlace();
    void testMetadata();
    void testLocales();

private:
    QGeoServiceProvider *provider;
    QPlaceManager *placeManager;
};

void tst_QPlaceManager::initTestCase()
{
    provider = 0;

    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    QVERIFY(providers.contains("qmlgeo.test.plugin"));

    provider = new QGeoServiceProvider("qmlgeo.test.plugin");
    placeManager = provider->placeManager();
    QVERIFY(placeManager);
}

void tst_QPlaceManager::testMetadata()
{
    QCOMPARE(placeManager->managerName(), QLatin1String("qmlgeo.test.plugin"));
    QCOMPARE(placeManager->managerVersion(), 3);
}

void tst_QPlaceManager::testLocales()
{
    QCOMPARE(placeManager->locales().count(), 1);
    QCOMPARE(placeManager->locales().at(0), QLocale());

    QLocale locale(QLocale::Norwegian, QLocale::Norway);
    placeManager->setLocale(locale);

    QCOMPARE(placeManager->locales().at(0), locale);

    QList<QLocale> locales;
    QLocale en_AU = QLocale(QLocale::English, QLocale::Australia);
    QLocale en_UK = QLocale(QLocale::English, QLocale::UnitedKingdom);
    locales << en_AU << en_UK;
    placeManager->setLocales(locales);
    QCOMPARE(placeManager->locales().count(), 2);
    QCOMPARE(placeManager->locales().at(0), en_AU);
    QCOMPARE(placeManager->locales().at(1), en_UK);
}

void tst_QPlaceManager::compatiblePlace()
{
    QPlace place;
    place.setPlaceId(QLatin1String("4-8-15-16-23-42"));
    place.setName(QLatin1String("Island"));
    place.setVisibility(QtLocation::PublicVisibility);

    QPlace compatPlace = placeManager->compatiblePlace(place);
    QVERIFY(compatPlace.placeId().isEmpty());
    QCOMPARE(compatPlace.name(), QLatin1String("Island"));
    QCOMPARE(compatPlace.visibility(), QtLocation::UnspecifiedVisibility);
}

void tst_QPlaceManager::cleanupTestCase()
{
    delete provider;
}

QTEST_GUILESS_MAIN(tst_QPlaceManager)

#include "tst_qplacemanager.moc"
