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

#ifndef QGEOSHAPE_P_H
#define QGEOSHAPE_P_H

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

#include <QtCore/QSharedData>

#include "qgeorectangle.h"

QT_BEGIN_NAMESPACE

class QGeoShapePrivate : public QSharedData
{
public:
    explicit QGeoShapePrivate(QGeoShape::ShapeType type);
    virtual ~QGeoShapePrivate();

    virtual bool isValid() const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool contains(const QGeoCoordinate &coordinate) const = 0;

    virtual QGeoCoordinate center() const = 0;

    virtual QGeoRectangle boundingGeoRectangle() const = 0;

    virtual void extendShape(const QGeoCoordinate &coordinate) = 0;

    virtual QGeoShapePrivate *clone() const = 0;

    virtual bool operator==(const QGeoShapePrivate &other) const;

    QGeoShape::ShapeType type;
};

// don't use the copy constructor when detaching from a QSharedDataPointer, use virtual clone()
// call instead.
template <>
Q_INLINE_TEMPLATE QGeoShapePrivate *QSharedDataPointer<QGeoShapePrivate>::clone()
{
    return d->clone();
}

QT_END_NAMESPACE

#endif
