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

#ifndef QMAPICONOBJECTQSG_P_P_H
#define QMAPICONOBJECTQSG_P_P_H

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
#include <QtLocation/private/qmapiconobject_p.h>
#include <QtLocation/private/qmapiconobject_p_p.h>
#include <QtLocation/private/qqsgmapobject_p.h>
#include <QtCore/qscopedvaluerollback.h>
#include <QtGui/qimage.h>

QT_BEGIN_NAMESPACE

class QSGImageNode;
class Q_LOCATION_PRIVATE_EXPORT QMapIconObjectPrivateQSG : public QMapIconObjectPrivateDefault, public QQSGMapObject
{
public:
    QMapIconObjectPrivateQSG(QGeoMapObject *q);
    QMapIconObjectPrivateQSG(const QMapIconObjectPrivate &other);
    ~QMapIconObjectPrivateQSG() override;

    void clearContent();

    // QQSGMapObject
    void updateGeometry() override;
    QSGNode *updateMapObjectNode(QSGNode *oldNode,
                                 VisibleNode **visibleNode,
                                 QSGNode *root,
                                 QQuickWindow *window) override;

    // QGeoMapIconPrivate interface
    void setCoordinate(const QGeoCoordinate &coordinate) override;
    void setContent(const QVariant &content) override;
    void setIconSize(const QSizeF &size) override;

    // QGeoMapObjectPrivate
    QGeoMapObjectPrivate *clone() override;

public:
    // Data Members
    bool m_imageDirty = false;
    bool m_geometryDirty = false;
    QImage m_image;
    QSGImageNode *m_imageNode = nullptr;
    QDoubleVector2D m_itemPosition;
    QMatrix4x4 m_transformation;
};

QT_END_NAMESPACE

#endif // QMAPICONOBJECTQSG_P_P_H
