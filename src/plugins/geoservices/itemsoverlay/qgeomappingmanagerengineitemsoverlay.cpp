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

#include "qgeomappingmanagerengineitemsoverlay.h"

#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include "qgeomapitemsoverlay.h"

QT_BEGIN_NAMESPACE



QGeoMappingManagerEngineItemsOverlay::QGeoMappingManagerEngineItemsOverlay(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
:   QGeoMappingManagerEngine()
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);
    Q_UNUSED(errorString);
    QGeoCameraCapabilities cameraCaps;
    cameraCaps.setMinimumZoomLevel(0.0);
    cameraCaps.setMaximumZoomLevel(30.0);
    cameraCaps.setSupportsBearing(true);
    cameraCaps.setSupportsTilting(true);
    cameraCaps.setMinimumTilt(0);
    cameraCaps.setMaximumTilt(89);
    cameraCaps.setMinimumFieldOfView(1.0);
    cameraCaps.setMaximumFieldOfView(179.0);
    setCameraCapabilities(cameraCaps);

    QList<QGeoMapType> mapTypes;
    mapTypes << QGeoMapType(QGeoMapType::NoMap, tr("Empty Map"), tr("Empty Map"), false, false, 1, "itemsoverlay", cameraCaps);
    setSupportedMapTypes(mapTypes);

    engineInitialized();
}

QGeoMappingManagerEngineItemsOverlay::~QGeoMappingManagerEngineItemsOverlay()
{
}

QGeoMap *QGeoMappingManagerEngineItemsOverlay::createMap()
{
    return new QGeoMapItemsOverlay(this, this);
}

QT_END_NAMESPACE
