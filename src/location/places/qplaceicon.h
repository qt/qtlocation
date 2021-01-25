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

#ifndef QPLACEICON_H
#define QPLACEICON_H

#include <QtLocation/qlocationglobal.h>

#include <QtCore/QUrl>
#include <QtCore/QFlags>
#include <QtCore/QMetaType>
#include <QtCore/QSize>
#include <QtCore/QSharedDataPointer>

QT_BEGIN_NAMESPACE

class QPlaceManager;

class QPlaceIconPrivate;
class Q_LOCATION_EXPORT QPlaceIcon
{
public:
    static const QString SingleUrl;

    QPlaceIcon();
    QPlaceIcon(const QPlaceIcon &other);

    ~QPlaceIcon();

    QPlaceIcon &operator=(const QPlaceIcon &other);
    bool operator == (const QPlaceIcon &other) const;
    bool operator != (const QPlaceIcon &other) const {
        return !(*this == other);
    }

    QUrl url(const QSize &size = QSize()) const;

    QPlaceManager *manager() const;
    void setManager(QPlaceManager *manager);

    QVariantMap parameters() const;
    void setParameters(const QVariantMap &parameters);

    bool isEmpty() const;

private:
    QSharedDataPointer<QPlaceIconPrivate> d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceIcon)

#endif
