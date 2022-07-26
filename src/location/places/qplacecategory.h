/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#ifndef QPLACECATEGORY_H
#define QPLACECATEGORY_H

#include <QtLocation/qlocationglobal.h>

#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtLocation/QLocation>

QT_BEGIN_NAMESPACE

class QPlaceIcon;

class QPlaceCategoryPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QPlaceCategoryPrivate, Q_LOCATION_EXPORT)

class Q_LOCATION_EXPORT QPlaceCategory
{
public:
    QPlaceCategory();
    QPlaceCategory(const QPlaceCategory &other) noexcept;
    QPlaceCategory(QPlaceCategory &&other) noexcept = default;
    ~QPlaceCategory();

    QPlaceCategory &operator=(const QPlaceCategory &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QPlaceCategory)

    void swap(QPlaceCategory &other) noexcept { d.swap(other.d); }

    friend inline bool operator==(const QPlaceCategory &lhs, const QPlaceCategory &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator!=(const QPlaceCategory &lhs, const QPlaceCategory &rhs) noexcept
    { return !lhs.isEqual(rhs); }

    QString categoryId() const;
    void setCategoryId(const QString &identifier);

    QString name() const;
    void setName(const QString &name);

    QLocation::Visibility visibility() const;
    void setVisibility(QLocation::Visibility visibility);

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &icon);

    bool isEmpty() const;

private:
    QSharedDataPointer<QPlaceCategoryPrivate> d;

    bool isEqual(const QPlaceCategory &other) const noexcept;
};

Q_DECLARE_TYPEINFO(QPlaceCategory, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceCategory)

#endif // QPLACECATEGORY_H
