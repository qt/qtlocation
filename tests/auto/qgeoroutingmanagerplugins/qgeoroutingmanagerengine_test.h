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

#ifndef QGEOROUTINGMANAGERENGINE_TEST_H
#define QGEOROUTINGMANAGERENGINE_TEST_H

#include <qgeoserviceprovider.h>
#include <qgeoroutingmanagerengine.h>
#include <qgeocoordinate.h>
#include <qgeoroutereply.h>
#include <qgeorouterequest.h>

QT_USE_NAMESPACE

class QGeoRoutingManagerEngineTest: public QGeoRoutingManagerEngine

{
Q_OBJECT
public:
    QGeoRoutingManagerEngineTest(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) :
        QGeoRoutingManagerEngine(parameters)
    {
        Q_UNUSED(error)
        Q_UNUSED(errorString)
        setSupportsRouteUpdates(true);
        setSupportsAlternativeRoutes(true);
        setSupportsExcludeAreas(true);
        setSupportedTravelModes(QGeoRouteRequest::PedestrianTravel);
        setSupportedFeatureTypes(QGeoRouteRequest::TollFeature);
        setSupportedFeatureWeights(QGeoRouteRequest::PreferFeatureWeight);
        setSupportedRouteOptimizations(QGeoRouteRequest::FastestRoute);
        setSupportedSegmentDetails(QGeoRouteRequest::BasicSegmentData);
        setSupportedManeuverDetails(QGeoRouteRequest::BasicManeuvers);
    }

    QGeoRouteReply* calculateRoute(const QGeoRouteRequest& request)
    {
        Q_UNUSED(request);
        return new QGeoRouteReply(QGeoRouteReply::NoError,"no error");
    }

    QGeoRouteReply* updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
    {
        Q_UNUSED(route);
        Q_UNUSED(position);
        return new QGeoRouteReply(QGeoRouteReply::CommunicationError,"no error");

    }


};

#endif
