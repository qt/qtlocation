/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOSHAPE_H
#define QGEOSHAPE_H

#include <QtCore/QSharedDataPointer>
#include <QtLocation/QGeoCoordinate>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDebug;
class QGeoShapePrivate;

class Q_LOCATION_EXPORT QGeoShape
{
public:
    QGeoShape();
    QGeoShape(const QGeoShape &other);
    ~QGeoShape();

    enum ShapeType {
        UnknownType,
        RectangleType,
        CircleType
    };

    ShapeType type() const;

    bool isValid() const;
    bool isEmpty() const;
    bool contains(const QGeoCoordinate &coordinate) const;

    bool operator==(const QGeoShape &other) const;
    bool operator!=(const QGeoShape &other) const;

    QGeoShape &operator=(const QGeoShape &other);

protected:
    QGeoShape(QGeoShapePrivate *d);

    QSharedDataPointer<QGeoShapePrivate> d_ptr;

private:
    inline QGeoShapePrivate *d_func();
    inline const QGeoShapePrivate *d_func() const;
};

Q_DECLARE_TYPEINFO(QGeoShape, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
Q_LOCATION_EXPORT QDebug operator<<(QDebug, const QGeoShape &);
#endif

#ifndef QT_NO_DATASTREAM
Q_LOCATION_EXPORT QDataStream &operator<<(QDataStream &stream, const QGeoShape &shape);
Q_LOCATION_EXPORT QDataStream &operator>>(QDataStream &stream, QGeoShape &shape);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoShape)

QT_END_HEADER

#endif

