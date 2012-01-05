/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
****************************************************************************/
#ifndef QGEOPROJECTION_H
#define QGEOPROJECTION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qglobal.h"

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QVector2D;
class QVector3D;

class Q_AUTOTEST_EXPORT QGeoProjection
{
public:
    QGeoProjection();
    virtual ~QGeoProjection();

    virtual QVector3D coordToPoint(const QGeoCoordinate &coord) const = 0;
    virtual QGeoCoordinate pointToCoord(const QVector3D &point) const = 0;

    QVector2D coordToMercator(const QGeoCoordinate &coord) const;
    QGeoCoordinate mercatorToCoord(const QVector2D &mercator) const;

    virtual QVector3D mercatorToPoint(const QVector2D &mercator) const;
    virtual QVector2D pointToMercator(const QVector3D &point) const;

    virtual QGeoCoordinate interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress) = 0;
private:
    static double realmod(const double a, const double b);
};

QT_END_NAMESPACE

#endif // QGEOPROJECTION_H
