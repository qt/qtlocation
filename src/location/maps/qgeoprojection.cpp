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

#include "qgeoprojection_p.h"
#include <QtPositioning/private/qwebmercator_p.h>
#include <QtPositioning/private/qlocationutils_p.h>
#include <QSize>
#include <cmath>

QT_BEGIN_NAMESPACE

static const double defaultTileSize = 256.0;

QGeoProjection::QGeoProjection()
{

}

QGeoProjection::~QGeoProjection()
{

}

/*
 * QGeoProjectionWebMercator implementation
*/


QGeoProjectionWebMercator::QGeoProjectionWebMercator()
    : QGeoProjection(),
      m_mapEdgeSize(256), // at zl 0
      m_minimumZoom(0),
      m_cameraCenterXMercator(0),
      m_cameraCenterYMercator(0),
      m_viewportWidth(1),
      m_viewportHeight(1),
      m_1_viewportWidth(0),
      m_1_viewportHeight(0),
      m_sideLength(256),
      m_aperture(0.0),
      m_nearPlane(0.0),
      m_farPlane(0.0),
      m_halfWidth(0.0),
      m_halfHeight(0.0),
      m_plane(QDoubleVector3D(0,0,0), QDoubleVector3D(0,0,1))
{

}

QGeoProjectionWebMercator::~QGeoProjectionWebMercator()
{

}

// This method returns the minimum zoom level that this specific qgeomap type allows
// at the current viewport size and for the default tile size of 256^2.
double QGeoProjectionWebMercator::minimumZoom() const
{
    return m_minimumZoom;
}

// This method recalculates the "no-trespassing" limits for the map center.
// This has to be used when:
// 1) the map is resized, because the meters per pixel remain the same, but
//    the amount of pixels between the center and the borders changes
// 2) when the zoom level changes, because the amount of pixels between the center
//    and the borders stays the same, but the meters per pixel change
double QGeoProjectionWebMercator::maximumCenterLatitudeAtZoom(double zoomLevel) const
{
    double mapEdgeSize = std::pow(2.0, zoomLevel) * defaultTileSize;

    // At init time weird things happen
    int clampedWindowHeight = (m_viewportHeight > mapEdgeSize) ? mapEdgeSize : m_viewportHeight;

    // Use the window height divided by 2 as the topmost allowed center, with respect to the map size in pixels
    double mercatorTopmost = (clampedWindowHeight * 0.5) /  mapEdgeSize ;
    QGeoCoordinate topMost = QWebMercator::mercatorToCoord(QDoubleVector2D(0.0, mercatorTopmost));

    return topMost.latitude();
}

double QGeoProjectionWebMercator::mapWidth() const
{
    return m_mapEdgeSize;
}

double QGeoProjectionWebMercator::mapHeight() const
{
    return m_mapEdgeSize;
}

void QGeoProjectionWebMercator::setViewportSize(const QSize &size)
{
    m_viewportWidth = size.width();
    m_viewportHeight = size.height();
    m_1_viewportWidth = 1.0 / m_viewportWidth;
    m_1_viewportHeight = 1.0 / m_viewportHeight;
    m_minimumZoom =  std::log(qMax(m_viewportWidth, m_viewportHeight) / defaultTileSize) / std::log(2.0);
    setupCamera();
}

void QGeoProjectionWebMercator::setCameraData(const QGeoCameraData &cameraData)
{
    m_cameraData = cameraData;
    m_mapEdgeSize = std::pow(2.0, cameraData.zoomLevel()) * defaultTileSize;
    setupCamera();
}

QDoubleVector2D QGeoProjectionWebMercator::geoToMapProjection(const QGeoCoordinate &coordinate) const
{
    return QWebMercator::coordToMercator(coordinate);
}

QGeoCoordinate QGeoProjectionWebMercator::mapProjectionToGeo(const QDoubleVector2D &projection) const
{
    return QWebMercator::mercatorToCoord(projection);
}

//wraps around center
QDoubleVector2D QGeoProjectionWebMercator::wrapMapProjection(const QDoubleVector2D &projection) const
{
    double x = projection.x();
    if (m_cameraCenterXMercator < 0.5) {
        if (x - m_cameraCenterXMercator > 0.5 )
            x -= 1.0;
    } else if (m_cameraCenterXMercator > 0.5) {
        if (x - m_cameraCenterXMercator < -0.5 )
            x += 1.0;
    }

    return QDoubleVector2D(x, projection.y());
}

QDoubleVector2D QGeoProjectionWebMercator::unwrapMapProjection(const QDoubleVector2D &wrappedProjection) const
{
    double x = wrappedProjection.x();
    if (x > 1.0)
        return QDoubleVector2D(x - 1.0, wrappedProjection.y());
    if (x <= 0.0)
        return QDoubleVector2D(x + 1.0, wrappedProjection.y());
    return wrappedProjection;
}

QDoubleVector2D QGeoProjectionWebMercator::wrappedMapProjectionToItemPosition(const QDoubleVector2D &wrappedProjection) const
{
    QDoubleVector3D pos = wrappedProjection * m_sideLength;
    QDoubleVector2D res =  (m_transformation * pos).toVector2D();
    res += QDoubleVector2D(1.0,1.0);
    res *= 0.5;
    res *= QDoubleVector2D(m_viewportWidth, m_viewportHeight);
    return res;
}

