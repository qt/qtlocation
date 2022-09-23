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

#ifndef QDECLARATIVEGEOMAPQUICKITEM_H
#define QDECLARATIVEGEOMAPQUICKITEM_H

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
#include <QtQuick/QSGNode>

#include <QtLocation/private/qdeclarativegeomap_p.h>
#include <QtLocation/private/qdeclarativegeomapitembase_p.h>
#include <QtPositioning/qgeoshape.h>

QT_BEGIN_NAMESPACE

class QMapQuickItemMatrix4x4 : public QQuickTransform
{
public:
    QMapQuickItemMatrix4x4(QObject *parent = nullptr);

    void setMatrix(const QMatrix4x4& matrix);
    void applyTo(QMatrix4x4 *matrix) const override;

    QMatrix4x4 m_matrix;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapQuickItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MapQuickItem)
    QML_ADDED_IN_VERSION(5, 0)
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QPointF anchorPoint READ anchorPoint WRITE setAnchorPoint NOTIFY anchorPointChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QQuickItem *sourceItem READ sourceItem WRITE setSourceItem NOTIFY sourceItemChanged)

public:
    explicit QDeclarativeGeoMapQuickItem(QQuickItem *parent = nullptr);
    ~QDeclarativeGeoMapQuickItem();

    void setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map) override;

    void setCoordinate(const QGeoCoordinate &coordinate);
    QGeoCoordinate coordinate();

    void setSourceItem(QQuickItem *sourceItem);
    QQuickItem *sourceItem();

    void setAnchorPoint(const QPointF &anchorPoint);
    QPointF anchorPoint() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    const QGeoShape &geoShape() const override;
    void setGeoShape(const QGeoShape &shape) override;

Q_SIGNALS:
    void coordinateChanged();
    void sourceItemChanged();
    void anchorPointChanged();
    void zoomLevelChanged();

protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void updatePolish() override;
    bool childMouseEventFilter(QQuickItem *item, QEvent *event) override;

protected Q_SLOTS:
    void afterChildrenChanged() override;
    void afterViewportChanged(const QGeoMapViewportChangeEvent &event) override;

private:
    qreal scaleFactor();
    QGeoCoordinate dragStartCoordinate_;
    QGeoCoordinate coordinate_;
    QGeoRectangle geoshape_;
    QPointer<QQuickItem> sourceItem_;
    QQuickItem *opacityContainer_ = nullptr;
    QPointF anchorPoint_;
    qreal zoomLevel_ = 0.0;
    bool mapAndSourceItemSet_ = false;
    bool updatingGeometry_ = false;
    QMapQuickItemMatrix4x4 *matrix_ = nullptr;

    friend class QDeclarativeGeoMap;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGeoMapQuickItem)

#endif
