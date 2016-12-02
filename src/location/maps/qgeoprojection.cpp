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
#include <QSize>
#include <cmath>

QT_BEGIN_NAMESPACE

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
      m_cameraWidthMercator(1),
      m_cameraHeightMercator(1),
      m_1_cameraWidthMercator(0),
      m_1_cameraHeightMercator(0),
      m_viewportWidth(1),
      m_viewportHeight(1),
      m_1_viewportWidth(0),
      m_1_viewportHeight(0)
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
    double mapEdgeSize = std::pow(2.0, zoomLevel) * 256.0;

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
    m_minimumZoom =  std::log(qMax(m_viewportWidth, m_viewportHeight) / 256.0) / std::log(2.0);
    setupCamera();
}

void QGeoProjectionWebMercator::setCameraData(const QGeoCameraData &cameraData)
{
    m_cameraData = cameraData;
    m_mapEdgeSize = std::pow(2.0, cameraData.zoomLevel()) * 256.0;
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
    // TODO: Support tilt/bearing through a projection matrix.
    double x = ((wrappedProjection.x() - m_cameraCenterXMercator) * m_1_cameraWidthMercator + 0.5) * m_viewportWidth;
    double y = ((wrappedProjection.y() - m_cameraCenterYMercator) * m_1_cameraHeightMercator + 0.5) * m_viewportHeight;
    return QDoubleVector2D(x, y);
}

QDoubleVector2D QGeoProjectionWebMercator::itemPositionToWrappedMapProjection(const QDoubleVector2D &itemPosition) const
{
    // TODO: Support tilt/bearing through an inverse projection matrix.
    double x = itemPosition.x();
    x *= m_1_viewportWidth;
    x -= 0.5;
    x *= m_cameraWidthMercator;
    x += m_cameraCenterXMercator;

    double y = itemPosition.y();
    y *= m_1_viewportHeight;
    y -= 0.5;
    y *= m_cameraHeightMercator;
    y += m_cameraCenterYMercator;

    return QDoubleVector2D(x, y);
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
    // TODO: test for it
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

void QGeoProjectionWebMercator::setupCamera()
{
    m_cameraWidthMercator = m_viewportWidth / m_mapEdgeSize;
    m_cameraHeightMercator = m_viewportHeight / m_mapEdgeSize;
    m_1_cameraWidthMercator = 1.0 / m_cameraWidthMercator;
    m_1_cameraHeightMercator = 1.0 / m_cameraHeightMercator;
    QDoubleVector2D camCenterMercator = QWebMercator::coordToMercator(m_cameraData.center());
    m_cameraCenterXMercator = camCenterMercator.x();
    m_cameraCenterYMercator = camCenterMercator.y();
}

QT_END_NAMESPACE
