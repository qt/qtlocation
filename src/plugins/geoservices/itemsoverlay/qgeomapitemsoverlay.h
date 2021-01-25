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


#ifndef QGEOMAPITEMSOVERLAY_H
#define QGEOMAPITEMSOVERLAY_H

#include <QtLocation/private/qgeomap_p.h>
#include <QtLocation/private/qgeoprojection_p.h>

QT_BEGIN_NAMESPACE

class QGeoMappingManagerEngineItemsOverlay;
class QGeoMapItemsOverlayPrivate;
class QGeoMapItemsOverlay: public QGeoMap
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGeoMapItemsOverlay)
public:
    QGeoMapItemsOverlay(QGeoMappingManagerEngineItemsOverlay *engine, QObject *parent);
    virtual ~QGeoMapItemsOverlay();

    QGeoMap::Capabilities capabilities() const override;
    bool createMapObjectImplementation(QGeoMapObject *obj) override;
    void removeMapObject(QGeoMapObject *obj) override;
    QList<QObject *> mapObjectsAt(const QGeoCoordinate &coordinate) const override;

protected:
    QSGNode *updateSceneGraph(QSGNode *node, QQuickWindow *window) override;

private:
    Q_DISABLE_COPY(QGeoMapItemsOverlay)
};

QT_END_NAMESPACE

#endif
