/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEROUTEMAPITEM_H_
#define QDECLARATIVEROUTEMAPITEM_H_

#include "qdeclarativegeomapitembase_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomap_p.h"
#include <QPen>
#include <QBrush>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoRoute;
class PolylineMapPaintedItem;

class QDeclarativeRouteMapItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoRoute* route READ route WRITE setRoute NOTIFY routeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QDeclarativeRouteMapItem(QQuickItem *parent = 0);
    ~QDeclarativeRouteMapItem();

    QDeclarativeGeoRoute* route() const;
    void setRoute(QDeclarativeGeoRoute *route);

    QColor color() const;
    void setColor(const QColor &color);

    void dragStarted();
    bool contains(QPointF point);

Q_SIGNALS:
    void routeChanged(const QDeclarativeGeoRoute *route);
    void colorChanged(const QColor &color);

protected:
    void updateContent();
    QPointF contentTopLeftPoint();
    void mapChanged();

private Q_SLOTS:
    // map size changed
    void handleCameraDataChanged(const CameraData& cameraData);

private:
    PolylineMapPaintedItem *polylineItem_;
    QDeclarativeGeoRoute* route_;
    QColor color_;
    bool dragActive_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeRouteMapItem));

#endif /* QDECLARATIVEROUTEMAPITEM_H_ */
