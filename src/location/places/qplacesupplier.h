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

#ifndef QPLACESUPPLIER_H
#define QPLACESUPPLIER_H

#include <QtLocation/qlocationglobal.h>
#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>

QT_BEGIN_NAMESPACE

class QUrl;
class QPlaceIcon;
class QPlaceSupplierPrivate;

class Q_LOCATION_EXPORT QPlaceSupplier
{
public:
    QPlaceSupplier();
    QPlaceSupplier(const QPlaceSupplier &other);
    ~QPlaceSupplier();

    QPlaceSupplier &operator=(const QPlaceSupplier &other);

    bool operator==(const QPlaceSupplier &other) const;
    bool operator!=(const QPlaceSupplier &other) const {
        return !(other == *this);
    }

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
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceSupplier)

#endif // QPLACESUPPLIER_H
