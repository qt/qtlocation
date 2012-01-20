/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
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

#ifndef TST_QGEOROUTE_H
#define TST_QGEOROUTE_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QMetaType>

#include <qgeoroute.h>
#include <qgeoboundingbox.h>
#include <qgeocoordinate.h>
#include <qgeorouterequest.h>
#include <qgeoroutesegment.h>


QT_USE_NAMESPACE

class tst_QGeoRoute : public QObject
{
    Q_OBJECT

public:
    tst_QGeoRoute();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start unit test for QGeoRoute
    void constructor();
    void copy_constructor();
    void destructor();
    void bounds();
    void distance();
    void path();
    void path_data();
    void request();
    void routeId();
    void firstrouteSegments();
    void travelMode();
    void travelMode_data();
    void travelTime();
    void operators();
    //End Unit Test for QGeoRoute

private:
    QGeoRoute *qgeoroute;
    QGeoBoundingBox *qgeoboundingbox;
    QGeoCoordinate *qgeocoordinate;
    QGeoRouteRequest *qgeorouterequest;
    QGeoRouteSegment *qgeoroutesegment;


};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QList<QGeoCoordinate>);
Q_DECLARE_METATYPE( QGeoRouteRequest::TravelMode);


#endif // TST_QGEOROUTE_H
