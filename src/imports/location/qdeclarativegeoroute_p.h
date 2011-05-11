/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOROUTE_H
#define QDECLARATIVEGEOROUTE_H

#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoroutesegment_p.h"

#include <qgeoroute.h>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoRoute : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoBoundingBox *bounds READ bounds CONSTANT)
    Q_PROPERTY(int travelTime READ travelTime CONSTANT)
    Q_PROPERTY(qreal distance READ distance CONSTANT)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCoordinate> path READ path)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> segments READ segments)

public:
    QDeclarativeGeoRoute(QObject *parent = 0);
    QDeclarativeGeoRoute(const QGeoRoute &route, QObject *parent = 0);
    ~QDeclarativeGeoRoute();

    QDeclarativeGeoBoundingBox* bounds() const;
    int travelTime() const;
    qreal distance() const;
    QDeclarativeListProperty<QDeclarativeCoordinate> path();
    QDeclarativeListProperty<QDeclarativeGeoRouteSegment> segments();

private:
    static void path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate);
    static int path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index);
    static void path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);

    static void segments_append(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop, QDeclarativeGeoRouteSegment *segment);
    static int segments_count(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop);
    static QDeclarativeGeoRouteSegment* segments_at(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop, int index);
    static void segments_clear(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop);

    QGeoRoute route_;
    QDeclarativeGeoBoundingBox* bounds_;
    QList<QDeclarativeCoordinate*> path_;
    QList<QDeclarativeGeoRouteSegment*> segments_;
};


QTM_END_NAMESPACE

#endif

