/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
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

struct Q_LOCATION_PRIVATE_EXPORT QGeoMapViewportChangeEvent
{
    QGeoCameraData cameraData;
    QSizeF mapSize;

    bool zoomLevelChanged = false;
    bool centerChanged = false;
    bool mapSizeChanged = false;
    bool tiltChanged = false;
    bool bearingChanged = false;
    bool rollChanged = false;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapItemBase : public QQuickItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(GeoMapItemBase)
    QML_ADDED_IN_VERSION(5, 0)
    QML_UNCREATABLE("GeoMapItemBase is not intended instantiable by developer.")

    Q_PROPERTY(QGeoShape geoShape READ geoShape WRITE setGeoShape STORED false )
    Q_PROPERTY(bool autoFadeIn READ autoFadeIn WRITE setAutoFadeIn REVISION(5, 14))
    Q_PROPERTY(int lodThreshold READ lodThreshold WRITE setLodThreshold NOTIFY lodThresholdChanged REVISION(5, 15))

public:
    explicit QDeclarativeGeoMapItemBase(QQuickItem *parent = nullptr);
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

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    virtual QSGNode *updateMapItemPaintNode(QSGNode *, UpdatePaintNodeData *);

    inline QGeoMap::ItemType itemType() const { return m_itemType; }
    qreal mapItemOpacity() const;

    void setParentGroup(QDeclarativeGeoMapItemGroup &parentGroup);

    template <typename T = QObject>
    QList<T*> quickChildren() const
    {
        QList<T*> res;
        QObjectList kids = children();
        const QList<QQuickItem *> quickKids = childItems();
        for (const auto &quickKid : quickKids)
            kids.append(quickKid);
        for (auto *kid : qAsConst(kids)) {
            if (auto *val = qobject_cast<T*>(kid))
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
    bool childMouseEventFilter(QQuickItem *item, QEvent *event) override;
    bool isPolishScheduled() const;
    virtual void setMaterialDirty();

    QGeoMap::ItemType m_itemType = QGeoMap::NoItem;

private Q_SLOTS:
    void baseCameraDataChanged(const QGeoCameraData &camera);
    void visibleAreaChanged();

private:
    QPointer<QGeoMap> map_;
    QDeclarativeGeoMap *quickMap_ = nullptr;

    QSizeF lastSize_;
    QGeoCameraData lastCameraData_;

    QDeclarativeGeoMapItemGroup *parentGroup_ = nullptr;

    std::unique_ptr<QDeclarativeGeoMapItemTransitionManager> m_transitionManager;
    bool m_autoFadeIn = true;
    int m_lodThreshold = 0;

    friend class QDeclarativeGeoMap;
    friend class QDeclarativeGeoMapItemView;
    friend class QDeclarativeGeoMapItemTransitionManager;
};

QT_END_NAMESPACE

#endif
