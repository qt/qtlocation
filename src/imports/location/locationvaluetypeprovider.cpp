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

#include "locationvaluetypeprovider.h"
#include "qdeclarativecoordinate_p.h"

QT_BEGIN_NAMESPACE

QVariant stringToCoordinate(const QString &s)
{
    QGeoCoordinate c;
    QStringList fields = s.split(QLatin1Char(','));
    switch (fields.length()) {
    case 2:
        c = QGeoCoordinate(fields.at(0).toDouble(), fields.at(1).toDouble());
        break;
    case 3:
        c = QGeoCoordinate(fields.at(0).toDouble(), fields.at(1).toDouble(), fields.at(2).toDouble());
        break;
    }

    return QVariant::fromValue(c);
}

LocationValueTypeProvider::LocationValueTypeProvider()
:   QQmlValueTypeProvider()
{
}

bool LocationValueTypeProvider::create(int type, QQmlValueType *&v)
{
    if (type == qMetaTypeId<QGeoCoordinate>()) {
        v = new CoordinateValueType;
        return true;
    }

    return false;
}

bool LocationValueTypeProvider::init(int type, void *data, size_t n)
{
    if (type == qMetaTypeId<QGeoCoordinate>()) {
        Q_ASSERT(n >= sizeof(QGeoCoordinate));
        QGeoCoordinate *c = reinterpret_cast<QGeoCoordinate *>(data);
        new (c) QGeoCoordinate();
        return true;
    }

    return false;
}

bool LocationValueTypeProvider::destroy(int type, void *data, size_t n)
{
    if (type == qMetaTypeId<QGeoCoordinate>()) {
        Q_ASSERT(n >= sizeof(QGeoCoordinate));
        QGeoCoordinate *c = reinterpret_cast<QGeoCoordinate *>(data);
        c->~QGeoCoordinate();
        return true;
    }

    return false;
}

bool LocationValueTypeProvider::copy(int type, const void *src, void *dst, size_t n)
{
    if (type == qMetaTypeId<QGeoCoordinate>()) {
        Q_ASSERT(n >= sizeof(QGeoCoordinate));
        const QGeoCoordinate *srcC = reinterpret_cast<const QGeoCoordinate *>(src);
        QGeoCoordinate *dstC = reinterpret_cast<QGeoCoordinate *>(dst);
        new (dstC) QGeoCoordinate(*srcC);
        return true;
    }

    return false;
}

bool LocationValueTypeProvider::create(int type, int argc, const void *argv[], QVariant *v)
{
    if (type == qMetaTypeId<QGeoCoordinate>()) {
        if (argc == 2) {
            const float *a = reinterpret_cast<const float *>(argv[0]);
            const float *b = reinterpret_cast<const float *>(argv[1]);
            *v = QVariant::fromValue(QGeoCoordinate(*a, *b));
            return true;
        } else if (argc == 3) {
            const float *a = reinterpret_cast<const float *>(argv[0]);
            const float *b = reinterpret_cast<const float *>(argv[1]);
            const float *c = reinterpret_cast<const float *>(argv[2]);
            *v = QVariant::fromValue(QGeoCoordinate(*a, *b, *c));
            return true;
        }
    }

    return false;
}

bool LocationValueTypeProvider::createFromString(int, const QString &, void *, size_t)
{
    return false;
}

bool LocationValueTypeProvider::createStringFrom(int, const void *, QString *)
{
    return false;
}

bool LocationValueTypeProvider::variantFromJsObject(int, QQmlV8Handle, QV8Engine *, QVariant *)
{
    return false;
}

bool LocationValueTypeProvider::equal(int type, const void *lhs, const void *rhs, size_t n)
{
    if (type == qMetaTypeId<QGeoCoordinate>()) {
        Q_ASSERT(n >= sizeof(QGeoCoordinate));
        const QGeoCoordinate *lhsC = reinterpret_cast<const QGeoCoordinate *>(lhs);
        const QGeoCoordinate *rhsC = reinterpret_cast<const QGeoCoordinate *>(rhs);
        return *lhsC == *rhsC;
    }

    return false;
}

bool LocationValueTypeProvider::store(int type, const void *src, void *dst, size_t n)
{
    if (type == qMetaTypeId<QGeoCoordinate>())
        return copy(type, src, dst, n);

    return false;
}

bool LocationValueTypeProvider::read(int srcType, const void *src, size_t n, int dstType, void *dst)
{
    if (dstType == qMetaTypeId<QGeoCoordinate>()) {
        QGeoCoordinate *dstC = reinterpret_cast<QGeoCoordinate *>(dst);
        if (srcType == qMetaTypeId<QGeoCoordinate>())
            return copy(srcType, src, dst, n);
        *dstC = QGeoCoordinate();
        return true;
    }

    return false;
}

bool LocationValueTypeProvider::write(int type, const void *src, void *dst, size_t n)
{
    if (type == qMetaTypeId<QGeoCoordinate>()) {
        Q_ASSERT(n >= sizeof(QGeoCoordinate));
        const QGeoCoordinate *srcC = reinterpret_cast<const QGeoCoordinate *>(src);
        QGeoCoordinate *dstC = reinterpret_cast<QGeoCoordinate *>(dst);
        if (*dstC != *srcC) {
            *dstC = *srcC;
            return true;
        }
    }

    return false;
}

QT_END_NAMESPACE
