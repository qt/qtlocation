/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOPROJECTION_H
#define QGEOPROJECTION_H

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
#include <QtLocation/private/qgeocameradata_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QGeoProjection
{
public:
    QGeoProjection();
    virtual ~QGeoProjection();

    virtual void setViewportSize(const QSize &size) = 0;
    virtual void setCameraData(const QGeoCameraData &cameraData) = 0;

    // returns the minimum zoom at the current viewport size
    virtual double minimumZoom() const = 0;
    virtual double maximumCenterLatitudeAtZoom(double zoomLevel) const = 0;

    // returns the size of the underlying map, at the current zoom level.
    virtual double mapWidth() const = 0;
    virtual double mapHeight() const = 0;

    // Conversion methods for QGeoCoordinate <-> screen.
    // This currently assumes that the "MapProjection" space is [0, 1][0, 1] for every type of possibly supported map projection
    virtual QDoubleVector2D geoToMapProjection(const QGeoCoordinate &coordinate) const = 0;
    virtual QGeoCoordinate mapProjectionToGeo(const QDoubleVector2D &projection) const = 0;

    virtual QDoubleVector2D wrapMapProjection(const QDoubleVector2D &projection) const = 0;
    virtual QDoubleVector2D unwrapMapProjection(const QDoubleVector2D &wrappedProjection) const = 0;

    virtual QDoubleVector2D wrappedMapProjectionToItemPosition(const QDoubleVector2D &wrappedProjection) const = 0;
    virtual QDoubleVector2D itemPositionToWrappedMapProjection(const QDoubleVector2D &itemPosition) const = 0;

    // Convenience methods to avoid the chain itemPositionToWrappedProjection(wrapProjection(geoToProjection()))
    // These also come with a default implementation that can, however, be overridden.
    virtual QGeoCoordinate itemPositionToCoordinate(const QDoubleVector2D &pos, bool clipToViewport = true) const = 0;
    virtual QDoubleVector2D coordinateToItemPosition(const QGeoCoordinate &coordinate, bool clipToViewport = true) const = 0;
};

class Q_LOCATION_PRIVATE_EXPORT QGeoProjectionWebMercator : public QGeoProjection
{
public:
    QGeoProjectionWebMercator();
    ~QGeoProjectionWebMercator();

    double minimumZoom() const Q_DECL_OVERRIDE;
    double maximumCenterLatitudeAtZoom(double zoomLevel) const Q_DECL_OVERRIDE;

    // The size of the underlying map, at the current zoom level.
    double mapWidth() const Q_DECL_OVERRIDE;
    double mapHeight() const Q_DECL_OVERRIDE;

    void setViewportSize(const QSize &size) Q_DECL_OVERRIDE;
    void setCameraData(const QGeoCameraData &cameraData) Q_DECL_OVERRIDE;

    QDoubleVector2D geoToMapProjection(const QGeoCoordinate &coordinate) const Q_DECL_OVERRIDE;
    QGeoCoordinate mapProjectionToGeo(const QDoubleVector2D &projection) const Q_DECL_OVERRIDE;

    QDoubleVector2D wrapMapProjection(const QDoubleVector2D &projection) const Q_DECL_OVERRIDE;
    QDoubleVector2D unwrapMapProjection(const QDoubleVector2D &wrappedProjection) const Q_DECL_OVERRIDE;

    QDoubleVector2D wrappedMapProjectionToItemPosition(const QDoubleVector2D &wrappedProjection) const Q_DECL_OVERRIDE;
    QDoubleVector2D itemPositionToWrappedMapProjection(const QDoubleVector2D &itemPosition) const Q_DECL_OVERRIDE;

    QGeoCoordinate itemPositionToCoordinate(const QDoubleVector2D &pos, bool clipToViewport = true) const Q_DECL_OVERRIDE;
    QDoubleVector2D coordinateToItemPosition(const QGeoCoordinate &coordinate, bool clipToViewport = true) const Q_DECL_OVERRIDE;

private:
    void setupCamera();

private:
    QGeoCameraData m_cameraData;
    double m_mapEdgeSize;
    double m_minimumZoom;
    // mercator to camera transform for coordinates (not tiles!)
    double m_cameraCenterXMercator;
    double m_cameraCenterYMercator;
    double m_cameraWidthMercator;
    double m_cameraHeightMercator;
    double m_1_cameraWidthMercator;
    double m_1_cameraHeightMercator;

    // cameraToScreen transform
    double m_viewportWidth; // in pixels
    double m_viewportHeight; // in pixels
    double m_1_viewportWidth;
    double m_1_viewportHeight;
};

QT_END_NAMESPACE

#endif // QGEOPROJECTION_H
