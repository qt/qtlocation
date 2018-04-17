/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "qgeomapitemsoverlay.h"
#include "qgeomappingmanagerengineitemsoverlay.h"
#include <QtLocation/private/qgeomap_p_p.h>
#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgrectanglenode.h>
#include <QtQuick/qquickwindow.h>

#ifdef LOCATIONLABS
#include <QtLocation/private/qmappolylineobjectqsg_p_p.h>
#include <QtLocation/private/qmappolygonobjectqsg_p_p.h>
#include <QtLocation/private/qmapcircleobjectqsg_p_p.h>
#include <QtLocation/private/qmaprouteobjectqsg_p_p.h>
#include <QtLocation/private/qmapiconobjectqsg_p_p.h>
struct MapObject {
    MapObject(QPointer<QGeoMapObject> &o, QQSGMapObject *sgo)
        : object(o), sgObject(sgo) {}
    QPointer<QGeoMapObject> object;
    QQSGMapObject *sgObject = nullptr;
};
#endif

QT_BEGIN_NAMESPACE

class QGeoMapItemsOverlayPrivate : public QGeoMapPrivate
{
    Q_DECLARE_PUBLIC(QGeoMapItemsOverlay)
public:
    QGeoMapItemsOverlayPrivate(QGeoMappingManagerEngineItemsOverlay *engine);
    virtual ~QGeoMapItemsOverlayPrivate();

#ifdef LOCATIONLABS
    QGeoMapObjectPrivate *createMapObjectImplementation(QGeoMapObject *obj) override;
    virtual QList<QGeoMapObject *> mapObjects() const override;
    static int findMapObject(QGeoMapObject *o, const QList<MapObject> &list);
    void removeMapObject(QGeoMapObject *obj);
    void updateMapObjects(QSGNode *root, QQuickWindow *window);

    QList<MapObject> m_mapObjects;
    QList<MapObject> m_pendingMapObjects;
#endif

    void updateObjectsGeometry();
protected:
    void changeViewportSize(const QSize &size) override;
    void changeCameraData(const QGeoCameraData &oldCameraData) override;
    void changeActiveMapType(const QGeoMapType mapType) override;
};

QGeoMapItemsOverlay::QGeoMapItemsOverlay(QGeoMappingManagerEngineItemsOverlay *engine, QObject *parent)
    : QGeoMap(*(new QGeoMapItemsOverlayPrivate(engine)), parent)
{

}

QGeoMapItemsOverlay::~QGeoMapItemsOverlay()
{
}

QGeoMap::Capabilities QGeoMapItemsOverlay::capabilities() const
{
    return Capabilities(SupportsVisibleRegion
                        | SupportsSetBearing
                        | SupportsAnchoringCoordinate);
}

QSGNode *QGeoMapItemsOverlay::updateSceneGraph(QSGNode *node, QQuickWindow *window)
{
#ifndef LOCATIONLABS
    Q_UNUSED(window)
    return node;
#else
    Q_D(QGeoMapItemsOverlay);

    QSGRectangleNode *mapRoot = static_cast<QSGRectangleNode *>(node);
    if (!mapRoot)
        mapRoot = window->createRectangleNode();

    mapRoot->setRect(QRect(0, 0, viewportWidth(), viewportHeight()));
    mapRoot->setColor(QColor(0,0,0,0));

    d->updateMapObjects(mapRoot, window);
    return mapRoot;
#endif
}

QGeoMapItemsOverlayPrivate::QGeoMapItemsOverlayPrivate(QGeoMappingManagerEngineItemsOverlay *engine)
    : QGeoMapPrivate(engine, new QGeoProjectionWebMercator)
{
}

QGeoMapItemsOverlayPrivate::~QGeoMapItemsOverlayPrivate()
{
}

