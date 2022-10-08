/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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
#ifndef QGEOMAP_P_P_H
#define QGEOMAP_P_P_H

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
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeoprojection_p.h>
#include <QtLocation/private/qgeomap_p.h>
#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtCore/private/qobject_p.h>
#include <QtCore/QSize>
#include <QtCore/QList>
#include "qgeomap_p.h"


QT_BEGIN_NAMESPACE

class QGeoMappingManagerEngine;
class QGeoMap;
class QGeoMapController;
class QDeclarativeGeoMapItemBase;

class Q_LOCATION_PRIVATE_EXPORT QGeoMapPrivate :  public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGeoMap)
public:
    QGeoMapPrivate(QGeoMappingManagerEngine *engine, QGeoProjection *geoProjection);
    virtual ~QGeoMapPrivate();

    const QGeoProjection *geoProjection() const;
    void setCameraCapabilities(const QGeoCameraCapabilities &cameraCapabilities);
    const QGeoCameraCapabilities &cameraCapabilities() const;

    static const QGeoMapPrivate *get(const QGeoMap &map);

protected:
    /* Hooks into the actual map implementations */
    virtual QGeoMap::ItemTypes supportedMapItemTypes() const;
    virtual void addMapItem(QDeclarativeGeoMapItemBase *item);
    virtual void removeMapItem(QDeclarativeGeoMapItemBase *item);

    virtual void changeViewportSize(const QSize &size) = 0; // called by QGeoMap::setSize()
    virtual void changeCameraData(const QGeoCameraData &oldCameraData) = 0; // called by QGeoMap::setCameraData()
    virtual void changeActiveMapType(const QGeoMapType &mapType) = 0; // called by QGeoMap::setActiveMapType()

    virtual double mapWidth() const;
    virtual double mapHeight() const;

    virtual void setCopyrightVisible(bool visible);
    virtual bool copyrightVisible() const;
    virtual double maximumCenterLatitudeAtZoom(const QGeoCameraData &cameraData) const;
    virtual double minimumCenterLatitudeAtZoom(const QGeoCameraData &cameraData) const;

    virtual void setVisibleArea(const QRectF &visibleArea);
    virtual QRectF visibleArea() const;

    QRectF clampVisibleArea(const QRectF &visibleArea) const;

#ifdef QT_LOCATION_DEBUG
public:
#else
protected:
#endif
    QSize m_viewportSize;
    QGeoProjection *m_geoProjection;
    QPointer<QGeoMappingManagerEngine> m_engine;
    QGeoCameraData m_cameraData;
    QGeoMapType m_activeMapType;
    QList<QDeclarativeGeoMapItemBase *> m_mapItems;
    QGeoCameraCapabilities m_cameraCapabilities;
    bool m_copyrightVisible = true;
    mutable double m_maximumViewportLatitude = 0;
    mutable double m_minimumViewportLatitude = 0;
};

QT_END_NAMESPACE

#endif // QGEOMAP_P_P_H
