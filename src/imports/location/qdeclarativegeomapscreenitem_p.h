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

#ifndef QDECLARATIVEGEOMAPSCREENITEM_H
#define QDECLARATIVEGEOMAPSCREENITEM_H

#include <QtDeclarative/qquickitem.h>
#include <QtCore/QCoreApplication>

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomap_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoMapItemBase : public QQuickItem
{
    Q_OBJECT
public:
    QDeclarativeGeoMapItemBase(QQuickItem *parent = 0);
    ~QDeclarativeGeoMapItemBase();

    void setMap(QDeclarativeGeoMap* quickMap, Map *map);

protected Q_SLOTS:
    virtual void update();

protected:
    QDeclarativeGeoMap* quickMap();
    Map* map();

private:
    QDeclarativeGeoMap* quickMap_;
    Map* map_;
};

class QDeclarativeGeoMapScreenItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QPointF anchorPoint READ anchorPoint WRITE setAnchorPoint NOTIFY anchorPointChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QQuickItem* sourceItem READ sourceItem WRITE setSourceItem NOTIFY sourceItemChanged)

public:
    QDeclarativeGeoMapScreenItem(QQuickItem *parent = 0);
    ~QDeclarativeGeoMapScreenItem();

    void setCoordinate(QDeclarativeCoordinate *coordinate);
    QDeclarativeCoordinate* coordinate();

    void setSourceItem(QQuickItem* sourceItem);
    QQuickItem* sourceItem();

    void setAnchorPoint(const QPointF &anchorPoint);
    QPointF anchorPoint() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

Q_SIGNALS:
    void coordinateChanged();
    void sourceItemChanged();
    void anchorPointChanged();
    void zoomLevelChanged();
    void camerDataChanged(const CameraData &cameraData);

protected Q_SLOTS:
    void update();

private Q_SLOTS:
    void coordinateCoordinateChanged(double);

private:
    QDeclarativeCoordinate* coordinate_;
    QQuickItem* sourceItem_;
    QPointF anchorPoint_;
    qreal zoomLevel_;
    bool inUpdate_;
    bool mapAndSourceItemSet_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMapScreenItem));

#endif
