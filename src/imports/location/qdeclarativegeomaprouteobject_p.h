/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEGEOMAPROUTEOBJECT_H
#define QDECLARATIVEGEOMAPROUTEOBJECT_H

#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoroute_p.h"
#include "qgeomaprouteobject.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapRouteObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoRoute* route READ route WRITE setRoute NOTIFY routeChanged)
    Q_PROPERTY(quint32 detailLevel READ detailLevel WRITE setDetailLevel NOTIFY detailLevelChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QDeclarativeGeoMapRouteObject(QDeclarativeItem *parent = 0);
    ~QDeclarativeGeoMapRouteObject();

    QDeclarativeGeoRoute* route();
    void setRoute(QDeclarativeGeoRoute* route);

    quint32 detailLevel() const;
    void setDetailLevel(quint32 radius);

    QColor color() const;
    void setColor(const QColor &color);

Q_SIGNALS:
    void routeChanged();
    void detailLevelChanged();
    void colorChanged();

private:
    // The declarative route (datatype)
    QDeclarativeGeoRoute* declarativeRoute_;
    // Used internally as the visual map object
    QGeoMapRouteObject* route_;
    QColor color_;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapRouteObject));

#endif
