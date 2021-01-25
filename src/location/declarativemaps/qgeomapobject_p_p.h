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

#ifndef QGEOMAPOBJECTBASE_P_H
#define QGEOMAPOBJECTBASE_P_H

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
#include <QtLocation/private/qgeomap_p.h>
#include <QSharedData>
#include <QPointer>

#include <QUrl>
#include "qgeomapobject_p.h"

QT_BEGIN_NAMESPACE

class QGeoMapObject;
class Q_LOCATION_PRIVATE_EXPORT QGeoMapObjectPrivate : public QSharedData
{
public:
    virtual ~QGeoMapObjectPrivate();

    bool operator == (const QGeoMapObjectPrivate &other) const;

    virtual QByteArray engineName() const;
    virtual QGeoMapObject::Features features() const;
    virtual bool equals(const QGeoMapObjectPrivate &other) const;
    virtual QGeoMapObject::Type type() const;
    virtual bool visible() const;
    virtual void setVisible(bool visible);
    virtual void setParentVisibility(bool visible);
    virtual void setMap(QGeoMap *map);
    virtual QGeoMapObjectPrivate *clone() = 0; // to allow proper detaching
    virtual QGeoShape geoShape() const = 0;
    virtual void setGeoShape(const QGeoShape &shape) = 0;

    QGeoMapObject *q = nullptr;
    QPointer<QGeoMap> m_map;
    bool m_componentCompleted = false;
    bool m_visible = true;
    bool m_parentVisible = true;

protected:
    QGeoMapObjectPrivate(QGeoMapObject *q);
    QGeoMapObjectPrivate(const QGeoMapObjectPrivate &other);

private:
    QGeoMapObjectPrivate();
};

QT_END_NAMESPACE

#endif // QGEOMAPOBJECTBASE_P_H
