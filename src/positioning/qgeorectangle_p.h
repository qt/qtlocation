/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#ifndef QGEORECTANGLE_P_H
#define QGEORECTANGLE_P_H

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

#include "qgeoshape_p.h"
#include "qgeocoordinate.h"

QT_BEGIN_NAMESPACE

class QGeoRectanglePrivate : public QGeoShapePrivate
{
public:
    QGeoRectanglePrivate();
    QGeoRectanglePrivate(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
    QGeoRectanglePrivate(const QGeoRectanglePrivate &other);
    ~QGeoRectanglePrivate();

    bool isValid() const override;
    bool isEmpty() const override;
    bool contains(const QGeoCoordinate &coordinate) const override;

    QGeoCoordinate center() const override;

    QGeoRectangle boundingGeoRectangle() const override;

    void extendShape(const QGeoCoordinate &coordinate) override;

    QGeoShapePrivate *clone() const override;

    bool operator==(const QGeoShapePrivate &other) const override;

    QGeoCoordinate topLeft;
    QGeoCoordinate bottomRight;
};

QT_END_NAMESPACE

#endif
