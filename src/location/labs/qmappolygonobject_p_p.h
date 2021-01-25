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

#ifndef QMAPPOLYGONOBJECT_P_P_H
#define QMAPPOLYGONOBJECT_P_P_H

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
#include <QtLocation/private/qgeomapobject_p_p.h>
#include <QGeoCoordinate>
#include <QColor>
#include <QGeoPolygon>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QMapPolygonObjectPrivate : public QGeoMapObjectPrivate
{
public:
    QMapPolygonObjectPrivate(QGeoMapObject *q);
    ~QMapPolygonObjectPrivate() override;

    virtual QGeoMapObject::Type type() const override final;

    virtual QList<QGeoCoordinate> path() const = 0;
    virtual void setPath(const QList<QGeoCoordinate> &path) = 0;
    virtual QColor fillColor() const = 0;
    virtual void setFillColor(const QColor &color) = 0;
    virtual QColor borderColor() const = 0;
    virtual void setBorderColor(const QColor &color) = 0;
    virtual qreal borderWidth() const = 0;
    virtual void setBorderWidth(qreal width) = 0;

    // QGeoMapObjectPrivate interface
    bool equals(const QGeoMapObjectPrivate &other) const override;
    virtual QGeoShape geoShape() const override;
    virtual void setGeoShape(const QGeoShape &shape) override;
};

class Q_LOCATION_PRIVATE_EXPORT QMapPolygonObjectPrivateDefault : public QMapPolygonObjectPrivate
{
public:
    QMapPolygonObjectPrivateDefault(QGeoMapObject *q);
    QMapPolygonObjectPrivateDefault(const QMapPolygonObjectPrivate &other);
    ~QMapPolygonObjectPrivateDefault() override;

    // QMapPolygonObjectPrivate interface
    QList<QGeoCoordinate> path() const override;
    void setPath(const QList<QGeoCoordinate> &path) override;
    QColor fillColor() const override;
    void setFillColor(const QColor &color) override;
    QColor borderColor() const override;
    void setBorderColor(const QColor &color) override;
    qreal borderWidth() const override;
    void setBorderWidth(qreal width) override;

    // QGeoMapObjectPrivate interface
    QGeoMapObjectPrivate *clone() override;
    virtual QGeoShape geoShape() const override;
    virtual void setGeoShape(const QGeoShape &shape) override;

public:
    QGeoPolygon m_path; // small overhead compared to plain QList<QGeoCoordinate>
    QColor m_borderColor = Qt::transparent;
    QColor m_fillColor = Qt::transparent;
    qreal m_borderWidth = 0;

private:
    QMapPolygonObjectPrivateDefault(const QMapPolygonObjectPrivateDefault &other) = delete;
};

QT_END_NAMESPACE


#endif // QMAPPOLYGONOBJECT_P_P_H
