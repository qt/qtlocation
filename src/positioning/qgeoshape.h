/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
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
    Q_PROPERTY(QGeoCoordinate center READ center)
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
    QGeoCoordinate center() const;

    friend bool operator==(const QGeoShape &lhs, const QGeoShape &rhs)
    {
        return equals(lhs, rhs);
    }
    friend bool operator!=(const QGeoShape &lhs, const QGeoShape &rhs)
    {
        return !equals(lhs, rhs);
    }

    QGeoShape &operator=(const QGeoShape &other);

    Q_INVOKABLE QString toString() const;
protected:
    QGeoShape(QGeoShapePrivate *d);

    QSharedDataPointer<QGeoShapePrivate> d_ptr;

private:
    static bool equals(const QGeoShape &lhs, const QGeoShape &rhs);
    inline QGeoShapePrivate *d_func();
    inline const QGeoShapePrivate *d_func() const;
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const QGeoShape &shape)
    {
        return debugStreaming(dbg, shape);
    }
    static QDebug debugStreaming(QDebug dbg, const QGeoShape &shape);
#endif
#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &stream, const QGeoShape &shape)
    {
        return dataStreamOut(stream, shape);
    }
    friend QDataStream &operator>>(QDataStream &stream, QGeoShape &shape)
    {
        return dataStreamIn(stream, shape);
    }
    static QDataStream &dataStreamOut(QDataStream &stream, const QGeoShape &shape);
    static QDataStream &dataStreamIn(QDataStream &stream, QGeoShape &shape);
#endif
    friend Q_POSITIONING_EXPORT size_t qHash(const QGeoShape &key, size_t seed) noexcept;
};

Q_POSITIONING_EXPORT size_t qHash(const QGeoShape &shape, size_t seed = 0) noexcept;

Q_DECLARE_TYPEINFO(QGeoShape, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoShape)

#endif
