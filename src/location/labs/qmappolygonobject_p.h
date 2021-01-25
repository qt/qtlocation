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

#ifndef QMAPPOLYGONOBJECT_P_H
#define QMAPPOLYGONOBJECT_P_H

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
#include <QtLocation/private/qgeomapobject_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>

#include <QJSValue>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QMapPolygonObject : public QGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeMapLineProperties *border READ border CONSTANT)

public:
    QMapPolygonObject(QObject *parent = nullptr);
    ~QMapPolygonObject() override;

    QVariantList path() const;
    void setPath(const QVariantList &path);

    QColor color() const;
    void setColor(const QColor &color);

    QDeclarativeMapLineProperties *border();
    void setMap(QGeoMap *map) override;

signals:
    void pathChanged();
    void colorChanged();

protected:
    QDeclarativeMapLineProperties *m_border = nullptr;
};

QT_END_NAMESPACE

#endif // QMAPPOLYGONOBJECT_P_H
