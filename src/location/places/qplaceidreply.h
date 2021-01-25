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

#ifndef QPLACEIDREPLY_H
#define QPLACEIDREPLY_H

#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlace>

QT_BEGIN_NAMESPACE

class QPlaceIdReplyPrivate;
class Q_LOCATION_EXPORT QPlaceIdReply : public QPlaceReply
{
    Q_OBJECT
public:
    enum OperationType
    {
        SavePlace,
        SaveCategory,
        RemovePlace,
        RemoveCategory
    };

    explicit QPlaceIdReply(OperationType operationType, QObject *parent = nullptr);
    virtual ~QPlaceIdReply();

    QPlaceReply::Type type() const;
    OperationType operationType() const;

    QString id() const;

protected:
    void setId(const QString &identifier);
private:
    Q_DISABLE_COPY(QPlaceIdReply)
    Q_DECLARE_PRIVATE(QPlaceIdReply)
};

QT_END_NAMESPACE

#endif
