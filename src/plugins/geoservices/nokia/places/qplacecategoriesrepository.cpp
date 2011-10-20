/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacecategoriesrepository.h"
#include "qplacejsoncategoriesparser.h"

#include "qplacerestmanager.h"

QT_USE_NAMESPACE

static QMap<QString, QString> categoriesMap;
static QPlaceCategory *secondSearchCenter = NULL;

QPlaceCategoriesRepository *QPlaceCategoriesRepository::repositoryInstance = NULL;

QPlaceCategoriesRepository *QPlaceCategoriesRepository::instance()
{
    if (!repositoryInstance) {
        repositoryInstance = new QPlaceCategoriesRepository();
    }
    return repositoryInstance;
}

QPlaceCategoriesRepository::QPlaceCategoriesRepository(QObject *parent)
:   QObject(parent)
{
    setupCategoriesMapper();
}

QPlaceCategoriesRepository::~QPlaceCategoriesRepository()
{
}

QPlaceReply *QPlaceCategoriesRepository::initializeCategories()
{
    if (m_categoriesReply)
        return m_categoriesReply.data();

    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendCategoriesTreeRequest();
    if (restReply) {
        m_categoriesReply = new QPlaceCategoriesReplyImpl(restReply);
        connect(m_categoriesReply.data(), SIGNAL(finished()),
                this, SLOT(replyFinished()));
    }

    return m_categoriesReply.data();
}

QPlaceCategoryTree QPlaceCategoriesRepository::categories() const
{
    return m_categoryTree;
}

QPlaceCategory QPlaceCategoriesRepository::mapCategory(const QString &number)
{
    QString categoryId = categoriesMap.value(number, "");
    if (categoryId == "second-search-center") {
        if (!secondSearchCenter) {
            secondSearchCenter = new QPlaceCategory();
            secondSearchCenter->setCategoryId("second-search-center");
        }
        return *secondSearchCenter;
    } else if (!categoryId.isEmpty()) {
        return findCategoryById(categoryId);
    }
    return QPlaceCategory();
}

QString QPlaceCategoriesRepository::getCategoryTagId(const QPlaceCategory &category)
{
    return categoriesMap.key(category.categoryId(), QString());
}

QPlaceCategory QPlaceCategoriesRepository::findCategoryById(const QString &id)
{
    return m_categoryTree.value(id).category;
}

void QPlaceCategoriesRepository::replyFinished()
{
    if (!m_categoriesReply)
        return;

    m_categoryTree = m_categoriesReply.data()->categories();

    m_categoriesReply.clear();
}

