/****************************************************************************
**
** Copyright (C) 2020 Paolo Angelelli <paolo.angelelli@gmail.com>
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

#ifndef QDECLARATIVECIRCLEMAPITEM_P_P_H
#define QDECLARATIVECIRCLEMAPITEM_P_P_H

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
#include <QtLocation/private/qdeclarativepolygonmapitem_p_p.h>
#include <QtLocation/private/qdeclarativecirclemapitem_p.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QGeoMapCircleGeometry : public QGeoMapPolygonGeometry
{
public:
    QGeoMapCircleGeometry();

    void updateScreenPointsInvert(const QList<QDoubleVector2D> &circlePath, const QGeoMap &map);
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeCircleMapItemPrivate
{
    Q_DISABLE_COPY_MOVE(QDeclarativeCircleMapItemPrivate)
public:
    static const int CircleSamples = 128; // ToDo: make this radius && ZL dependent?

    QDeclarativeCircleMapItemPrivate(QDeclarativeCircleMapItem &circle)
        : m_circle(circle)
    {
    }

    virtual ~QDeclarativeCircleMapItemPrivate();
    virtual void onLinePropertiesChanged() = 0;
    virtual void markSourceDirtyAndUpdate() = 0;
    virtual void onMapSet() = 0;
    virtual void onGeoGeometryChanged() = 0;
    virtual void onItemGeometryChanged() = 0;
    virtual void updatePolish() = 0;
    virtual void afterViewportChanged() = 0;
    virtual QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) = 0;
    virtual bool contains(const QPointF &point) const = 0;

    void updateCirclePath()
    {
        if (!m_circle.map() || m_circle.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
            return;

        const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_circle.map()->geoProjection());
        QList<QGeoCoordinate> path;
        calculatePeripheralPoints(path, m_circle.center(), m_circle.radius(), CircleSamples, m_leftBound);
        m_circlePath.clear();
        for (const QGeoCoordinate &c : path)
            m_circlePath << p.geoToMapProjection(c);
    }

    static bool crossEarthPole(const QGeoCoordinate &center, qreal distance);

    static bool preserveCircleGeometry(QList<QDoubleVector2D> &path, const QGeoCoordinate &center,
                                qreal distance, const QGeoProjectionWebMercator &p);
    static void updateCirclePathForRendering(QList<QDoubleVector2D> &path, const QGeoCoordinate &center,
                                      qreal distance, const QGeoProjectionWebMercator &p);

    static void calculatePeripheralPoints(QList<QGeoCoordinate> &path, const QGeoCoordinate &center,
                                   qreal distance, int steps, QGeoCoordinate &leftBound);

    QDeclarativeCircleMapItem &m_circle;
    QList<QDoubleVector2D> m_circlePath;
    QGeoCoordinate m_leftBound;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeCircleMapItemPrivateCPU: public QDeclarativeCircleMapItemPrivate
{
public:

    QDeclarativeCircleMapItemPrivateCPU(QDeclarativeCircleMapItem &circle)
        : QDeclarativeCircleMapItemPrivate(circle)
    {
    }

    ~QDeclarativeCircleMapItemPrivateCPU() override;

    void onLinePropertiesChanged() override
    {
        // mark dirty just in case we're a width change
        markSourceDirtyAndUpdate();
    }
    void markSourceDirtyAndUpdate() override
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        m_geometry.markSourceDirty();
        m_borderGeometry.markSourceDirty();
        m_circle.polishAndUpdate();
    }
    void onMapSet() override
    {
        updateCirclePath();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryChanged() override
    {
        updateCirclePath();
        markSourceDirtyAndUpdate();
    }
    void onItemGeometryChanged() override
    {
        onGeoGeometryChanged();
    }
    void afterViewportChanged() override
    {
        markSourceDirtyAndUpdate();
    }
    void updatePolish() override;
    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override;
    bool contains(const QPointF &point) const override;

    QGeoMapCircleGeometry m_geometry;
    QGeoMapPolylineGeometry m_borderGeometry;
    MapPolygonNode *m_node = nullptr;
};

QT_END_NAMESPACE

#endif // QDECLARATIVECIRCLEMAPITEM_P_P_H
