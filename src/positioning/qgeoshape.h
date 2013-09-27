/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
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

class Q_POSITIONING_EXPORT QGeoShape
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

    void extendShape(const QGeoCoordinate &coordinate);

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
Q_POSITIONING_EXPORT QDebug operator<<(QDebug, const QGeoShape &);
#endif

#ifndef QT_NO_DATASTREAM
Q_POSITIONING_EXPORT QDataStream &operator<<(QDataStream &stream, const QGeoShape &shape);
Q_POSITIONING_EXPORT QDataStream &operator>>(QDataStream &stream, QGeoShape &shape);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoShape)

#endif

