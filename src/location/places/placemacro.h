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

#ifndef PLACE_MACRO_H
#define PLACE_MACRO_H

#include <QtCore/QtGlobal>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



#define Q_DECLARE_D_FUNC(Class) \
    inline Class##Private *d_func(); \
    inline const Class##Private *d_func() const;\
    friend class Class##Private;

#define Q_DECLARE_COPY_CTOR(Class, BaseClass) \
    Class(const BaseClass &other);

#define Q_IMPLEMENT_D_FUNC(Class) \
    Class##Private *Class::d_func() { return reinterpret_cast<Class##Private *>(d_ptr.data()); } \
    const Class##Private *Class::d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr.constData()); }

#define Q_IMPLEMENT_COPY_CTOR(Class, BaseClass) \
    Class::Class(const BaseClass &other) : BaseClass() { Class##Private::copyIfPossible(d_ptr, other); }

#define Q_DEFINE_PRIVATE_HELPER(Class, BaseClass, ClassType) \
    BaseClass##Private *clone() const { return new Class##Private(*this); } \
    static void copyIfPossible(QSharedDataPointer<BaseClass##Private> &d_ptr, const BaseClass &other) \
    { \
        if (other.type() == ClassType) \
            d_ptr = extract_d(other); \
        else \
            d_ptr = new Class##Private; \
    }

#endif

QT_END_NAMESPACE

QT_END_HEADER
