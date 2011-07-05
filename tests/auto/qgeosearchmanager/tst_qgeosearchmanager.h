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

#ifndef TST_QGEOSEARCHMANAGER_H
#define TST_QGEOSEARCHMANAGER_H

#include <QLocale>
#include <QtTest/QtTest>
#include <QSignalSpy>

#include <qgeoserviceprovider.h>
#include <qgeosearchmanager.h>
#include <qlandmarkmanager.h>
#include <qgeosearchreply.h>
#include <qgeoboundingbox.h>
#include <qgeoaddress.h>
#include <qgeocoordinate.h>


QTM_USE_NAMESPACE

class SubLandmarkManager :public QLandmarkManager
{
    Q_OBJECT
public:
    SubLandmarkManager (QString name):QLandmarkManager(){this->name=name;}
    QString name;
};

class tst_QGeoSearchManager: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void supports();
    void landmarkManager();
    void locale();
    void name();
    void version();
    void search();
    void search_data();
    void geocode();
    void reverseGeocode();

private:
    QGeoServiceProvider *qgeoserviceprovider;
    QGeoSearchManager *qgeosearchmanager;
    QSignalSpy *signalerror;
    QSignalSpy *signalfinished;
    void loadSearchManager();

};
Q_DECLARE_METATYPE( QGeoSearchReply*);
Q_DECLARE_METATYPE( QGeoSearchReply::Error);
Q_DECLARE_METATYPE( QGeoSearchManager::SearchType);

#endif

