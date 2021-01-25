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

#ifndef QDECLARATIVEROUTEMAPITEM_H_
#define QDECLARATIVEROUTEMAPITEM_H_

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
#include <QtLocation/private/qdeclarativegeomapitembase_p.h>
#include <QtLocation/private/qdeclarativegeomap_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QPen>
#include <QBrush>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoRoute;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeRouteMapItem : public QDeclarativePolylineMapItem
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoRoute *route READ route WRITE setRoute NOTIFY routeChanged)

public:
    explicit QDeclarativeRouteMapItem(QQuickItem *parent = 0);
    ~QDeclarativeRouteMapItem();

    QDeclarativeGeoRoute *route() const;
    void setRoute(QDeclarativeGeoRoute *route);

Q_SIGNALS:
    void routeChanged(const QDeclarativeGeoRoute *route);

private slots:
    void updateRoutePath();

protected:
    void setPath(const QJSValue &value) override;

private:
    QDeclarativeGeoRoute *route_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeRouteMapItem)

#endif /* QDECLARATIVEROUTEMAPITEM_H_ */
