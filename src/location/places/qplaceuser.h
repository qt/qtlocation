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

#ifndef QPLACEUSER_H
#define QPLACEUSER_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>
#include <QtLocation/qlocationglobal.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QPlaceUserPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QPlaceUserPrivate, Q_LOCATION_EXPORT)

class Q_LOCATION_EXPORT QPlaceUser
{
    Q_GADGET
    QML_VALUE_TYPE(user)
    Q_PROPERTY(QString userId READ userId WRITE setUserId)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QPlaceUser();
    QPlaceUser(const QPlaceUser &other) noexcept;
    QPlaceUser(QPlaceUser &&other) noexcept = default;
    ~QPlaceUser();

    QPlaceUser &operator=(const QPlaceUser &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QPlaceUser)

    void swap(QPlaceUser &other) noexcept { d.swap(other.d); }

    friend inline bool operator==(const QPlaceUser &lhs, const QPlaceUser &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator!=(const QPlaceUser &lhs, const QPlaceUser &rhs) noexcept
    { return !lhs.isEqual(rhs); }

    QString userId() const;
    void setUserId(const QString &identifier);

    QString name() const;
    void setName(const QString &name);

private:
    QSharedDataPointer<QPlaceUserPrivate> d;

    bool isEqual(const QPlaceUser &other) const noexcept;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceUser)

#endif
