/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/
#ifndef QWEBMERCATOR_P_H
#define QWEBMERCATOR_P_H

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

#include <qglobal.h>
#include <QtCore/qvariant.h>
#include "qpositioningglobal_p.h"

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QDoubleVector2D;

class Q_POSITIONING_PRIVATE_EXPORT QWebMercator
{
public:
    static QDoubleVector2D coordToMercator(const QGeoCoordinate &coord);
    static QGeoCoordinate mercatorToCoord(const QDoubleVector2D &mercator);
    static QGeoCoordinate coordinateInterpolation(const QGeoCoordinate &from, const QGeoCoordinate &to, qreal progress);

private:
    static double realmod(const double a, const double b);
};

QT_END_NAMESPACE

#endif // QWEBMERCATOR_P_H
