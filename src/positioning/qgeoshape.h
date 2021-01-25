/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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
**
**
**
****************************************************************************/

#ifndef QGEOSHAPE_H
#define QGEOSHAPE_H

#include <QtCore/QSharedDataPointer>
#include <QtPositioning/QGeoCoordinate>

QT_BEGIN_NAMESPACE

class QDebug;
class QGeoShapePrivate;
class QGeoRectangle;

class Q_POSITIONING_EXPORT QGeoShape
{
    Q_GADGET
    Q_PROPERTY(ShapeType type READ type)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(bool isEmpty READ isEmpty)
    Q_ENUMS(ShapeType)

public:
    QGeoShape();
    QGeoShape(const QGeoShape &other);
    ~QGeoShape();

    enum ShapeType {
        UnknownType,
        RectangleType,
        CircleType,
        PathType,
        PolygonType
    };

    ShapeType type() const;

    bool isValid() const;
    bool isEmpty() const;
    Q_INVOKABLE bool contains(const QGeoCoordinate &coordinate) const;
    Q_INVOKABLE QGeoRectangle boundingGeoRectangle() const;
    Q_INVOKABLE QGeoCoordinate center() const;

    Q_INVOKABLE void extendShape(const QGeoCoordinate &coordinate);

    bool operator==(const QGeoShape &other) const;
    bool operator!=(const QGeoShape &other) const;

    QGeoShape &operator=(const QGeoShape &other);

    Q_INVOKABLE QString toString() const;
protected:
    QGeoShape(QGeoShapePrivate *d);

    QSharedDataPointer<QGeoShapePrivate> d_ptr;

private:
    inline QGeoShapePrivate *d_func();
    inline const QGeoShapePrivate *d_func() const;
};

Q_DECLARE_TYPEINFO(QGeoShape, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
Q_POSITIONING_EXPORT QDebug operator<<(QDebug, const QGeoShape &);
#endif

#ifndef QT_NO_DATASTREAM
Q_POSITIONING_EXPORT QDataStream &operator<<(QDataStream &stream, const QGeoShape &shape);
Q_POSITIONING_EXPORT QDataStream &operator>>(QDataStream &stream, QGeoShape &shape);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoShape)

#endif