void QPlaceCategoriesRepository::setupCategoriesMapper()
{
    // setup mapper
    if (!categoriesMap.count()) {
        //BusinessServices
        categoriesMap.insert(QString("9000269"),QString("business-services"));
        categoriesMap.insert(QString("9000005"),QString("business-industry"));
        categoriesMap.insert(QString("9000277"),QString("travel-agency"));
        categoriesMap.insert(QString("9000028"),QString("tourist-information"));
        categoriesMap.insert(QString("9000040"),QString("police-emergency"));
        categoriesMap.insert(QString("9000017"),QString("petrol-station"));
        categoriesMap.insert(QString("9000278"),QString("communication-media"));
        categoriesMap.insert(QString("9000047"),QString("atm-bank-exchange"));
        categoriesMap.insert(QString("9000002"),QString("car-dealer-repair"));
        categoriesMap.insert(QString("9000020"),QString("car-rental"));
        categoriesMap.insert(QString("9000215"),QString("service"));
        categoriesMap.insert(QString("9000019"),QString("post-office"));
        // Transport
        categoriesMap.insert(QString("9000272"),QString("transport"));
        categoriesMap.insert(QString("9000216"),QString("taxi-stand"));
        categoriesMap.insert(QString("9000043"),QString("airport"));
        categoriesMap.insert(QString("9000041"),QString("railway-station"));
        categoriesMap.insert(QString("9000058"),QString("public-transport"));
        categoriesMap.insert(QString("9000035"),QString("ferry-terminal"));
        // Shopping
        categoriesMap.insert(QString("9000270"),QString("shopping"));
        categoriesMap.insert(QString("9000205"),QString("clothing-accessories-shop"));
        categoriesMap.insert(QString("9000276"),QString("bookshop"));
        categoriesMap.insert(QString("9000194"),QString("hardware-house-garden-shop"));
        categoriesMap.insert(QString("9000049"),QString("pharmacy"));
        categoriesMap.insert(QString("9000024"),QString("mall"));
        categoriesMap.insert(QString("9000197"),QString("kiosk-convenience-store"));
        categoriesMap.insert(QString("9000196"),QString("electronics-shop"));
        categoriesMap.insert(QString("9000191"),QString("sport-outdoor-shop"));
        categoriesMap.insert(QString("9000189"),QString("department-store"));
        // Accomodation
        categoriesMap.insert(QString("9000271"),QString("accommodation"));
        categoriesMap.insert(QString("9000032"),QString("camping"));
        categoriesMap.insert(QString("9000174"),QString("motel"));
        categoriesMap.insert(QString("9000038"),QString("hotel"));
        categoriesMap.insert(QString("9000173"),QString("hostel"));
        // GoingOut
        categoriesMap.insert(QString("9000274"),QString("going-out"));
        categoriesMap.insert(QString("9000203"),QString("dance-night-club"));
        categoriesMap.insert(QString("9000003"),QString("casino"));
        categoriesMap.insert(QString("9000004"),QString("cinema"));
        categoriesMap.insert(QString("9000181"),QString("theatre-music-culture"));
        // Facility
        categoriesMap.insert(QString("9000261"),QString("facilities"));
        categoriesMap.insert(QString("9000039"),QString("parking-facility"));
        categoriesMap.insert(QString("9000007"),QString("fair-convention-facility"));
        categoriesMap.insert(QString("9000012"),QString("government-community-facility"));
        categoriesMap.insert(QString("9000200"),QString("hospital-health-care-facility"));
        categoriesMap.insert(QString("9000106"),QString("education-facility"));
        categoriesMap.insert(QString("9000220"),QString("sports-facility-venue"));
        categoriesMap.insert(QString("9000031"),QString("library"));
        // NatureGeography
        categoriesMap.insert(QString("9000265"),QString("natural-geographical"));
        categoriesMap.insert(QString("9000262"),QString("body-of-water"));
        categoriesMap.insert(QString("9000045"),QString("mountain-hill"));
        categoriesMap.insert(QString("9000259"),QString("forest-health-vegetation"));
        categoriesMap.insert(QString("9000260"),QString("undersea-feature"));
        // AreasInfrastructure
        categoriesMap.insert(QString("9000266"),QString("administrative-areas-buildings"));
        categoriesMap.insert(QString("9000279"),QString("administrative-region"));
        categoriesMap.insert(QString("9000263"),QString("outdoor-area-complex"));
        categoriesMap.insert(QString("9000283"),QString("city-town-village"));
        categoriesMap.insert(QString("9000280"),QString("building"));
        // EatDrink
        categoriesMap.insert(QString("9000275"),QString("eat-drink"));
        categoriesMap.insert(QString("9000063"),QString("coffee-tea"));
        categoriesMap.insert(QString("9000022"),QString("restaurant"));
        categoriesMap.insert(QString("9000064"),QString("snacks-fast-food"));
        categoriesMap.insert(QString("9000033"),QString("bar-pub"));
        categoriesMap.insert(QString("9000143"),QString("food-drink"));
        // Leisure
        categoriesMap.insert(QString("9000267"),QString("leisure-outdoor"));
        categoriesMap.insert(QString("9000048"),QString("recreation"));
        categoriesMap.insert(QString("9000001"),QString("amusement-holiday-park"));
        // SightsMuseums
        categoriesMap.insert(QString("9000273"),QString("sights-museums"));
        categoriesMap.insert(QString("9000014"),QString("museum"));
        categoriesMap.insert(QString("9000158"),QString("religious-place"));
        categoriesMap.insert(QString("9000211"),QString("landmark-attraction"));
        // SSC
        categoriesMap.insert(QString("9000282"),QString("second-search-center"));
    }
}
