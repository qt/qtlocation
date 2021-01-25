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
****************************************************************************/

#ifndef QGEOMAPROUTE_P_P_H
#define QGEOMAPROUTE_P_P_H

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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeomapobject_p_p.h>
#include <QtLocation/private/qdeclarativegeoroute_p.h>
QT_BEGIN_NAMESPACE

class QGeoRoute;

class Q_LOCATION_PRIVATE_EXPORT QMapRouteObjectPrivate : public QGeoMapObjectPrivate
{
public:
    QMapRouteObjectPrivate(QGeoMapObject *q);
    QMapRouteObjectPrivate(const QMapRouteObjectPrivate &other);
    ~QMapRouteObjectPrivate() override;

    virtual QGeoMapObject::Type type() const override final;

    QDeclarativeGeoRoute *declarativeGeoRoute() const;

    virtual QGeoRoute route() const;
    virtual void setRoute(const QDeclarativeGeoRoute *route);

    // QGeoMapObjectPrivate interface
    bool equals(const QGeoMapObjectPrivate &other) const override;
    QGeoMapObjectPrivate *clone() override;
    virtual QGeoShape geoShape() const override;
    virtual void setGeoShape(const QGeoShape &shape) override;
};

QT_END_NAMESPACE

#endif // QGEOMAPROUTE_P_P_H
