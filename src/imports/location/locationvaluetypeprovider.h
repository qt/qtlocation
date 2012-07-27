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

#ifndef LOCATIONVALUETYPEPROVIDER_H
#define LOCATIONVALUETYPEPROVIDER_H

#include <QtQml/private/qqmlglobal_p.h>

QT_BEGIN_NAMESPACE

QVariant stringToCoordinate(const QString &s);

class LocationValueTypeProvider : public QQmlValueTypeProvider
{
public:
    LocationValueTypeProvider();

private:
    Q_DECL_OVERRIDE bool create(int type, QQmlValueType *&v);

    Q_DECL_OVERRIDE bool init(int type, void *data, size_t n);
    Q_DECL_OVERRIDE bool destroy(int type, void *data, size_t n);
    Q_DECL_OVERRIDE bool copy(int type, const void *src, void *dst, size_t n);

    Q_DECL_OVERRIDE bool create(int type, int argc, const void *argv[], QVariant *v);
    Q_DECL_OVERRIDE bool createFromString(int type, const QString &s, void *data, size_t n);
    Q_DECL_OVERRIDE bool createStringFrom(int type, const void *data, QString *s);

    Q_DECL_OVERRIDE bool variantFromJsObject(int type, QQmlV8Handle h, QV8Engine *e, QVariant *v);

    Q_DECL_OVERRIDE bool equal(int type, const void *lhs, const void *rhs, size_t n);
    Q_DECL_OVERRIDE bool store(int type, const void *src, void *dst, size_t n);
    Q_DECL_OVERRIDE bool read(int srcType, const void *src, size_t n, int dstType, void *dst);
    Q_DECL_OVERRIDE bool write(int type, const void *src, void *dst, size_t n);
};

QT_END_NAMESPACE

#endif // LOCATIONVALUETYPEPROVIDER_H
