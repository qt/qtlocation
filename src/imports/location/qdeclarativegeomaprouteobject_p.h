/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QDECLARATIVEGEOMAPROUTEOBJECT_H
#define QDECLARATIVEGEOMAPROUTEOBJECT_H

#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoroute_p.h"
#include "qdeclarativegeomapobjectborder_p.h"
#include "qgeomaprouteobject.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoMapRouteObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoRoute* route READ route WRITE setRoute NOTIFY routeChanged)
    Q_PROPERTY(quint32 detailLevel READ detailLevel WRITE setDetailLevel NOTIFY detailLevelChanged)
    Q_PROPERTY(QDeclarativeGeoMapObjectBorder* border READ border CONSTANT)

public:
    QDeclarativeGeoMapRouteObject(QSGItem *parent = 0);
    ~QDeclarativeGeoMapRouteObject();

    QDeclarativeGeoRoute* route();
    void setRoute(QDeclarativeGeoRoute* route);

    quint32 detailLevel() const;
    void setDetailLevel(quint32 radius);

    QDeclarativeGeoMapObjectBorder* border();

Q_SIGNALS:
    void routeChanged();
    void detailLevelChanged();
    void colorChanged();

private Q_SLOTS:
    void borderColorChanged(const QColor &color);
    void borderWidthChanged(int width);

private:
    // The declarative route (datatype)
    QDeclarativeGeoRoute* declarativeRoute_;
    // Used internally as the visual map object
    QGeoMapRouteObject* route_;
    QDeclarativeGeoMapObjectBorder border_;
    QColor color_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMapRouteObject));

#endif
