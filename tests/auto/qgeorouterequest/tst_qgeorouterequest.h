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

#ifndef TST_QGEOROUTEREQUEST_H
#define TST_QGEOROUTEREQUEST_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QMetaType>

#include <qgeocoordinate.h>
#include <qgeoboundingbox.h>
#include <qgeorouterequest.h>


QT_USE_NAMESPACE

class tst_QGeoRouteRequest : public QObject
{
    Q_OBJECT

public:
    tst_QGeoRouteRequest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for QGeoRouteRequest
    void constructor_waypoints();
    void constructor_orig_dest();
    void copy_constructor();
    void destructor();
    void excludeAreas();
    void numberAlternativeRoutes();
    void routeOptimization();
    void routeOptimization_data();
    void segmentDetail();
    void segmentDetail_data();
    void travelModes();
    void travelModes_data();
    void waypoints();
    void waypoints_data();
    void maneuverDetail();
    void maneuverDetail_data();
    void featureWeight();
    void featureWeight_data();
    //End Unit Test for QGeoRouteRequest

private:
    QGeoCoordinate *qgeocoordinate;
    QGeoBoundingBox *qgeoboundingbox;
    QGeoRouteRequest *qgeorouterequest;

};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QGeoRouteRequest::RouteOptimization );
Q_DECLARE_METATYPE( QGeoRouteRequest::SegmentDetail );
Q_DECLARE_METATYPE( QGeoRouteRequest::TravelMode );
Q_DECLARE_METATYPE( QGeoRouteRequest::FeatureType);
Q_DECLARE_METATYPE( QGeoRouteRequest::FeatureWeight);
Q_DECLARE_METATYPE( QGeoRouteRequest::ManeuverDetail );

#endif // TST_QGEOROUTEREQUEST_H

