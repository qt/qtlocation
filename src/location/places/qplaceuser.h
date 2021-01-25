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

#ifndef QPLACEUSER_H
#define QPLACEUSER_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>
#include <QtLocation/qlocationglobal.h>

QT_BEGIN_NAMESPACE

class QPlaceUserPrivate;

class Q_LOCATION_EXPORT QPlaceUser
{
public:
    QPlaceUser();
    QPlaceUser(const QPlaceUser &other);
    ~QPlaceUser();

    QPlaceUser &operator=(const QPlaceUser &other);

    bool operator==(const QPlaceUser &other) const;
    bool operator!=(const QPlaceUser &other) const {
        return !(other == *this);
    }

    QString userId() const;
    void setUserId(const QString &identifier);

    QString name() const;
    void setName(const QString &name);

private:
    QSharedDataPointer<QPlaceUserPrivate> d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceUser)

#endif
