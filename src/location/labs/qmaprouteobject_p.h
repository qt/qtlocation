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

#ifndef QDECLARATIVEMAPROUTEDELEGATE_P_H
#define QDECLARATIVEMAPROUTEDELEGATE_P_H

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
#include <QtQml/qqml.h>

#include <QtLocation/private/qgeomapobject_p.h>
#include <QtLocation/private/qparameterizableobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoRoute;
class QGeoRoute;
class QMapRouteObjectPrivate;
class Q_LOCATION_PRIVATE_EXPORT QMapRouteObject : public QGeoMapObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeGeoRoute *route READ route WRITE setRoute NOTIFY routeChanged)

public:
    explicit QMapRouteObject(QObject *parent = nullptr);
    ~QMapRouteObject() override;

    QDeclarativeGeoRoute *route() const;
    QGeoRoute geoRoute() const;

    void setMap(QGeoMap *map) override;
    void setRoute(QDeclarativeGeoRoute * route);

signals:
    void routeChanged(QDeclarativeGeoRoute * route);

protected:
    QDeclarativeGeoRoute *m_route = nullptr;

    friend class QMapRouteObjectPrivate;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QMapRouteObject)

#endif // QDECLARATIVEMAPROUTEDELEGATE_P_H
