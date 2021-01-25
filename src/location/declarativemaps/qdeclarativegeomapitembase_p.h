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

#ifndef QDECLARATIVEGEOMAPITEMBASE_H
#define QDECLARATIVEGEOMAPITEMBASE_H

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

#include <QtQuick/QQuickItem>
#include <QtPositioning/QGeoShape>

#include <QtLocation/private/qdeclarativegeomap_p.h>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeomap_p.h>
#include <QtLocation/private/qdeclarativegeomapitemtransitionmanager_p.h>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QGeoMapViewportChangeEvent
{
public:
    explicit QGeoMapViewportChangeEvent();
    QGeoMapViewportChangeEvent(const QGeoMapViewportChangeEvent &other);
    QGeoMapViewportChangeEvent &operator=(const QGeoMapViewportChangeEvent &other);

    QGeoCameraData cameraData;
    QSizeF mapSize;

    bool zoomLevelChanged;
    bool centerChanged;
    bool mapSizeChanged;
    bool tiltChanged;
    bool bearingChanged;
    bool rollChanged;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapItemBase : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QGeoShape geoShape READ geoShape WRITE setGeoShape STORED false )
    Q_PROPERTY(bool autoFadeIn READ autoFadeIn WRITE setAutoFadeIn REVISION 14)
    Q_PROPERTY(int lodThreshold READ lodThreshold WRITE setLodThreshold NOTIFY lodThresholdChanged REVISION 15)

public:
    explicit QDeclarativeGeoMapItemBase(QQuickItem *parent = 0);
    virtual ~QDeclarativeGeoMapItemBase();

    virtual void setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map);
    virtual void setPositionOnMap(const QGeoCoordinate &coordinate, const QPointF &offset);

    QDeclarativeGeoMap *quickMap() const { return quickMap_; }
    QGeoMap *map() const { return map_; }
    virtual const QGeoShape &geoShape() const = 0;
    virtual void setGeoShape(const QGeoShape &shape) = 0;

    bool autoFadeIn() const;
    void setAutoFadeIn(bool fadeIn);

    int lodThreshold() const;
    void setLodThreshold(int lt);
    unsigned int zoomForLOD(int zoom) const;

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual QSGNode *updateMapItemPaintNode(QSGNode *, UpdatePaintNodeData *);

    QGeoMap::ItemType itemType() const;
    qreal mapItemOpacity() const;

    void setParentGroup(QDeclarativeGeoMapItemGroup &parentGroup);

    template <typename T = QObject>

    QList<T*> quickChildren() const
    {
        QList<T*> res;
        QObjectList kids = children();
        QList<QQuickItem *> quickKids = childItems();
        for (int i = 0; i < quickKids.count(); ++i)
            kids.append(quickKids.at(i));
        for (auto kid : qAsConst(kids)) {
            auto val = qobject_cast<T*>(kid);
            if (val)
                res.push_back(val);
        }
        return res;
    }

Q_SIGNALS:
    void mapItemOpacityChanged();
    Q_REVISION(12) void addTransitionFinished();
    Q_REVISION(12) void removeTransitionFinished();
    void lodThresholdChanged();

protected Q_SLOTS:
    virtual void afterChildrenChanged();
    virtual void afterViewportChanged(const QGeoMapViewportChangeEvent &event) = 0;
    void polishAndUpdate();

protected:
    float zoomLevelOpacity() const;
    bool childMouseEventFilter(QQuickItem *item, QEvent *event);
    bool isPolishScheduled() const;
    virtual void setMaterialDirty();

    QGeoMap::ItemType m_itemType = QGeoMap::NoItem;

private Q_SLOTS:
    void baseCameraDataChanged(const QGeoCameraData &camera);
    void visibleAreaChanged();

private:
    QPointer<QGeoMap> map_;
    QDeclarativeGeoMap *quickMap_;

    QSizeF lastSize_;
    QGeoCameraData lastCameraData_;

    QDeclarativeGeoMapItemGroup *parentGroup_;

    QScopedPointer<QDeclarativeGeoMapItemTransitionManager> m_transitionManager;
    bool m_autoFadeIn = true;
    int m_lodThreshold = 0;

    friend class QDeclarativeGeoMap;
    friend class QDeclarativeGeoMapItemView;
    friend class QDeclarativeGeoMapItemTransitionManager;
};

QT_END_NAMESPACE

#endif
