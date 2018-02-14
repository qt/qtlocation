/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
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

#include "qgeotiledmaplabs_p.h"
#include <QtLocation/private/qgeotiledmap_p_p.h>
#include <QtLocation/private/qgeomapobject_p.h>
#include <QtLocation/private/qmappolylineobjectqsg_p_p.h>
#include <QtLocation/private/qmappolygonobjectqsg_p_p.h>
#include <QtLocation/private/qmapcircleobjectqsg_p_p.h>
#include <QtLocation/private/qmaprouteobjectqsg_p_p.h>
#include <QtLocation/private/qmapiconobjectqsg_p_p.h>

QT_BEGIN_NAMESPACE

struct MapObject {
    MapObject(QPointer<QGeoMapObject> &o, QQSGMapObject *sgo)
        : object(o), sgObject(sgo) {}
    QPointer<QGeoMapObject> object;
    QQSGMapObject *sgObject = nullptr;
};

class QGeoTiledMapLabsPrivate : public QGeoTiledMapPrivate
{
    Q_DECLARE_PUBLIC(QGeoTiledMapLabs)
public:
    QGeoTiledMapLabsPrivate(QGeoTiledMappingManagerEngine *engine);
    virtual ~QGeoTiledMapLabsPrivate();

    QGeoMapObjectPrivate *createMapObjectImplementation(QGeoMapObject *obj) override;
    virtual QList<QGeoMapObject *> mapObjects() const override;
    static int findMapObject(QGeoMapObject *o, const QList<MapObject> &list);
    void removeMapObject(QGeoMapObject *obj);

    void updateMapObjects(QSGNode *root, QQuickWindow *window);
    void updateObjectsGeometry();

protected:
    void changeViewportSize(const QSize &size) override;
    void changeCameraData(const QGeoCameraData &oldCameraData) override;
    void changeActiveMapType(const QGeoMapType mapType) override;

    QList<MapObject> m_mapObjects;
    QList<MapObject> m_pendingMapObjects;
};

QGeoTiledMapLabsPrivate::QGeoTiledMapLabsPrivate(QGeoTiledMappingManagerEngine *engine)
    : QGeoTiledMapPrivate(engine)
{

}

QGeoTiledMapLabsPrivate::~QGeoTiledMapLabsPrivate()
{

}

QGeoMapObjectPrivate *QGeoTiledMapLabsPrivate::createMapObjectImplementation(QGeoMapObject *obj)
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

QList<QGeoMapObject *> QGeoTiledMapLabsPrivate::mapObjects() const
{
    return QList<QGeoMapObject *>();
}

int QGeoTiledMapLabsPrivate::findMapObject(QGeoMapObject *o, const QList<MapObject> &list)
{
    for (int i = 0; i < list.size(); ++i)
    {
        if (list.at(i).object.data() == o)
            return i;
    }
    return -1;
}

void QGeoTiledMapLabsPrivate::removeMapObject(QGeoMapObject *obj)
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

void QGeoTiledMapLabsPrivate::updateMapObjects(QSGNode *root, QQuickWindow *window)
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

void QGeoTiledMapLabsPrivate::updateObjectsGeometry()
{
    Q_Q(QGeoTiledMapLabs);
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
}

void QGeoTiledMapLabsPrivate::changeViewportSize(const QSize &size)
{
    updateObjectsGeometry();
    QGeoTiledMapPrivate::changeViewportSize(size);
}

void QGeoTiledMapLabsPrivate::changeCameraData(const QGeoCameraData &oldCameraData)
{
    updateObjectsGeometry();
    QGeoTiledMapPrivate::changeCameraData(oldCameraData);
}

void QGeoTiledMapLabsPrivate::changeActiveMapType(const QGeoMapType mapType)
{
    updateObjectsGeometry();
    QGeoTiledMapPrivate::changeActiveMapType(mapType);
}


/*
    QGeoTiledMapLabs
*/



QGeoTiledMapLabs::QGeoTiledMapLabs(QGeoTiledMappingManagerEngine *engine, QObject *parent)
    : QGeoTiledMap(*new QGeoTiledMapLabsPrivate(engine), engine, parent)
{

}

QGeoTiledMapLabs::~QGeoTiledMapLabs()
{

}

bool QGeoTiledMapLabs::createMapObjectImplementation(QGeoMapObject *obj)
{
    Q_D(QGeoTiledMapLabs);
    QExplicitlySharedDataPointer<QGeoMapObjectPrivate> pimpl =
            QExplicitlySharedDataPointer<QGeoMapObjectPrivate>(d->createMapObjectImplementation(obj));
    if (pimpl.constData()) {
        bool res = obj->setImplementation(pimpl);
        if (res)
            emit sgNodeChanged();
        return res;
    }
    return false;
}

QSGNode *QGeoTiledMapLabs::updateSceneGraph(QSGNode *node, QQuickWindow *window)
{
    Q_D(QGeoTiledMapLabs);
    QSGNode *root = QGeoTiledMap::updateSceneGraph(node, window);
    d->updateMapObjects(root, window);
    return root;
}

void QGeoTiledMapLabs::removeMapObject(QGeoMapObject *obj)
{
    Q_D(QGeoTiledMapLabs);
    d->removeMapObject(obj);
}

QGeoTiledMapLabs::QGeoTiledMapLabs(QGeoTiledMapLabsPrivate &dd, QGeoTiledMappingManagerEngine *engine, QObject *parent)
    : QGeoTiledMap(dd, engine, parent)
{

}

QT_END_NAMESPACE

