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

#ifndef PLACE_MACRO_H
#define PLACE_MACRO_H

#include <QtCore/QtGlobal>

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

QT_END_NAMESPACE

#endif