QDoubleVector2D QGeoProjectionWebMercator::itemPositionToWrappedMapProjection(const QDoubleVector2D &itemPosition) const
{
    QDoubleVector2D pos = itemPosition;
    pos /= QDoubleVector2D(m_viewportWidth, m_viewportHeight);
    pos *= 2.0;
    pos -= QDoubleVector2D(1.0,1.0);
    pos *= QDoubleVector2D(m_halfWidth, m_halfHeight);

    QDoubleVector3D p = m_centerNearPlane;
    p -= m_up * pos.y();
    p -= m_side * pos.x();

    QDoubleVector3D ray = p - m_eye;
    ray.normalize();

    return (m_plane.lineIntersection(m_eye, ray) / m_sideLength).toVector2D();
}

/* Default implementations */
QGeoCoordinate QGeoProjectionWebMercator::itemPositionToCoordinate(const QDoubleVector2D &pos, bool clipToViewport) const
{
    if (clipToViewport) {
        int w = m_viewportWidth;
        int h = m_viewportHeight;

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }

    QDoubleVector2D wrappedMapProjection = itemPositionToWrappedMapProjection(pos);
    // With rotation/tilting, a screen position might end up outside the projection space.
    if (!isProjectable(wrappedMapProjection))
        return QGeoCoordinate();
    return mapProjectionToGeo(unwrapMapProjection(wrappedMapProjection));
}

QDoubleVector2D QGeoProjectionWebMercator::coordinateToItemPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    QDoubleVector2D pos = wrappedMapProjectionToItemPosition(wrapMapProjection(geoToMapProjection(coordinate)));

    if (clipToViewport) {
        int w = m_viewportWidth;
        int h = m_viewportHeight;
        double x = pos.x();
        double y = pos.y();
        if ((x < -0.5) || (x > w + 0.5) || (y < -0.5) || (y > h + 0.5) || qIsNaN(x) || qIsNaN(y))
            return QDoubleVector2D(qQNaN(), qQNaN());
    }
    return pos;
}

bool QGeoProjectionWebMercator::isProjectable(const QDoubleVector2D &wrappedProjection) const
{
    QDoubleVector3D pos = wrappedProjection * m_sideLength;

    // TODO: add an offset to the eye
    QDoubleVector3D p = m_eye - pos;
    double dot = QDoubleVector3D::dotProduct(p , m_viewNormalized);

    if (dot < 0.0) // behind the eye
        return false;
    return true;
}

void QGeoProjectionWebMercator::setupCamera()
{
    QDoubleVector2D camCenterMercator = geoToMapProjection(m_cameraData.center());
    m_cameraCenterXMercator = camCenterMercator.x();
    m_cameraCenterYMercator = camCenterMercator.y();

    int intZoomLevel = static_cast<int>(std::floor(m_cameraData.zoomLevel()));
    m_sideLength = (1 << intZoomLevel) * defaultTileSize;
    m_center = camCenterMercator * m_sideLength;

    double f = 1.0 * qMin(m_viewportWidth, m_viewportHeight);
    double z = std::pow(2.0, m_cameraData.zoomLevel() - intZoomLevel) * defaultTileSize;
    double altitude = f / (2.0 * z) ;

    // calculate eye
    // TODO: support field of view with apertureSize = tan(QLocationUtils::radians(m_cameraData.fieldOfView()) * 0.5);
    double m_aperture = 1.0; //aperture(90 / 2) = 1

    m_eye = m_center;
    m_eye.setZ(altitude * defaultTileSize / m_aperture);

    m_view = m_eye - m_center;
    QDoubleVector3D side = QDoubleVector3D::normal(m_view, QDoubleVector3D(0.0, 1.0, 0.0));
    m_up = QDoubleVector3D::normal(side, m_view);

    if (m_cameraData.bearing() > 0.0) {
        // old bearing, tilt and roll code
        QDoubleMatrix4x4 mBearing;
        mBearing.rotate(m_cameraData.bearing(), m_view);
        m_up = mBearing * m_up;
    }

    m_side = QDoubleVector3D::normal(m_up, m_view);

    if (m_cameraData.tilt() > 0.0) {
        QDoubleMatrix4x4 mTilt;
        mTilt.rotate(-m_cameraData.tilt(), m_side);
        m_eye = mTilt * m_view + m_center;
    }

    m_view = m_eye - m_center;
    m_viewNormalized = m_view.normalized();
    m_up = QDoubleVector3D::normal(m_view, m_side);

    m_nearPlane = 1;
    // 10000.0 to make sure that everything fits in the frustum even at high zoom levels
    // (that is, with a very large map)
    // TODO: extend this to support clip distance
    m_farPlane =  (altitude + 10000.0) * defaultTileSize;

    double aspectRatio = 1.0 * m_viewportWidth / m_viewportHeight;

    m_halfWidth = 1 * m_aperture;
    m_halfHeight = 1 * m_aperture;
    if (aspectRatio > 1.0) {
        m_halfWidth *= aspectRatio;
    } else if (aspectRatio > 0.0 && aspectRatio < 1.0) {
        m_halfHeight /= aspectRatio;
    }

    QDoubleMatrix4x4 cameraMatrix;
    cameraMatrix.lookAt(m_eye, m_center, m_up);

    QDoubleMatrix4x4 projectionMatrix;
    projectionMatrix.frustum(-m_halfWidth, m_halfWidth, -m_halfHeight, m_halfHeight, m_nearPlane, m_farPlane);

    m_transformation = projectionMatrix * cameraMatrix;
    m_centerNearPlane = m_eye + m_view.normalized();
}

QT_END_NAMESPACE
