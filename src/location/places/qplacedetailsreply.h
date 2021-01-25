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

#ifndef QPLACEDETAILSREPLY_H
#define QPLACEDETAILSREPLY_H

#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlace>

QT_BEGIN_NAMESPACE

class QPlaceDetailsReplyPrivate;
class Q_LOCATION_EXPORT QPlaceDetailsReply : public QPlaceReply
{
    Q_OBJECT
public:
    explicit QPlaceDetailsReply(QObject *parent = nullptr);
    virtual ~QPlaceDetailsReply();

    QPlaceReply::Type type() const;

    QPlace place() const;

protected:
    void setPlace(const QPlace &place);

private:
    Q_DISABLE_COPY(QPlaceDetailsReply)
    Q_DECLARE_PRIVATE(QPlaceDetailsReply)
};

QT_END_NAMESPACE

#endif // QPLACEDETAILSREPLY_H