#ifdef LOCATIONLABS
QGeoMapObjectPrivate *QGeoMapItemsOverlayPrivate::createMapObjectImplementation(QGeoMapObject *obj)
{
    switch (obj->type()) {
        case QGeoMapObject::PolylineType: {
            QMapPolylineObjectPrivate &oldImpl = static_cast<QMapPolylineObjectPrivate &>(*obj->implementation());
            QMapPolylineObjectPrivateQSG *pimpl =
                    new QMapPolylineObjectPrivateQSG(oldImpl);
            QPointer<QGeoMapObject> p(obj);
            MapObject mo(p, pimpl);
            m_pendingMapObjects << mo;
            return pimpl;
        }
        case QGeoMapObject::PolygonType: {
            QMapPolygonObjectPrivate &oldImpl = static_cast<QMapPolygonObjectPrivate &>(*obj->implementation());
            QMapPolygonObjectPrivateQSG *pimpl =
                    new QMapPolygonObjectPrivateQSG(oldImpl);
            QPointer<QGeoMapObject> p(obj);
            MapObject mo(p, pimpl);
            m_pendingMapObjects << mo;
            return pimpl;
        }
        case QGeoMapObject::CircleType: {
            QMapCircleObjectPrivate &oldImpl = static_cast<QMapCircleObjectPrivate &>(*obj->implementation());
            QMapCircleObjectPrivateQSG *pimpl =
                    new QMapCircleObjectPrivateQSG(oldImpl);
            QPointer<QGeoMapObject> p(obj);
            MapObject mo(p, pimpl);
            m_pendingMapObjects << mo;
            return pimpl;
        }
        case QGeoMapObject::RouteType: {
            QMapRouteObjectPrivate &oldImpl = static_cast<QMapRouteObjectPrivate &>(*obj->implementation());
            QMapRouteObjectPrivateQSG *pimpl =
                    new QMapRouteObjectPrivateQSG(oldImpl);
            QPointer<QGeoMapObject> p(obj);
            MapObject mo(p, pimpl);
            m_pendingMapObjects << mo;
            return pimpl;
        }
        case QGeoMapObject::IconType: {
            QMapIconObjectPrivate &oldImpl = static_cast<QMapIconObjectPrivate &>(*obj->implementation());
            QMapIconObjectPrivateQSG *pimpl =
                    new QMapIconObjectPrivateQSG(oldImpl);
            QPointer<QGeoMapObject> p(obj);
            MapObject mo(p, pimpl);
            m_pendingMapObjects << mo;
            return pimpl;
        }
        default:
            qWarning() << "Unsupported object type: " << obj->type();
            break;
    }
    return nullptr;
}

QList<QGeoMapObject *> QGeoMapItemsOverlayPrivate::mapObjects() const
{
    return QList<QGeoMapObject *>();
}

int QGeoMapItemsOverlayPrivate::findMapObject(QGeoMapObject *o, const QList<MapObject> &list)
{
    for (int i = 0; i < list.size(); ++i)
    {
        if (list.at(i).object.data() == o)
            return i;
    }
    return -1;
}

void QGeoMapItemsOverlayPrivate::removeMapObject(QGeoMapObject *obj)
{
    int idx = findMapObject(obj, m_mapObjects);
    if (idx >= 0) {
        m_mapObjects.removeAt(idx);
    } else {
        idx = findMapObject(obj, m_pendingMapObjects);
        if (idx >= 0) {
            m_pendingMapObjects.removeAt(idx);
        } else {
            // obj not here.
        }
    }
}

void QGeoMapItemsOverlayPrivate::updateMapObjects(QSGNode *root, QQuickWindow *window)
{
    for (int i = 0; i < m_mapObjects.size(); ++i) {
        // already added as node
        if (!m_mapObjects.at(i).object) {
            qWarning() << "m_mapObjects at "<<i<< " NULLed!!";
            continue;
        }

        QQSGMapObject *sgo = m_mapObjects.at(i).sgObject;
        QSGNode *oldNode = sgo->node;
        sgo->node = sgo->updateMapObjectNode(oldNode, root, window);
    }

    QList<int> toRemove;
    for (int i = 0; i < m_pendingMapObjects.size(); ++i) {
        // already added as node
        QQSGMapObject *sgo = m_pendingMapObjects.at(i).sgObject;
        QSGNode *oldNode = sgo->node;
        sgo->updateGeometry(); // or subtree will be blocked
        sgo->node = sgo->updateMapObjectNode(oldNode, root, window);
        if (sgo->node) {
            m_mapObjects << m_pendingMapObjects.at(i);
            toRemove.push_front(i);
        } else {
            // leave it to be processed
        }
    }

    for (int i: qAsConst(toRemove))
        m_pendingMapObjects.removeAt(i);
}
#endif

void QGeoMapItemsOverlayPrivate::updateObjectsGeometry()
{
#ifdef LOCATIONLABS
    Q_Q(QGeoMapItemsOverlay);
    for (int i = 0; i < m_mapObjects.size(); ++i) {
        // already added as node
        if (!m_mapObjects.at(i).object) {
            qWarning() << "m_mapObjects at "<<i<< " NULLed!!";
            continue;
        }

        QQSGMapObject *sgo = m_mapObjects.at(i).sgObject;
        sgo->updateGeometry();
    }
    emit q->sgNodeChanged();
#endif
}

void QGeoMapItemsOverlayPrivate::changeViewportSize(const QSize &/*size*/)
{
    updateObjectsGeometry();
}

void QGeoMapItemsOverlayPrivate::changeCameraData(const QGeoCameraData &/*oldCameraData*/)
{
    updateObjectsGeometry();
}

void QGeoMapItemsOverlayPrivate::changeActiveMapType(const QGeoMapType /*mapType*/)
{
    updateObjectsGeometry();
}

QT_END_NAMESPACE




