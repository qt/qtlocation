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

#include "qmapiconobjectqsg_p_p.h"
#include <QtQuick/qsgimagenode.h>
#include <QtQuick/qsgnode.h>
#include <QtQuick/private/qquickimage_p.h>
#include <QtQuick/qquickimageprovider.h>
#include <QtQuick/qquickwindow.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqml.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>

QT_BEGIN_NAMESPACE

class RootNode : public QSGTransformNode, public VisibleNode
{
public:
    RootNode() { }

    bool isSubtreeBlocked() const override
    {
        return subtreeBlocked();
    }
};

QMapIconObjectPrivateQSG::QMapIconObjectPrivateQSG(QGeoMapObject *q)
    : QMapIconObjectPrivateDefault(q)
{

}

QMapIconObjectPrivateQSG::QMapIconObjectPrivateQSG(const QMapIconObjectPrivate &other)
    : QMapIconObjectPrivateDefault(other)
{
    // Data already cloned by the *Default copy constructor, but necessary
    // update operations triggered only by setters overrides
    setContent(content());
//    setCoordinate(coordinate());
}

QMapIconObjectPrivateQSG::~QMapIconObjectPrivateQSG()
{
    if (m_map)
        m_map->removeMapObject(q);
}

void QMapIconObjectPrivateQSG::updateGeometry()
{
    if (!m_map)
        return;

    m_geometryDirty = true;
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_map->geoProjection());

    m_itemPosition = p.coordinateToItemPosition(coordinate());
    if (m_itemPosition.isFinite()) {
        m_transformation.setToIdentity();
        m_transformation.translate(QVector3D(m_itemPosition.x(), m_itemPosition.y(), 0));
    }

    // TODO: support and test for zoomLevel
}

QSGNode *QMapIconObjectPrivateQSG::updateMapObjectNode(QSGNode *oldNode,
                                                       VisibleNode **visibleNode,
                                                       QSGNode *root,
                                                       QQuickWindow *window)
{
    Q_UNUSED(visibleNode);
    RootNode *node = static_cast<RootNode *>(oldNode);
    if (!node) {
        node = new RootNode();
        m_imageNode = window->createImageNode();
        m_imageNode->setOwnsTexture(true);
        node->appendChildNode(m_imageNode);
        *visibleNode = static_cast<VisibleNode *>(node);
    }

    if (m_imageDirty) {
        m_imageDirty = false;
        m_imageNode->setTexture(window->createTextureFromImage(m_image));
        QRect rect = m_image.rect();
        m_imageNode->setSourceRect(rect);
        m_imageNode->setRect(QRectF(QPointF(0,0), iconSize()));
    }

    if (m_geometryDirty) {
        m_geometryDirty = false;
        if (!m_itemPosition.isFinite()) {
            node->setSubtreeBlocked(true);
        } else {
            node->setSubtreeBlocked(false);
            node->setMatrix(m_transformation);
        }
    }

    root->appendChildNode(node);

    return node;
}

void QMapIconObjectPrivateQSG::setCoordinate(const QGeoCoordinate &coordinate)
{
    QMapIconObjectPrivateDefault::setCoordinate(coordinate);
    updateGeometry();
}

template<typename T>
static T *getContent(const QVariant &content)
{
    QObject *obj = qvariant_cast<QObject *>(content);
    return qobject_cast<T *>(obj);
}

static inline QString imageId(const QUrl &url)
{
    return url.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority).mid(1);
}

void QMapIconObjectPrivateQSG::clearContent()
{
    m_image = QImage();
}

void QMapIconObjectPrivateQSG::setContent(const QVariant &content)
{
    // First reset all local containers
    clearContent();
    QQmlEngine *engine = qmlEngine(q);

    // Then pull the new content
    QMapIconObjectPrivateDefault::setContent(content);
    switch (content.type()) {
        case QVariant::UserType: {
            // TODO: Handle QObject subclasses -- first decide which ones
            break;
        }
        case QVariant::String:
        case QVariant::Url: {
            // URL, including image/texture providers
            // Supporting only image providers for now
            const QUrl url = content.toUrl();
            if (!url.isValid()) {
                m_image = QImage(content.toString());
                m_imageDirty = true;
                updateGeometry();
            } else if (url.scheme().isEmpty() || url.scheme() == QLatin1String("file")) {
                m_image = QImage(url.toString(QUrl::RemoveScheme));
                m_imageDirty = true;
                updateGeometry();
            } else if (url.scheme() == QLatin1String("image")) {
                QQuickImageProvider *provider = static_cast<QQuickImageProvider *>(engine->imageProvider(url.host()));
                QSize outSize;
                m_image = provider->requestImage(imageId(url), &outSize, QSize());
                if (outSize.isEmpty())
                    break;
                m_imageDirty = true;
                updateGeometry();
            } else { // ToDo: Use QNAM

            }

            break;
        }
        case QVariant::ByteArray: {
            // ToDo: Build the image from bytearray
            break;
        }
        default:
            qWarning() << "Unsupported parameter type: " << content.type();
            break;
    }

    if (m_map && m_imageDirty)
        emit m_map->sgNodeChanged();
}

void QMapIconObjectPrivateQSG::setIconSize(const QSizeF &size)
{
    QMapIconObjectPrivateDefault::setIconSize(size);
    updateGeometry();
}

QGeoMapObjectPrivate *QMapIconObjectPrivateQSG::clone()
{
    return new QMapIconObjectPrivateQSG(static_cast<QMapIconObjectPrivate &>(*this));
}

QT_END_NAMESPACE
