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

#ifndef QPLACECONTENTREPLY_H
#define QPLACECONTENTREPLY_H

#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceContent>
#include <QtLocation/QPlaceContentRequest>

QT_BEGIN_NAMESPACE

class QPlaceContentReplyPrivate;
class Q_LOCATION_EXPORT QPlaceContentReply : public QPlaceReply
{
    Q_OBJECT

public:
    explicit QPlaceContentReply(QObject *parent = nullptr);
    virtual ~QPlaceContentReply();

    QPlaceReply::Type type() const;

    QPlaceContent::Collection content() const;

    int totalCount() const;

    QPlaceContentRequest request() const;

    QPlaceContentRequest previousPageRequest() const;
    QPlaceContentRequest nextPageRequest() const;

protected:
    void setContent(const QPlaceContent::Collection &content);
    void setTotalCount(int total);
    void setRequest(const QPlaceContentRequest &request);
    void setPreviousPageRequest(const QPlaceContentRequest &previous);
    void setNextPageRequest(const QPlaceContentRequest &next);

private:
    Q_DISABLE_COPY(QPlaceContentReply)
    Q_DECLARE_PRIVATE(QPlaceContentReply)
};

QT_END_NAMESPACE

#endif
