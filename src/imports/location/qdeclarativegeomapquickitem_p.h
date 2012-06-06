/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPQUICKITEM_H
#define QDECLARATIVEGEOMAPQUICKITEM_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>

#include "qdeclarativegeomap_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomapitembase_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoMapQuickItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate *coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QPointF anchorPoint READ anchorPoint WRITE setAnchorPoint NOTIFY anchorPointChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QQuickItem *sourceItem READ sourceItem WRITE setSourceItem NOTIFY sourceItemChanged)

public:
    explicit QDeclarativeGeoMapQuickItem(QQuickItem *parent = 0);
    ~QDeclarativeGeoMapQuickItem();

    virtual void setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map);

    void setCoordinate(QDeclarativeCoordinate *coordinate);
    QDeclarativeCoordinate *coordinate();

    void setSourceItem(QQuickItem *sourceItem);
    QQuickItem *sourceItem();

    void setAnchorPoint(const QPointF &anchorPoint);
    QPointF anchorPoint() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void dragEnded();

Q_SIGNALS:
    void coordinateChanged();
    void sourceItemChanged();
    void anchorPointChanged();
    void zoomLevelChanged();

protected Q_SLOTS:
    virtual void updateMapItem();
    virtual void afterChildrenChanged();
    void afterViewportChanged(const QGeoMapViewportChangeEvent &event);

private:
    qreal scaleFactor();
    QDeclarativeCoordinate *coordinate_;
    QDeclarativeCoordinate internalCoordinate_;
    QPointer<QQuickItem> sourceItem_;
    QQuickItem *opacityContainer_;
    QPointF anchorPoint_;
    qreal zoomLevel_;
    bool inUpdate_;
    bool mapAndSourceItemSet_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMapQuickItem));

#endif
