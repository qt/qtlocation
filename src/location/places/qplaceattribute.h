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

#ifndef QPLACEATTRIBUTE_H
#define QPLACEATTRIBUTE_H

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QSharedDataPointer>

#include <QtLocation/qlocationglobal.h>

QT_BEGIN_NAMESPACE

class QPlaceAttributePrivate;
class Q_LOCATION_EXPORT QPlaceAttribute
{
public:
    static const QString OpeningHours;
    static const QString Payment;
    static const QString Provider;

    QPlaceAttribute();
    QPlaceAttribute(const QPlaceAttribute &other);
    virtual ~QPlaceAttribute();

    QPlaceAttribute &operator=(const QPlaceAttribute &other);

    bool operator==(const QPlaceAttribute &other) const;
    bool operator!=(const QPlaceAttribute &other) const;

    QString label() const;
    void setLabel(const QString &label);

    QString text() const;
    void setText(const QString &text);

    bool isEmpty() const;

protected:
    QSharedDataPointer<QPlaceAttributePrivate> d_ptr;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceAttribute)

#endif
