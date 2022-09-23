/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#ifndef QPLACESUPPLIER_H
#define QPLACESUPPLIER_H

#include <QtLocation/qlocationglobal.h>
#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QUrl;
class QPlaceIcon;
class QPlaceSupplierPrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QPlaceSupplierPrivate, Q_LOCATION_EXPORT)

class Q_LOCATION_EXPORT QPlaceSupplier
{
    Q_GADGET
    QML_VALUE_TYPE(supplier)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString supplierId READ supplierId WRITE setSupplierId)
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(QPlaceIcon icon READ icon WRITE setIcon)

public:
    QPlaceSupplier();
    QPlaceSupplier(const QPlaceSupplier &other) noexcept;
    QPlaceSupplier(QPlaceSupplier &&other) noexcept = default;
    ~QPlaceSupplier();

    QPlaceSupplier &operator=(const QPlaceSupplier &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QPlaceSupplier)

    void swap(QPlaceSupplier &other) noexcept { d.swap(other.d); }

    friend inline bool operator==(const QPlaceSupplier &lhs,
                                  const QPlaceSupplier &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator!=(const QPlaceSupplier &lhs,
                                  const QPlaceSupplier &rhs) noexcept
    { return !lhs.isEqual(rhs); }

    QString name() const;
    void setName(const QString &data);

    QString supplierId() const;
    void setSupplierId(const QString &identifier);

    QUrl url() const;
    void setUrl(const QUrl &data);

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &icon);

    bool isEmpty() const;

private:
    QSharedDataPointer<QPlaceSupplierPrivate> d;

    bool isEqual(const QPlaceSupplier &other) const noexcept;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceSupplier)

#endif // QPLACESUPPLIER_H
