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

#ifndef QPLACECONTENTREQUEST_H
#define QPLACECONTENTREQUEST_H

#include <QtCore/QSharedDataPointer>

#include <QtLocation/QPlaceContent>

QT_BEGIN_NAMESPACE

class QPlaceContentRequestPrivate;

class Q_LOCATION_EXPORT QPlaceContentRequest
{
public:
    QPlaceContentRequest();
    QPlaceContentRequest(const QPlaceContentRequest &other);
    ~QPlaceContentRequest();

    QPlaceContentRequest &operator=(const QPlaceContentRequest &other);

    bool operator==(const QPlaceContentRequest &other) const;
    bool operator!=(const QPlaceContentRequest &other) const;

    QPlaceContent::Type contentType() const;
    void setContentType(QPlaceContent::Type type);

    QString placeId() const;
    void setPlaceId(const QString &identifier);

    QVariant contentContext() const;
    void setContentContext(const QVariant &context);

    int limit() const;
    void setLimit(int limit);

    void clear();

private:
    QSharedDataPointer<QPlaceContentRequestPrivate> d_ptr;
    inline QPlaceContentRequestPrivate *d_func();
    inline const QPlaceContentRequestPrivate *d_func() const;
};

QT_END_NAMESPACE

#endif
