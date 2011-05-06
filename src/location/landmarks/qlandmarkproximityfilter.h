/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QLANDMARKPROXIMITY_H
#define QLANDMARKPROXIMITY_H

#include "qlandmarkfilter.h"
#include "qgeocoordinate.h"
#include "qgeoboundingcircle.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLandmarkProximityFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkProximityFilter : public QLandmarkFilter
{
public:
    QLandmarkProximityFilter(const QGeoCoordinate &center = QGeoCoordinate(), qreal radius = -1);
    QLandmarkProximityFilter(const QGeoBoundingCircle &circle);
    QLandmarkProximityFilter(const QLandmarkFilter &other);
    virtual ~QLandmarkProximityFilter();

    QGeoCoordinate center() const;
    void setCenter(const QGeoCoordinate &center);

    qreal radius() const;
    void setRadius(qreal radius);

    QGeoBoundingCircle boundingCircle();
    void setBoundingCircle(const QGeoBoundingCircle &circle);

private:
    Q_DECLARE_LANDMARKFILTER_PRIVATE(QLandmarkProximityFilter)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
